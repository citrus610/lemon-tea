#include "pathfinder.h"

bool PathFinder::search(BitBoard& board, PieceData& placement, MoveType list[32], int& list_count)
{
	// Sanity
	list_count = 0;
	if (placement.type == PIECE_NONE ||
		board.is_colliding(
			placement.x,
			placement.y,
			placement.type,
			placement.rotation
		)) {
		list_count = 1;
		list[0] = MOVE_DOWN;
		return false;
	}

	// Find all on stack positions
	std::vector<PathFinderNode> on_stack;
	PathFinder::generate_on_stack(board, placement.type, on_stack);

	// For those on stack positions, try move to find soft drop positions
	std::vector<PathFinderNode> under_stack;
	under_stack.reserve(64);
	for (int i = 0; i < (int)on_stack.size(); ++i) {
		// If current on stack position is the solution, then ok
		if (on_stack[i].position == placement) {
			memcpy(list, on_stack[i].path, on_stack[i].path_count * sizeof(MoveType));
			list_count = on_stack[i].path_count;
			return true;
		}

		// Expand
		PathFinderNode children[4];
		int children_count = 0;
		PathFinder::expand(board, on_stack[i], children, children_count);

		// For every child
		for (int k = 0; k < children_count; ++k) {
			// If above stack, then skip
			if (board.is_above_stack(children[k].position)) continue;

			// Update position by soft drop
			int distain = board.get_drop_distance(children[k].position);
			children[k].position.y -= distain;
			if (distain > 0) {
				children[k].path[children[k].path_count] = MOVE_DOWN;
				++children[k].path_count;
			}

			// If found destination
			if (children[k].position == placement) {
				memcpy(list, children[k].path, children[k].path_count * sizeof(MoveType));
				list_count = children[k].path_count;
				if (list[list_count - 1] != MOVE_DOWN) {
					list[list_count] = MOVE_DOWN;
					++list_count;
				}
				return true;
			}

			// Push if hasn't existed
			if (PathFinder::list_index(children[k], under_stack) == -1) {
				under_stack.push_back(children[k]);
			}
		}
	}

	// For those under stack positions, try move to find soft drop positions again
	if (placement.type == PIECE_T) {
		std::vector<PathFinderNode> under_stack_2;
		under_stack_2.reserve(64);
		for (int i = 0; i < (int)under_stack.size(); ++i) {
			// Expand
			PathFinderNode children[4];
			int children_count = 0;
			PathFinder::expand(board, under_stack[i], children, children_count);

			// For every child
			for (int k = 0; k < children_count; ++k) {
				// If above stack, then skip
				if (board.is_above_stack(children[k].position)) continue;

				// Update position by soft drop
				int distain = board.get_drop_distance(children[k].position);
				children[k].position.y -= distain;
				if (distain > 0) {
					children[k].path[children[k].path_count] = MOVE_DOWN;
					++children[k].path_count;
				}

				// If found destination
				if (children[k].position == placement) {
					memcpy(list, children[k].path, children[k].path_count * sizeof(MoveType));
					list_count = children[k].path_count;
					if (list[list_count - 1] != MOVE_DOWN) {
						list[list_count] = MOVE_DOWN;
						++list_count;
					}
					return true;
				}

				// Push if hasn't existed
				if (PathFinder::list_index(children[k], under_stack_2) == -1) {
					under_stack_2.push_back(children[k]);
				}
			}
		}
	}

	// If no solution found, then simply hard drop to death
	list_count = 1;
	list[0] = MOVE_DOWN;
	return false;
}

void PathFinder::calculate_input(PathFinderNode& node)
{
	node.input_count = node.path_count;
	for (int i = 0; i < node.path_count - 1; ++i) {
		if (node.path[i] == node.path[i + 1]) ++node.input_count;
	}
	if ((node.path[node.path_count - 2] == MOVE_CW ||
		node.path[node.path_count - 2] == MOVE_CCW) &&
		node.path[node.path_count - 1] == MOVE_DOWN)
		--node.input_count;
}

void PathFinder::expand(BitBoard& board, PathFinderNode& node, PathFinderNode result[4], int& result_count)
{
	result_count = 0;

	PathFinderNode right = node;
	PathFinderNode left = node;

	if (board.piece_try_right(right.position)) {
		right.path[right.path_count] = MOVE_RIGHT;
		++right.path_count;
		result[result_count] = right;
		++result_count;
	}

	if (board.piece_try_left(left.position)) {
		left.path[left.path_count] = MOVE_LEFT;
		++left.path_count;
		result[result_count] = left;
		++result_count;
	}

	if (node.position.type != PIECE_O) {
		PathFinderNode cw = node;
		PathFinderNode ccw = node;

		if (board.piece_try_rotate(cw.position, true)) {
			cw.path[cw.path_count] = MOVE_CW;
			++cw.path_count;
			result[result_count] = cw;
			++result_count;
		}

		if (board.piece_try_rotate(ccw.position, false)) {
			ccw.path[ccw.path_count] = MOVE_CCW;
			++ccw.path_count;
			result[result_count] = ccw;
			++result_count;
		}
	}
}

void PathFinder::generate_on_stack(BitBoard& board, PieceType piece, std::vector<PathFinderNode>& result)
{
	// Sanity check
	result.reserve(64);
	result.clear();

	// Init open and close vector
	std::vector<PathFinderNode> open;
	std::vector<PathFinderNode> close;
	open.reserve(128);
	close.reserve(128);

	// Find init piece position
	PathFinderNode init;
	init.position = {
		4,
		19,
		piece,
		PIECE_UP
	};
	if (board.is_colliding(4, 19, piece, PIECE_UP)) {
		init.position.y = 20;
		if (board.is_colliding(4, 20, piece, PIECE_UP)) {
			return;
		}
	}

	// Push init position to open vector
	open.push_back(init);

	// Push init hard drop position to result vector
	init.position.y -= board.get_drop_distance(init.position);
	init.path[0] = MOVE_DOWN;
	init.path_count = 1;
	result.push_back(init);

	// Search
	while (!open.empty())
	{
		// Get a parent by pop heap
		PathFinderNode parent = open[0];
		open[0] = open[open.size() - 1];
		open.pop_back();

		// Create children
		PathFinderNode children[4];
		int children_count = 0;
		PathFinder::expand(board, parent, children, children_count);

		// Find new on stack positions
		for (int i = 0; i < children_count; ++i) {

			// If new node, then add to open vector
			int open_index = PathFinder::list_index(children[i], open);
			int close_index = PathFinder::list_index(children[i], close);
			if (open_index == -1 && close_index == -1) {
				open.push_back(children[i]);
			}
			// Else, if node exist in both vectors
			if (open_index != -1 && close_index != -1) {
				PathFinder::calculate_input(children[i]);
				PathFinder::calculate_input(open[open_index]);
				PathFinder::calculate_input(close[close_index]);
				PathFinderNode min_o_c = open[open_index];
				if (open[open_index] > close[close_index]) {
					min_o_c = close[close_index];
				}
				if (!(children[i] > min_o_c)) {
					open[open_index] = children[i];
					if (children[i] < close[close_index])
						close[close_index] = children[i];
				}
				else
					continue;
			}
			// Else, if node exist in open vector
			if (open_index != -1 && close_index == -1) {
				PathFinder::calculate_input(children[i]);
				PathFinder::calculate_input(open[open_index]);
				if (!(children[i] > open[open_index]))
					open[open_index] = children[i];
				else
					continue;
			}
			// Else, if node exist in close vector
			if (close_index != -1 && open_index == -1) {
				PathFinder::calculate_input(children[i]);
				PathFinder::calculate_input(close[close_index]);
				if (!(children[i] > close[close_index])) {
					open.push_back(children[i]);
					close[close_index] = children[i];
				}
				else
					continue;
			}

			// Drop piece to ground -> updated
			PathFinderNode updated_child = children[i];
			updated_child.position.y -= board.get_drop_distance(updated_child.position);
			updated_child.path[updated_child.path_count] = MOVE_DOWN;
			++updated_child.path_count;

			// Check if updated child had been in result vec or not
			int uchild_index = PathFinder::list_index(updated_child, result);
			if (uchild_index == -1) {
				// If not then simply push back
				result.push_back(updated_child);
			}
			else {
				// Else, then check if the new node have smaller input count, push back
				PathFinder::calculate_input(updated_child);
				PathFinder::calculate_input(result[uchild_index]);
				if (updated_child < result[uchild_index]) result[uchild_index] = updated_child;
			}
		}

		// Push parent to close vector
		close.push_back(parent);
	}
}

int PathFinder::list_index(PathFinderNode& node, std::vector<PathFinderNode>& list)
{
	for (int i = 0; i < (int)list.size(); ++i) {
		if (node.position == list[i].position) return i;
	}
	return -1;
}
