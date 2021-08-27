#include "pathfinder.h"

/*
* Score should prioritize safest paths instead of shortest paths
*/
int a_star_node::calculate_score(piece_data& destination)
{
	int min_height = 100;
	for (int i = 0; i < 4; ++i) {
		min_height = std::min(min_height, destination.y + piece_def_lut[destination.type][destination.rotation][i][1]);
	}
	score = (piece.x - destination.x) * (piece.x - destination.x);
	score += (piece.y - destination.y) * (piece.y - destination.y) * ((drop_count > 0) * 2 - 1);
	if (drop_count == 0) score -= min_height * min_height;
	if (piece.rotation - destination.rotation == 3)
		score += 1;
	else
		score += (piece.rotation - destination.rotation) * (piece.rotation - destination.rotation);
	score += path_count * path_count;
	score += drop_count * drop_count;
	score += rotate_count * rotate_count;
	score += move_after_drop_count * move_after_drop_count;
	return score;
}

void pathfinder::search(bitboard& board, piece_data& destination, move result[32], int& result_count)
{
	// Sanity check
	result_count = 0;

	// Init open list
	std::vector<a_star_node> open_list;
	open_list.reserve(800);

	// Init closed list
	std::vector<a_star_node> close_list;
	close_list.reserve(800);

	// Put the initial node in the open list
	int y_init_pos = 19;
	if (board.is_occupied(4, y_init_pos)) { // If piece collides with the board at the starting positions, then return null
		++y_init_pos;
		if (board.is_occupied(4, y_init_pos)) return;
	}

	a_star_node init_node;
	init_node.piece = { 4, y_init_pos, destination.type, PIECE_UP, false };

	open_list.push_back(init_node);

	// Loop while open list is not empty
	while (!open_list.empty())
	{
		// Find the node with the least score in the open list, call it "parent"
		a_star_node parent = open_list[0];

		// Pop "parent" off the open list
		std::pop_heap(open_list.begin(), open_list.end());
		open_list.pop_back();

		// Generate children
		a_star_node children[5];
		for (int i = 0; i < 5; ++i) {
			children[i] = parent;
			bool move_success = false;

			// Move the child node
			switch (i)
			{
			case MOVE_RIGHT:
				move_success = board.piece_try_right(children[i].piece);
				break;
			case MOVE_LEFT:
				move_success = board.piece_try_left(children[i].piece);
				break;
			case MOVE_CW:
				if (destination.type == PIECE_O) break;
				move_success = board.piece_try_rotate(children[i].piece, true);
				++children[i].rotate_count;
				break;
			case MOVE_CCW:
				if (destination.type == PIECE_O) break;
				move_success = board.piece_try_rotate(children[i].piece, false);
				++children[i].rotate_count;
				break;
			case MOVE_DOWN:
				move_success = board.piece_try_down(children[i].piece);
				children[i].drop_count++;
				break;
			default:
				break;
			}

			// If moved succesfully
			if (move_success) {

				// Add move to children's path
				children[i].path[children[i].path_count] = (move)i;
				++children[i].path_count;

				// If have already soft dropped once, every moves after that should be checked if on floor and updated
				if (children[i].drop_count > 0) {
					++children[i].move_after_drop_count;
					bool update_success = board.piece_try_down(children[i].piece);

					// If piece can be dropped, update it
					if (update_success) {
						children[i].path[children[i].path_count] = MOVE_DOWN;
						++children[i].path_count;
					}
				}

				// Calculate node's score
				children[i].calculate_score(destination);

				// If successor is the goal, stop search
				if (children[i].piece == destination) {
					memcpy(result, children[i].path, children[i].path_count * sizeof(move));
					result_count = children[i].path_count;
					return;
				}

				// Check if node had already been found
				int in_open_list_index = pathfinder::is_in_list(open_list, children[i]);
				int in_closed_list_index = pathfinder::is_in_list(close_list, children[i]);

				// If node haven't been found, push to open list
				if (in_open_list_index == -1 && in_closed_list_index == -1) {
					open_list.push_back(children[i]);
					std::push_heap(open_list.begin(), open_list.end());
				}
				// Else
				else {

					// If a node with the same position as this child is in the OPEN list and has a lower score then skip this child
					if (in_open_list_index != -1) {
						if (open_list[in_open_list_index].score < children[i].score) {
							continue;
						}
					}

					// If a node with the same position as this child is in the CLOSED list and has a lower score then skip this child
					if (in_closed_list_index != -1) {
						if (close_list[in_closed_list_index].score < children[i].score) {
							continue;
						}
					}

					// Push child to open list
					// If node have already in the open list, replace it
					if (in_open_list_index != -1) {
						open_list[in_open_list_index] = children[i];
						std::make_heap(open_list.begin(), open_list.end());
					}
					// Else, push it to open list normally
					else {
						open_list.push_back(children[i]);
						std::push_heap(open_list.begin(), open_list.end());
					}
				}
			}
		}

		// Push parent node to the closed list
		close_list.push_back(parent);
	}
}

/*
* Check if a node is in a list
* Return the index if found, else return -1
*/
int pathfinder::is_in_list(std::vector<a_star_node>& open_list, a_star_node& node)
{
	for (size_t i = 0; i < open_list.size(); ++i) {
		if (open_list[i].piece == node.piece) {
			return (int)i;
		}
	}
	return -1;
}