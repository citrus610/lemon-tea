#include "movegen.h"

namespace LemonTea
{

void PositionMap::push(const Piece& piece)
{
    this->data[piece.x][piece.rotation] |= 1 << piece.y;
};

bool PositionMap::exist(const Piece& piece)
{
    return ((this->data[piece.x][piece.rotation] >> piece.y) & 1) != 0;
};

void MoveGenerator::expand(Board& board, Piece& piece, Piece result[4], int& result_count)
{
    result_count = 0;
    Piece right = piece;
    Piece left = piece;
    if (right.move_right(board)) {
        result[result_count] = right;
        ++result_count;
    }
    if (left.move_left(board)) {
        result[result_count] = left;
        ++result_count;
    }
    if (piece.type != PIECE_O) {
        Piece cw = piece;
        Piece ccw = piece;
        if (cw.move_cw(board)) {
            result[result_count] = cw;
            ++result_count;
        }
        if (ccw.move_ccw(board)) {
            result[result_count] = ccw;
            ++result_count;
        }
    }
};

void MoveGenerator::generate(Board& board, PieceType type, Piece result[MOVE_GENERATION_POSITION_MAX], int& result_count)
{
    assert(type != PIECE_NONE);
    result_count = 0;

    // Check fast mode
    bool fast_mode = true;
    for (int i = 0; i < 10; ++i) {
        if (64 - std::countl_zero(board[i]) > 16) {
            fast_mode = false;
            break;
        }
    }

    // Find all above stack position
    Piece above_stack[MOVE_GENERATION_POSITION_MAX];
    int above_stack_count = 0;

    if (fast_mode) {
        int8_t rotation_count = 4;
        if (type == PIECE_O) rotation_count = 1;

        int8_t piece_max_rotation = 4;
        if (type == PIECE_I || type == PIECE_Z || type == PIECE_S) piece_max_rotation = 2;
        if (type == PIECE_O) piece_max_rotation = 1;

        for (int8_t rotation = 0; rotation < rotation_count; ++rotation) {
            for (int8_t x = 0; x < 10; ++x) {
                if (board.is_colliding(x, (int8_t)19, type, (PieceRotation)rotation)) {
                    continue;
                }

                Piece new_location = {
                    x,
                    (int8_t)19,
                    type,
                    (PieceRotation)rotation
                };

                new_location.move_drop(board);

                above_stack[above_stack_count] = new_location;
                ++above_stack_count;

                if (rotation < piece_max_rotation) {
                    result[result_count] = new_location;
                    ++result_count;
                }
            }
        }
    }
    else {
        PositionMap above_stack_map;
        PositionMap floating_map;
        Piece floating[MOVE_GENERATION_POSITION_MAX * 2];
        int floating_count = 0;

        Piece init_location = {
            (int8_t)4,
            (int8_t)19,
            type,
            PIECE_UP
        };

        if (board.is_colliding((int8_t)4, (int8_t)19, type, PIECE_UP)) {
            init_location.y = (int8_t)20;
            if (board.is_colliding((int8_t)4, (int8_t)20, type, PIECE_UP)) {
                return;
            }
        }

        floating_map.push(init_location);
        floating[0] = init_location;
        floating_count = 1;

        init_location.move_drop(board);
        above_stack_map.push(init_location);
        above_stack[0] = init_location;
        above_stack_count = 1;

        while (floating_count > 0)
        {
            Piece parent = floating[0];
            floating[0] = floating[floating_count - 1];
            --floating_count;

            Piece children[4];
            int children_count = 0;
            MoveGenerator::expand(board, parent, children, children_count);

            for (int i = 0; i < children_count; ++i) {
                if (!floating_map.exist(children[i])) {
                    floating_map.push(children[i]);
                    floating[floating_count] = children[i];
                    ++floating_count;

                    children[i].move_drop(board);
                    if (children[i].y >= 20) {
                        continue;
                    }
                    if (!above_stack_map.exist(children[i])) {
                        above_stack_map.push(children[i]);
                        above_stack[above_stack_count] = children[i];
                        ++above_stack_count;
                    }
                }
            }
        }

        PositionMap result_map;
        for (int i = 0; i < above_stack_count; ++i) {
            Piece normalized_position = above_stack[i].get_normalize();
            if (!result_map.exist(normalized_position)) {
                result_map.push(normalized_position);
                result[result_count] = normalized_position;
                ++result_count;
            }
        }
    }

    // Under stack level 1
    PositionMap under_stack_1_map;
    Piece under_stack_1[MOVE_GENERATION_POSITION_MAX];
    int under_stack_1_count = 0;
    for (int i = 0; i < above_stack_count; ++i) {
        Piece children[4];
        int children_count = 0;
        MoveGenerator::expand(board, above_stack[i], children, children_count);
        for (int k = 0; k < children_count; ++k) {
            if (board.is_above_stack(children[k])) {
                continue;
            }
            children[k].move_drop(board);
            if (!under_stack_1_map.exist(children[k])) {
                under_stack_1_map.push(children[k]);
                under_stack_1[under_stack_1_count] = children[k];
                ++under_stack_1_count;
            }
        }
    }

    // Under stack level 2
    PositionMap under_stack_2_map;
    Piece under_stack_2[MOVE_GENERATION_POSITION_MAX];
    int under_stack_2_count = 0;
    for (int i = 0; i < under_stack_1_count; ++i) {
        Piece children[4];
        int children_count = 0;
        MoveGenerator::expand(board, under_stack_1[i], children, children_count);
        for (int k = 0; k < children_count; ++k) {
            if (board.is_above_stack(children[k])) {
                continue;
            }
            children[k].move_drop(board);
            if (!under_stack_1_map.exist(children[k]) && !under_stack_2_map.exist(children[k])) {
                under_stack_2_map.push(children[k]);
                under_stack_2[under_stack_2_count] = children[k];
                ++under_stack_2_count;
            }
        }
    }

    // Push under stack positions to result
    PositionMap result_map;
    for (int i = 0; i < under_stack_1_count; ++i) {
        Piece normalized_position = under_stack_1[i].get_normalize();
        if (!result_map.exist(normalized_position)) {
            result_map.push(normalized_position);
            result[result_count] = normalized_position;
            ++result_count;
        }
    }
    for (int i = 0; i < under_stack_2_count; ++i) {
        Piece normalized_position = under_stack_2[i].get_normalize();
        if (!result_map.exist(normalized_position)) {
            result_map.push(normalized_position);
            result[result_count] = normalized_position;
            ++result_count;
        }
    }
};

};