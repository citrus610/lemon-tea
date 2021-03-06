#include "eval.h"

namespace LemonTea
{

void Evaluator::evaluate(Node& node, PieceType* queue, int queue_size, PieceType* bag, int bag_size)
{
    //
    // Defence evaluation
    //
    node.score.defence = 0;
    Board board = node.state.board;

    // Get column heights
    int column_height[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    board.get_height(column_height);

    // Height top of the board
    int max_height = *std::max_element(column_height, column_height + 10);
    node.score.defence += std::max(max_height - 10, 0) * this->weight.defence.height_10;
    node.score.defence += std::max(max_height - 15, 0) * this->weight.defence.height_15;

    // Quiescence
    int tspin_structure[4] = { 0, 0, 0, 0 };
    int donation_depth = (node.state.hold == PIECE_T) + (node.state.current == PIECE_T);
    for (int i = node.state.next; i < queue_size; ++i) {
        if (queue[i] == PIECE_T) {
            ++donation_depth;
        }
    }
    for (int i = 0; i < bag_size; ++i) {
        if (bag[i] == PIECE_T) {
            ++donation_depth;
            break;
        }
    }
    Evaluator::donation(board, column_height, donation_depth, tspin_structure);

    // Structure
    node.score.defence += tspin_structure[0] * this->weight.defence.structure[0];
    node.score.defence += tspin_structure[1] * this->weight.defence.structure[1];
    node.score.defence += tspin_structure[2] * this->weight.defence.structure[2];
    node.score.defence += tspin_structure[3] * this->weight.defence.structure[3];

    // Height normal
    max_height = *std::max_element(column_height, column_height + 10);
    node.score.defence += max_height * this->weight.defence.height;

    // Well
    int well_position = 0;
    int well_depth = Evaluator::well(board, column_height, well_position);
    int min_height = column_height[well_position];
    node.score.defence += well_depth * this->weight.defence.well;

    // Side well
    if (well_position == 0 || well_position == 9) {
        node.score.defence += well_depth * this->weight.defence.sidewell;
    }

    // Perfect tetris
    int ptetris_depth = (node.state.hold == PIECE_I) + (node.state.current == PIECE_I);
    for (int i = node.state.next; i < queue_size; ++i) {
        if (queue[i] == PIECE_I) {
            ++ptetris_depth;
            break;
        }
    }
    for (int i = 0; i < bag_size; ++i) {
        if (bag[i] == PIECE_I) {
            ++ptetris_depth;
            break;
        }
    }
    if (ptetris_depth > 0) {
        Evaluator::perfect_tetris(board, column_height, well_position);
    }

    // Bumpiness
    int bumpiness[3] = { 0, 0, 0 };
    Evaluator::bumpiness(column_height, well_position, bumpiness);
    node.score.defence += bumpiness[0] * this->weight.defence.bumpiness;
    node.score.defence += bumpiness[1] * this->weight.defence.bumpiness_s;
    node.score.defence += bumpiness[2] * this->weight.defence.flat;

    // Row transition
    int transition_row = Evaluator::transition_row(board, column_height);
    node.score.defence += transition_row * this->weight.defence.row_t;

    // Hole above & below min height
    int hole[2] = { 0, 0 };
    Evaluator::hole(board, column_height, min_height, hole);
    node.score.defence += hole[0] * this->weight.defence.hole_a;
    node.score.defence += hole[1] * this->weight.defence.hole_b;

    // Blocked hole
    int blocked = Evaluator::blocked(board, column_height);
    node.score.defence += blocked * this->weight.defence.blocked;

    // B2B
    node.score.defence += (node.state.b2b > 0) * this->weight.defence.b2b;

    // 
    // Attack evaluation
    // 
    // Locks
    switch (node.action.lock)
    {
    case LOCK_CLEAR_1:
        node.score.attack += this->weight.attack.clear[0];
        break;
    case LOCK_CLEAR_2:
        node.score.attack += this->weight.attack.clear[1];
        break;
    case LOCK_CLEAR_3:
        node.score.attack += this->weight.attack.clear[2];
        break;
    case LOCK_CLEAR_4:
        node.score.attack += this->weight.attack.clear[3];
        break;
    case LOCK_TSPIN_1:
        node.score.attack += this->weight.attack.tspin[0];
        break;
    case LOCK_TSPIN_2:
        node.score.attack += this->weight.attack.tspin[1];
        break;
    case LOCK_TSPIN_3:
        node.score.attack += this->weight.attack.tspin[2];
        break;
    case LOCK_PC:
        node.score.attack += this->weight.attack.pc;
        break;
    default:
        break;
    }

    // Waste clear
    if (node.action.lock == LOCK_CLEAR_1 ||
        node.action.lock == LOCK_CLEAR_2 ||
        node.action.lock == LOCK_CLEAR_3) {
        if (node.state.ren == 1) {
            node.score.attack += this->weight.attack.waste_clear;
        }
    }

    // Waste time
    if (node.action.softdrop &&
        node.action.lock != LOCK_TSPIN_1 &&
        node.action.lock != LOCK_TSPIN_2 &&
        node.action.lock != LOCK_TSPIN_3 &&
        node.action.lock != LOCK_PC) {
        node.score.attack += (20 - node.action.placement.y) * this->weight.attack.waste_time;
    }

    // Waste T
    if (node.action.placement.type == PIECE_T &&
        node.action.lock != LOCK_TSPIN_1 &&
        node.action.lock != LOCK_TSPIN_2 &&
        node.action.lock != LOCK_TSPIN_3 &&
        node.action.lock != LOCK_PC) {
        node.score.attack += this->weight.attack.waste_T;
    }

    // B2B
    node.score.attack += (node.state.b2b > 1) * this->weight.attack.b2b;

    // REN
    int ren = REN_LUT[std::min(node.state.ren, MAX_COMBO_TABLE_SIZE - 1)];
    node.score.attack += ren * this->weight.attack.ren;
    if (max_height >= 10) {
        node.score.attack += ren * this->weight.attack.ren_10;
    }
    if (max_height >= 15) {
        node.score.attack += ren * this->weight.attack.ren_15;
    }
};

int Evaluator::well(Board& board, int column_height[10], int& well_position)
{
    well_position = 0;
    for (int i = 1; i < 10; ++i) {
        if (column_height[i] < column_height[well_position]) {
            well_position = i;
        }
    }
    // for (int i = 1; i < 5; ++i) {
    //     if (column_height[i] <= column_height[well_position]) {
    //         well_position = i;
    //     }
    // }
    // for (int i = 5; i < 10; ++i) {
    //     if (column_height[i] < column_height[well_position]) {
    //         well_position = i;
    //     }
    // }

    uint64_t mask = ~0b0;
    for (int i = 0; i < 10; ++i) {
        if (i == well_position) continue;
        mask = mask & board[i];
    }
    mask = mask >> column_height[well_position];
    return std::min(std::countr_one(mask), 15);
};

void Evaluator::bumpiness(int column_height[10], int well_position, int result[3])
{
    int pre_index = 0;
    if (well_position == 0) pre_index = 1;
    for (int i = 1; i < 10; ++i) {
        if (i == well_position) continue;
        int height_different = std::abs(column_height[pre_index] - column_height[i]);
        result[0] += height_different;
        result[1] += height_different * height_different;
        result[2] += (height_different == 0);
        pre_index = i;
    }
};

int Evaluator::transition_row(Board& board, int column_height[10])
{
    int result = 0;
    for (int i = 0; i < 9; ++i) {
        uint64_t xor_column = board[i] ^ board[i + 1];
        result += std::popcount(xor_column);
    }
    result += 64 - std::popcount(board[0]);
    result += 64 - std::popcount(board[9]);
    return result;
};

void Evaluator::hole(Board& board, int column_height[10], int min_height, int result[2])
{
    for (int i = 0; i < 10; ++i) {
        result[0] += column_height[i] - min_height - std::popcount(board[i] >> min_height);
        result[1] += column_height[i] - std::popcount(board[i]);
        
    }
    result[1] -= result[0];
};

int Evaluator::blocked(Board& board, int column_height[10])
{
    int result = 0;
    for (int i = 0; i < 10; ++i) {
        uint64_t hole_mask = (~board[i]) & ((1ULL << column_height[i]) - 1);
        while (hole_mask != 0)
        {
            int hole_mask_trz = std::countr_zero(hole_mask);
            result += std::min(column_height[i] - hole_mask_trz - 1, 6);
            hole_mask = hole_mask & (~(1ULL << hole_mask_trz));
        }
    }
    return result;
};

Piece Evaluator::structure(Board& board, int column_height[10])
{
    for (int _x = 0; _x < 8; ++_x) {
        if (column_height[_x + 0] > column_height[_x + 1] && column_height[_x + 0] + 1 < column_height[_x + 2]) {
            if (((board[_x + 0] >> (column_height[_x + 0] - 1)) & 0b111) == 0b001 &&
                ((board[_x + 1] >> (column_height[_x + 0] - 1)) & 0b111) == 0b000 &&
                ((board[_x + 2] >> (column_height[_x + 0] - 1)) & 0b111) == 0b101) {
                return {
                    (int8_t)(_x + 1),
                    (int8_t)column_height[_x + 0],
                    PIECE_T,
                    PIECE_DOWN
                };
            }
        }
        if (column_height[_x + 2] > column_height[_x + 1] && column_height[_x + 2] + 1 < column_height[_x + 0]) {
            if (((board[_x + 0] >> (column_height[_x + 2] - 1)) & 0b111) == 0b101 &&
                ((board[_x + 1] >> (column_height[_x + 2] - 1)) & 0b111) == 0b000 &&
                ((board[_x + 2] >> (column_height[_x + 2] - 1)) & 0b111) == 0b001) {
                return {
                    (int8_t)(_x + 1),
                    (int8_t)column_height[_x + 2],
                    PIECE_T,
                    PIECE_DOWN
                };
            }
        }
        if (column_height[_x + 1] >= column_height[_x + 0] && column_height[_x + 1] + 1 < column_height[_x + 2]) {
            if (((board[_x + 0] >> (column_height[_x + 1] - 3)) & 0b11000) == 0b00000 &&
                ((board[_x + 1] >> (column_height[_x + 1] - 3)) & 0b11110) == 0b00100 &&
                ((board[_x + 2] >> (column_height[_x + 1] - 3)) & 0b11111) == 0b10000 &&
                (
                    board.is_occupied(_x + 1, column_height[_x + 1] - 3) ||
                    (!board.is_occupied(_x + 1, column_height[_x + 1] - 3) && board.is_occupied(_x + 2, column_height[_x + 1] - 4))
                    )) {
                return {
                    (int8_t)(_x + 2),
                    (int8_t)(column_height[_x + 1] - 2),
                    PIECE_T,
                    PIECE_LEFT
                };
            }
        }
        if (column_height[_x + 1] >= column_height[_x + 2] && column_height[_x + 1] + 1 < column_height[_x + 0]) {
            if (((board[_x + 0] >> (column_height[_x + 1] - 3)) & 0b11111) == 0b10000 &&
                ((board[_x + 1] >> (column_height[_x + 1] - 3)) & 0b11110) == 0b00100 &&
                ((board[_x + 2] >> (column_height[_x + 1] - 3)) & 0b11000) == 0b00000 &&
                (
                    board.is_occupied(_x + 1, column_height[_x + 1] - 3) ||
                    (!board.is_occupied(_x + 1, column_height[_x + 1] - 3) && board.is_occupied(_x + 0, column_height[_x + 1] - 4))
                    )) {
                return {
                    (int8_t)_x,
                    (int8_t)(column_height[_x + 1] - 2),
                    PIECE_T,
                    PIECE_RIGHT
                };
            }
        }
    }
    return {
        -1,
        -1,
        PIECE_NONE,
        PIECE_UP
    };
};

void Evaluator::donation(Board& board, int column_height[10], int depth, int tspin_structure[4])
{
    for (int i = 0; i < depth; ++i) {
        Board copy = board;
        Piece quiet_piece = Evaluator::structure(copy, column_height);
        if (quiet_piece.type == PIECE_NONE) break;
        quiet_piece.place(copy);
        int line_clear = copy.clear_line();
        ++tspin_structure[line_clear];
        if (line_clear >= 2) {
            board = copy;
            board.get_height(column_height);
        }
        else {
            break;
        }
    }
};

void Evaluator::perfect_tetris(Board& board, int column_height[10], int well_index)
{
    Board copy = board;
    Piece quiet_piece = {
        .x = (int8_t)well_index,
        .y = (int8_t)(column_height[well_index] + 2),
        .type = PIECE_I,
        .rotation = PIECE_RIGHT
    };
    quiet_piece.place(copy);
    int line_clear = copy.clear_line();
    if (line_clear == 4) {
        board = copy;
        board.get_height(column_height);
    }
};

int Evaluator::ren_sum(int ren)
{
    int result = 0;
    for (int i = 3; i <= ren; ++i) {
        result += REN_LUT[std::min(i, MAX_COMBO_TABLE_SIZE - 1)];
    }
    return result;
};

int Evaluator::spike(Node& root, Node& node)
{
    // Get line clear
    Board root_copy = root.state.board;
    node.origin.placement.place(root_copy);
    int line_clear = root_copy.clear_line();

    // Get attack
    int attack = 0;
    switch (node.origin.lock)
    {
    case LOCK_CLEAR_2:
        attack = 1;
        break;
    case LOCK_CLEAR_3:
        attack = 2;
        break;
    case LOCK_CLEAR_4:
        attack = 4;
        break;
    case LOCK_TSPIN_1:
        attack = 2;
        break;
    case LOCK_TSPIN_2:
        attack = 4;
        break;
    case LOCK_TSPIN_3:
        attack = 6;
        break;
    case LOCK_PC:
        attack = 10;
        break;
    default:
        break;
    }
    if (root.state.b2b > 0 &&
        (
            node.origin.lock == LOCK_CLEAR_4 ||
            node.origin.lock == LOCK_TSPIN_1 ||
            node.origin.lock == LOCK_TSPIN_2 ||
            node.origin.lock == LOCK_TSPIN_3
            )
        ) {
        ++attack;
    }
    if (line_clear > 0) {
        attack += REN_LUT[std::min(root.state.ren, MAX_COMBO_TABLE_SIZE - 1)];
    }

    return line_clear + attack;
};

};