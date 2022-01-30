#include "battle.h"

Battle::Battle()
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
}

void Battle::init()
{
    this->player_1.init(this->bag);
    this->player_2.init(this->bag);

    this->movement_1.clear();
    this->movement_2.clear();

    LemonTea::PieceType queue_1[32] = { this->player_1.current };
    LemonTea::PieceType queue_2[32] = { this->player_2.current };
    memcpy(queue_1 + 1, this->player_1.next.data(), this->player_1.next.size() * sizeof(LemonTea::PieceType));
    memcpy(queue_2 + 1, this->player_2.next.data(), this->player_2.next.size() * sizeof(LemonTea::PieceType));
    this->bot_1.init(queue_1, (int)this->player_1.next.size());
    this->bot_2.init(queue_2, (int)this->player_2.next.size());
}

void Battle::update()
{
    update_player(this->player_1, this->bot_1, this->movement_1);
    this->player_1.update();

    update_player(this->player_2, this->bot_2, this->movement_2);
    this->player_2.update();
}

void Battle::update_player(Player& player, LemonTea::Search& bot, std::vector<LemonTea::MoveType>& movement)
{
    if (player.gameover) return;
    if (player.is_clearline()) return;
    
    if (movement.empty()) {
        // Check garbage or misdrop
        if (!(bot.state.root.state.board == player.board)) {
            assert(false);
            bot.reset(player.board, player.b2b, player.ren);
            return;
        }

        // Search
        int bot_node = 0;
        int bot_depth = 0;
        bot.search(5, bot_node, bot_depth);

        // Get solution
        LemonTea::NodeAction action = bot.solution(player.incomming_garbage).origin;
        LemonTea::MoveType move_raw[32];
        int move_raw_cnt = 0;
        LemonTea::PathFinder::search(player.board, action.placement, move_raw, move_raw_cnt);

        // Hold if ok
        bool first_hold = (action.hold) && (player.hold == LemonTea::PIECE_NONE);
        if (action.hold) player.do_hold();
        assert(action.placement.type == player.current);

        // Set move vector
        for (int i = 0; i < move_raw_cnt; ++i) {
            if (!movement.empty()) {
                if (movement.back() == move_raw[i]) {
                    movement.push_back((LemonTea::MoveType)100);
                }
            }
            movement.push_back(move_raw[i]);
        }

        // Advance tree
        LemonTea::BotAction new_action;
        new_action.action = action;
        new_action.new_piece_count = 1 + first_hold;
        if (first_hold) {
            new_action.new_piece[0] = player.next[player.next.size() - 2];
            new_action.new_piece[1] = player.next[player.next.size() - 1];
        }
        else {
            new_action.new_piece[0] = player.next[player.next.size() - 1];
        }
        bool ad_suc = bot.advance(action, new_action.new_piece, new_action.new_piece_count);
        assert(ad_suc);
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

void Battle::render()
{
    std::string image;

    LemonTea::Board copy_1 = this->player_1.board;
    LemonTea::Board copy_2 = this->player_2.board;
    this->player_1.piece.place(copy_1);
    this->player_2.piece.place(copy_2);
    if (this->player_1.is_clearline()) {
        uint64_t mask = ~copy_1.get_mask();
        for (int i = 0; i < 10; ++i)
            copy_1[i] = copy_1[i] & mask;
    }
    if (this->player_2.is_clearline()) {
        uint64_t mask = ~copy_2.get_mask();
        for (int i = 0; i < 10; ++i)
            copy_2[i] = copy_2[i] & mask;
    }

    for (int y = 21; y >= 0; --y) {
        image.append("  ");
        if (y < this->player_1.incomming_garbage)
            image.append("[]");
        else
            image.append("  ");
        image.append("  ");

        for (int x = 0; x < 10; ++x) {
            if (copy_1.is_occupied(x, y))
                image.push_back('#');
            else
                image.push_back('.');
        }

        image.append("  ");
        if (y < this->player_1.incomming_garbage)
            image.append("[]");
        else
            image.append("  ");
        image.append("  ");

        image.append("          ");

        image.append("  ");
        if (y < this->player_2.incomming_garbage)
            image.append("[]");
        else
            image.append("  ");
        image.append("  ");

        for (int x = 0; x < 10; ++x) {
            if (copy_2.is_occupied(x, y))
                image.push_back('#');
            else
                image.push_back('.');
        }

        image.append("  ");
        if (y < this->player_2.incomming_garbage)
            image.append("[]");
        else
            image.append("  ");
        image.append("  ");

        image.append("\n");
    }

    image.append("CURRENT: ");
    image.push_back(convert_piece_to_char(this->player_1.current));
    image.append("                      ");
    image.append("CURRENT: ");
    image.push_back(convert_piece_to_char(this->player_2.current));
    image.append("\n");

    image.append("HOLD: ");
    image.push_back(convert_piece_to_char(this->player_1.hold));
    image.append("                         ");
    image.append("HOLD: ");
    image.push_back(convert_piece_to_char(this->player_2.hold));
    image.append("\n");

    image.append("QUEUE: ");
    for (int i = 0; i < PLAYER_NEXT_COUNT; ++i) {
        image.push_back(convert_piece_to_char(this->player_1.next[i]));
        image.append(" ");
    }
    image.append("                 ");
    image.append("QUEUE: ");
    for (int i = 0; i < PLAYER_NEXT_COUNT; ++i) {
        image.push_back(convert_piece_to_char(this->player_2.next[i]));
        image.append(" ");
    }
    image.append("\n");

    std::cout << image << std::endl;
}

bool Battle::is_gameover()
{
    return player_1.gameover || player_2.gameover;
}

int Battle::get_winner()
{
    return 1 * (!player_1.gameover) + 2 * (!player_2.gameover);
}