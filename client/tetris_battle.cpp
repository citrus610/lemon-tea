#include "tetris_battle.h"

namespace TetrisGame
{

TetrisBattle::TetrisBattle()
{
    this->bag.reserve(PLAYER_PRE_GENERATE_BAG_SIZE);
    while (true)
    {
        LemonTea::PieceType init_bag[7] = {
            LemonTea::PIECE_I,
            LemonTea::PIECE_J,
            LemonTea::PIECE_L,
            LemonTea::PIECE_S,
            LemonTea::PIECE_Z,
            LemonTea::PIECE_T,
            LemonTea::PIECE_O
        };
        for (int i = 0; i < 7; ++i) {
            LemonTea::PieceType value = init_bag[i];
            int swap = rand() % 7;
            init_bag[i] = init_bag[swap];
            init_bag[swap] = value;
        }
        for (int i = 0; i < 7; ++i) {
            this->bag.push_back(init_bag[i]);
            if (this->bag.size() >= PLAYER_PRE_GENERATE_BAG_SIZE) break;
        }
        if (this->bag.size() >= PLAYER_PRE_GENERATE_BAG_SIZE) break;
    }

    this->player_1.set_enemy(&this->player_2);
    this->player_2.set_enemy(&this->player_1);
    this->player_1.init(this->bag);
    this->player_2.init(this->bag);

    this->movement_1.reserve(32);
    this->movement_2.reserve(32);

    this->w_1 = LemonTea::DEFAULT_WEIGHT();
    this->w_2 = LemonTea::DEFAULT_WEIGHT();
}

void TetrisBattle::init(bool vs_cc)
{
    this->player_1.init(this->bag);
    this->player_2.init(this->bag);

    this->movement_1.clear();
    this->movement_2.clear();

    LemonTea::PieceType queue_1[32] = { this->player_1.current };
    LemonTea::PieceType queue_2[32] = { this->player_2.current };
    memcpy(queue_1 + 1, this->player_1.next.data(), this->player_1.next.size() * sizeof(LemonTea::PieceType));
    memcpy(queue_2 + 1, this->player_2.next.data(), this->player_2.next.size() * sizeof(LemonTea::PieceType));

    this->bot_1.init_thread({ .weight = this->w_1, .forecast = false }, queue_1, (int)this->player_1.next.size());
    this->bot_2.init_thread({ .weight = this->w_2, .forecast = false }, queue_2, (int)this->player_2.next.size());
}

void TetrisBattle::update()
{
    update_player(this->player_1, this->bot_1, this->movement_1);
    this->player_1.update();

    update_player(this->player_2, this->bot_2, this->movement_2);
    this->player_2.update();
}

void TetrisBattle::update_player(TetrisPlayer& player, LemonTea::Bot& bot, std::vector<LemonTea::MoveType>& movement)
{
    if (player.gameover) return;
    if (player.is_clearline()) return;
    
    if (movement.empty()) {
        // Request solution
        LemonTea::BotSolution solution;
        if (!bot.request_solution(solution, player.incomming_garbage)) {
            return;
        }

        // Check garbage or misdrop
        if (!(solution.original_board == player.board)) {
            bot.reset_state(player.board, player.b2b, player.ren);
            return;
        }

        // Hold if ok
        bool first_hold = (solution.action.hold) && (player.hold == LemonTea::PIECE_NONE);
        if (solution.action.hold) player.do_hold();
        //assert(placement.type == player.current);

        // Set move vector
        for (int i = 0; i < solution.move_count; ++i) {
            if (!movement.empty()) {
                if (movement.back() == solution.move[i]) {
                    movement.push_back((LemonTea::MoveType)100);
                }
            }
            movement.push_back(solution.move[i]);
        }

        // Advance tree
        LemonTea::BotAction action;
        action.action = solution.action;
        action.new_piece_count = 1 + first_hold;
        if (first_hold) {
            action.new_piece[0] = player.next[player.next.size() - 2];
            action.new_piece[1] = player.next[player.next.size() - 1];
        }
        else {
            action.new_piece[0] = player.next[player.next.size() - 1];
        }
        bot.advance_state(action);
        //assert(ad_suc);
    }
    if (!movement.empty()) {
        if (movement[0] != LemonTea::MOVE_DOWN) player.softdrop_cnter = 0;
        switch (movement[0])
        {
        case LemonTea::MOVE_RIGHT:
            player.do_right();
            movement.erase(movement.begin());
            break;
        case LemonTea::MOVE_LEFT:
            player.do_left();
            movement.erase(movement.begin());
            break;
        case LemonTea::MOVE_CW:
            player.do_rotate(true);
            movement.erase(movement.begin());
            break;
        case LemonTea::MOVE_CCW:
            player.do_rotate(false);
            movement.erase(movement.begin());
            break;
        case LemonTea::MOVE_DOWN:
            if ((int)movement.size() == 1) {
                player.do_drop();
                movement.clear();
                player.softdrop_cnter = 0;
            }
            else {
                ++player.softdrop_cnter;
                if (player.softdrop_cnter % PLAYER_DELAY_SOFTDROP == 0) player.do_down();
                if (player.board.get_drop_distance(player.piece) == 0) movement.erase(movement.begin());
            }
            break;
        case (LemonTea::MoveType)100:
            movement.erase(movement.begin());
            break;
        default:
            break;
        }
    }
}

void TetrisBattle::unload()
{
    this->bot_1.end_thread();
    this->bot_2.end_thread();
}

bool TetrisBattle::is_gameover()
{
    return player_1.gameover || player_2.gameover;
}

int TetrisBattle::get_winner()
{
    return 1 * (!player_1.gameover) + 2 * (!player_2.gameover);
}

}