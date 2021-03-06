#include "compare.h"

void Compare::save_json(LemonTea::SaveData& save_data, int batch_id)
{
    std::string batch_name = std::string("batch/batch_") + std::to_string(batch_id) + std::string(".json");
    std::ofstream o(batch_name, std::ofstream::out | std::ofstream::trunc);
    json js;
    to_json(js, save_data);
    o << std::setw(4) << js << std::endl;
    o.close();
}

void Compare::load_json(LemonTea::SaveData& save_data, int batch_id)
{
    std::string batch_name = std::string("batch/batch_") + std::to_string(batch_id) + std::string(".json");
    std::ifstream file;
    file.open(batch_name);
    json js;
    file >> js;
    file.close();
    from_json(js, save_data);
}

void Compare::start(LemonTea::Weight base, LemonTea::Weight w1, LemonTea::Weight w2, int total, int batch_id, int thread)
{
    // Thread
    int thread_count = std::max(0, thread - 1);
    std::vector<std::thread*> threads;
    for (int i = 0; i < thread_count; ++i) {
        threads.push_back(nullptr);
    }

    // Data
    this->data.base = base;
    this->data.v1 = w1;
    this->data.v2 = w2;
    save_json(this->data, batch_id);

    // Func
    auto func = [&](int total_cnt, int batch_cnt) {
        // Set id
        int id = batch_cnt;

        // Init battle
        Battle battle;
        LemonTea::Weight w1;
        LemonTea::Weight w2;
        {
            std::unique_lock<std::mutex> lk(mutex);
            w1 = this->data.v1;
            w2 = this->data.v2;
        }

        // Main loop
        while (true)
        {
            {
                std::unique_lock<std::mutex> lk(mutex);
                double win = (double)this->data.win_v1;
                double loss = (double)this->data.win_v2;
                double draw = (double)(this->data.total - this->data.win_v1 - this->data.win_v2);

                // Draw to console prettily
                system("cls");
                std::cout << std::fixed << std::setprecision(2);
                std::cout << "GEN: " << id << std::endl;
                std::cout << "PROGRESS: " << (double(this->data.total) / double(total_cnt) * 100.0) << "%" << std::endl;
                std::cout << "------------------------------------------" << std::endl;
                std::cout << "ELO: " 
                    << Sprt::win_rate_to_elo_diff((win + draw / 2.0) / double(this->data.total)) 
                    << " +- " << Sprt::win_rate_to_elo_error_margin(win, draw, loss)
                    << " (95%)"
                    << std::endl;
                std::cout << "LLR: " 
                    << Sprt::log_likelihood_ratio_approximate(win, draw, loss, -5.0, 5.0)
                    << " (-2.94, 2.94) [-5, 5]"
                    << std::endl;
                std::cout << "Total: " << this->data.total
                    << " W: " << this->data.win_v1
                    << " D: " << int(draw)
                    << " L: " << this->data.win_v2
                    << std::endl;

                // If sprt ok or reach max number of match, break
                if (Sprt::sprt(win, draw, loss, -5.0, 5.0, 0.15, 0.15) != SPRT_NULL) {
                    save_json(this->data, id);
                    break;
                }
                if (this->data.total >= total_cnt) {
                    save_json(this->data, id);
                    break;
                }
            }

            // Init battle
            battle.init();
            battle.bot_1.evaluator.weight = w1;
            battle.bot_2.evaluator.weight = w2;

            // Update battle
            bool draw = false;
            int frame = 0;
            while (!battle.is_gameover()) {
                if (frame > COMPARE_MAX_BATTLE_FRAME) {
                    draw = true;
                    break;
                }
                ++frame;
                battle.update();
            }

            // End
            int winner_id = -1;
            if (!draw) winner_id = battle.get_winner();
            {
                std::unique_lock<std::mutex> lk(mutex);
                this->data.total++;
                if (!draw) {
                    if (winner_id == 1) this->data.win_v1++;
                    if (winner_id == 2) this->data.win_v2++;
                }
                if (this->data.total % 10 == 0) {
                    save_json(this->data, id);
                }
            }
        }
    };

    // Start sub threads
    for (int i = 0; i < thread_count; ++i) {
        threads[i] = new std::thread(func, total, batch_id);
        std::cout << "Thread #" << (i + 1) << " started!" << std::endl;
    }

    // Start main thread
    std::cout << "Thread #0 started!" << std::endl;
    func(total, batch_id);
    std::cout << "Thread #0 ended!" << std::endl;

    // Wait sub threads end
    for (int i = 0; i < thread_count; ++i) {
        threads[i]->join();
        delete threads[i];
        threads[i] = nullptr;
        std::cout << "Thread #" << (i + 1) << " ended!" << std::endl;
    }
}
