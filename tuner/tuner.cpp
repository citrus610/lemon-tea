#include "tuner.h"

Tuner::Tuner()
{
    this->battle.init();
    this->compare.data = LemonTea::SaveData();
    this->rng = Spsa();
}

void Tuner::start()
{
    // Variables
    int starting_batch = 0;

    // Get mode
    using namespace std;
    system("CLS");
    int mode;
    cout << "What do you want to do?" << endl;
    cout << "    1 - Improve standard" << endl;
    cout << "    2 - Continue from previous" << endl;
    cin >> mode;

    // Set mode
    if (mode == 1) {
        this->compare.data = LemonTea::SaveData();
        this->compare.data.base = LemonTea::DEFAULT_WEIGHT();
        this->rng.vary_weight(this->compare.data.base, this->compare.data.v1, this->compare.data.v2);
        this->compare.save_json(this->compare.data, 0);
    }
    else if (mode == 2) {
        system("CLS");
        cout << "What batch?" << endl;
        cin >> starting_batch;
        this->compare.load_json(this->compare.data, starting_batch);
    }
    else return;

    // Get number of threads
    int thread_cnt = 0;
    system("CLS");
    cout << "How many threads?" << endl;
    cin >> thread_cnt;
    system("CLS");

    // Main loop
    while (true)
    {
        // Compare weights
        this->compare.start(this->compare.data.base, this->compare.data.v1, this->compare.data.v2, COMPARE_MAX_BATTLE, starting_batch, thread_cnt);

        // Set new weight
        LemonTea::Weight winner;
        if (this->compare.data.win_v1 >= this->compare.data.win_v2) {
            winner = this->compare.data.v1;
        }
        else {
            winner = this->compare.data.v2;
        }
        this->compare.data.next = this->compare.data.base;
        this->rng.approach_weight(this->compare.data.next, winner);
        this->compare.save_json(this->compare.data, starting_batch);

        // Restart data
        ++starting_batch;
        this->compare.data.base = this->compare.data.next;
        this->compare.data.next = LemonTea::Weight();
        this->rng.vary_weight(this->compare.data.base, this->compare.data.v1, this->compare.data.v2);
        this->compare.data.total = 0;
        this->compare.data.win_v1 = 0;
        this->compare.data.win_v2 = 0;
        this->compare.save_json(this->compare.data, starting_batch);
    }
}
