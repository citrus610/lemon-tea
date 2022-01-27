#include "pathfinder.h"

namespace LemonTea
{

bool PathFinderNode::operator < (PathFinderNode& other)
{
    if (this->input == other.input) {
        return this->path.get_size() < other.path.get_size();
    }
    return this->input < other.input;
};

bool PathFinderNode::operator > (PathFinderNode& other)
{
    if (this->input == other.input) {
        return this->path.get_size() > other.path.get_size();
    }
    return this->input > other.input;
};

bool PathFinderNode::operator == (PathFinderNode& other)
{
    return this->input == other.input && this->path.get_size() == other.path.get_size();
};

bool PathFinder::search(Board& board, Piece& placement, MoveType list[32], int& list_count)
{
    // Sanity
    list_count = 0;
    if (placement.type == PIECE_NONE ||
        board.is_colliding(placement.x, placement.y, placement.type, placement.rotation) ||
        board.get_drop_distance(placement) > 0
        ) {
        list_count = 1;
        list[0] = MOVE_DOWN;
        assert(false);
        return false;
    }
    Piece placement_normalize = placement.get_normalize();

    // Find on stack paths
    std::vector<PathFinderNode> on_stack;
    PathFinder::generate_onstack(board, placement.type, on_stack);
    if (board.is_above_stack(placement)) {
        bool found_path = false;
        PathFinderNode result_node;
        for (int i = 0; i < int(on_stack.size()); ++i) {
            if (placement_normalize == on_stack[i].placement.get_normalize()) {
                if (!found_path) {
                    result_node = on_stack[i];
                    found_path = true;
                }
                else {
                    if (on_stack[i] < result_node) {
                        result_node = on_stack[i];
                    }
                }
            }
        }
        if (found_path) {
            memcpy(list, result_node.path.iter_begin(), sizeof(MoveType) * result_node.path.get_size());
            list_count = result_node.path.get_size();
            if (list[list_count - 1] != MOVE_DOWN) {
                list[list_count] = MOVE_DOWN;
                list_count += 1;
            }
            return true;
        }
    }

    // Find underground position level 1
    std::vector<PathFinderNode> under_stack_1;
    PathFinder::generate_understack(board, on_stack, under_stack_1);
    for (int i = 0; i < int(under_stack_1.size()); ++i) {
        if (placement_normalize == under_stack_1[i].placement.get_normalize()) {
            memcpy(list, under_stack_1[i].path.iter_begin(), sizeof(MoveType) * under_stack_1[i].path.get_size());
            list_count = under_stack_1[i].path.get_size();
            if (list[list_count - 1] != MOVE_DOWN) {
                list[list_count] = MOVE_DOWN;
                list_count += 1;
            }
            return true;
        }
    }

    // Find underground position level 2
    std::vector<PathFinderNode> under_stack_2;
    PathFinder::generate_understack(board, under_stack_1, under_stack_2);
    for (int i = 0; i < int(under_stack_2.size()); ++i) {
        if (placement_normalize == under_stack_2[i].placement.get_normalize()) {
            memcpy(list, under_stack_2[i].path.iter_begin(), sizeof(MoveType) * under_stack_2[i].path.get_size());
            list_count = under_stack_2[i].path.get_size();
            if (list[list_count - 1] != MOVE_DOWN) {
                list[list_count] = MOVE_DOWN;
                list_count += 1;
            }
            return true;
        }
    }

    list[0] = MOVE_DOWN;
    list_count = 1;
    assert(false);
    return false;
};

bool PathFinder::attempt(Board& board, PathFinderNode& parent, PathFinderNode& child, MoveType move)
{
    child = parent;
    bool success = false;
    switch (move)
    {
    case MOVE_RIGHT:
        success = child.placement.move_right(board);
        break;
    case MOVE_LEFT:
        success = child.placement.move_left(board);
        break;
    case MOVE_CW:
        success = child.placement.move_cw(board);
        break;
    case MOVE_CCW:
        success = child.placement.move_ccw(board);
        break;
    case MOVE_DOWN:
        success = board.get_drop_distance(child.placement) > 0;
        child.placement.move_drop(board);
        break;
    default:
        break;
    }
    if (!success) {
        return false;
    }
    if (child.path[child.path.get_size() - 1] == move) {
        child.input += 2;
    }
    else {
        child.input += 1;
    }
    if (child.path[child.path.get_size() - 1] == MOVE_CW || child.path[child.path.get_size() - 1] == MOVE_CCW) {
        if (move == MOVE_DOWN) {
            child.input -= 1;
        }
    }
    child.path.add(move);
    return true;
};

void PathFinder::expand(Board& board, PathFinderNode& parent, arrayvec<PathFinderNode, 4>& children)
{
    children.clear();

    PathFinderNode c_right;
    if (PathFinder::attempt(board, parent, c_right, MOVE_RIGHT)) {
        children.add(c_right);
    }

    PathFinderNode c_left;
    if (PathFinder::attempt(board, parent, c_left, MOVE_LEFT)) {
        children.add(c_left);
    }

    if (parent.placement.type != PIECE_O) {
        PathFinderNode c_cw;
        if (PathFinder::attempt(board, parent, c_cw, MOVE_CW)) {
            children.add(c_cw);
        }

        PathFinderNode c_ccw;
        if (PathFinder::attempt(board, parent, c_ccw, MOVE_CCW)) {
            children.add(c_ccw);
        }
    }
};

int PathFinder::index(PathFinderNode& node, std::vector<PathFinderNode>& vec)
{
    for (int i = 0; i < int(vec.size()); ++i) {
        if (node.placement == vec[i].placement) {
            return i;
        }
    }
    return -1;
};

void PathFinder::generate_onstack(Board& board, PieceType type, std::vector<PathFinderNode>& position)
{
    position.reserve(64);
    position.clear();

    std::vector<PathFinderNode> open;
    std::vector<PathFinderNode> close;
    open.reserve(128);
    close.reserve(128);

    PathFinderNode spawn;
    spawn.placement = {
        .x = 4,
        .y = 19,
        .type = type,
        .rotation = PIECE_UP
    };

    if (board.is_colliding(4, 19, type, PIECE_UP)) {
        spawn.placement.y = 20;
        if (board.is_colliding(4, 20, type, PIECE_UP)) {
            return;
        }
    }

    open.push_back(spawn);

    spawn.placement.move_drop(board);
    spawn.path.add(MOVE_DOWN);
    position.push_back(spawn);

    while (!open.empty())
    {
        PathFinderNode parent = open[0];
        open[0] = open.back();
        open.pop_back();

        arrayvec<PathFinderNode, 4> children = arrayvec<PathFinderNode, 4>();
        PathFinder::expand(board, parent, children);

        for (int i = 0; i < children.get_size(); ++i) {
            int open_index = PathFinder::index(children[i], open);
            int close_index = PathFinder::index(children[i], close);

            if (open_index == -1 && close_index == -1) {
                open.push_back(children[i]);
            }

            if (open_index == -1 && close_index != -1) {
                if (!(children[i] > close[close_index])) {
                    close[close_index] = children[i];
                    open.push_back(children[i]);
                }
            }

            if (open_index != -1 && close_index == -1) {
                if (children[i] < open[open_index]) {
                    open[open_index] = children[i];
                }
                else if (children[i] == open[open_index]) {
                    open.push_back(children[i]);
                }
            }

            if (open_index != -1 && close_index != -1) {
                if (!(children[i] > close[close_index])) {
                    close[close_index] = children[i];
                }
                if (children[i] < open[open_index]) {
                    open[open_index] = children[i];
                }
                if (children[i] == open[open_index]) {
                    open.push_back(children[i]);
                }
            }

            PathFinderNode child_drop;
            PathFinder::attempt(board, children[i], child_drop, MOVE_DOWN);

            int child_drop_index = PathFinder::index(child_drop, position);
            if (child_drop_index == -1){
                position.push_back(child_drop);
            }
            else {
                if (child_drop < position[child_drop_index]) {
                    position[child_drop_index] = child_drop;
                }
            }
        }

        close.push_back(parent);
    }
};

void PathFinder::generate_understack(Board& board, std::vector<PathFinderNode>& input, std::vector<PathFinderNode>& position)
{
    position.reserve(128);
    position.clear();

    for (int i = 0; i < int(input.size()); ++i) {
        arrayvec<PathFinderNode, 4> children = arrayvec<PathFinderNode, 4>();
        PathFinder::expand(board, input[i], children);

        for (int c = 0; c < children.get_size(); ++c) {
            if (board.is_above_stack(children[c].placement)) {
                continue;
            }

            PathFinderNode child_drop;
            PathFinder::attempt(board, children[c], child_drop, MOVE_DOWN);

            if (PathFinder::index(child_drop, position) == -1) {
                position.push_back(child_drop);
            }
        }
    }
};

};