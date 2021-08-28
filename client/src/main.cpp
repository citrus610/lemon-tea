#include <iostream>
#include "gamebot.h"

using namespace std;

void see_layer(beam& a, bitboard& b, int layer_index, int& node_count) {
	for (int i = 0; i < a.layer[layer_index].size; ++i) {
		draw_board(b);
		cout << endl;
		draw_board(a.layer[layer_index].pointer[i].board);
		cout << "LEVEL " << (layer_index + 1) << endl;
		cout << "node count: " << node_count << endl;
		cout << "org index: " << (int)a.layer[layer_index].pointer[i].org_index << endl;
		cout << "current: " << (int)a.layer[layer_index].pointer[i].current << endl;
		cout << "hold: " << (int)a.layer[layer_index].pointer[i].hold << endl;
		cout << "next: " << (int)a.layer[layer_index].pointer[i].next << endl;
		cout << "lock: " << (int)a.layer[layer_index].pointer[i].lock << endl;
		cout << "score: " << a.layer[layer_index].pointer[i].score << endl;
		cout << "tspin: " << (int)a.layer[layer_index].pointer[i].tspin[0] << " " << (int)a.layer[layer_index].pointer[i].tspin[1] << " " << (int)a.layer[layer_index].pointer[i].tspin[2] << endl;
		cout << "clear: " << (int)a.layer[layer_index].pointer[i].clear[0] << " " << (int)a.layer[layer_index].pointer[i].clear[1] << " " << (int)a.layer[layer_index].pointer[i].clear[2] << " " << (int)a.layer[layer_index].pointer[i].clear[3] << endl;
		cout << "struct: " << (int)a.layer[layer_index].pointer[i].structure[0] << " " << (int)a.layer[layer_index].pointer[i].structure[1] << endl;

		cin.get();
		system("CLS");
	}

};

void see_node_path(bitboard board, bot_solution solution) {

	piece_data piece;
	piece.x = 4;
	piece.y = 19;
	piece.rotation = PIECE_UP;
	piece.type = solution.placement.type;

	for (int i = 0; i < solution.move_list_count; ++i) {

		bitboard cur_board = board;

		switch (solution.move_list[i])
		{
		case MOVE_RIGHT:
			board.piece_try_right(piece);
			break;
		case MOVE_LEFT:
			board.piece_try_left(piece);
			break;
		case MOVE_CW:
			board.piece_try_rotate(piece, true);
			break;
		case MOVE_CCW:
			board.piece_try_rotate(piece, false);
			break;
		case MOVE_DOWN:
			board.piece_try_down(piece);
			break;
		default:
			break;
		}

		cur_board.place_piece(piece);
		draw_board(cur_board);

		//cin.get();
		system("CLS");
	}

	board.place_piece(piece);
	board.clear_line();
	draw_board(board);

	cout << endl;
	cout << "hold:  " << solution.is_hold << endl;
	cout << "node:  " << solution.node << endl;
	cout << "depth: " << solution.depth << endl;
	cout << "visit: " << solution.visit << endl;
	cout << "score: " << solution.score << endl;
	cout << "index: " << solution.index << endl;
}

int main()
{
	srand((unsigned int)time(NULL));

	/*
	const int beam_size = 10000;
	vec<node> pre_data;
	pre_data.init(beam_size);
	for (int i = 0; i < beam_size; ++i) {
		pre_data.add(node());
		pre_data.pointer[i].score = rand();
	}

	int vec_make_time = 0;
	int vec_push_time = 0;
	int std_make_time = 0;
	int std_push_time = 0;

	for (int iter = 0; iter < 10000; ++iter) {

		for (int i = 0; i < beam_size; ++i) {
			pre_data.pointer[i].score = rand();
		}

		// My vec make heap
		vec<node> vec_make;
		vec_make.init(beam_size);

		auto start_vec_make = chrono::steady_clock::now();
		for (int i = 0; i < beam_size; ++i) {
			vec_make.add(pre_data[i]);
		}
		std::make_heap(vec_make.pointer, vec_make.pointer + vec_make.size);
		for (int i = 0; i < 150; ++i) {
			std::pop_heap(vec_make.pointer, vec_make.pointer + vec_make.size);
			vec_make.pop();
		}
		auto end_vec_make = chrono::steady_clock::now();
		vec_make_time += chrono::duration_cast<chrono::microseconds>(end_vec_make - start_vec_make).count();



		// My vec push heap
		vec<node> vec_push;
		vec_push.init(beam_size);

		auto start_vec_push = chrono::steady_clock::now();
		for (int i = 0; i < beam_size; ++i) {
			vec_push.add(pre_data[i]);
			std::push_heap(vec_push.pointer, vec_push.pointer + vec_push.size);
		}
		for (int i = 0; i < 150; ++i) {
			std::pop_heap(vec_push.pointer, vec_push.pointer + vec_push.size);
			vec_push.pop();
		}
		auto end_vec_push = chrono::steady_clock::now();
		vec_push_time += chrono::duration_cast<chrono::microseconds>(end_vec_push - start_vec_push).count();



		// Std vec make heap
		vector<node> std_make;
		std_make.reserve(beam_size);

		auto start_std_make = chrono::steady_clock::now();
		for (int i = 0; i < beam_size; ++i) {
			std_make.push_back(pre_data[i]);
		}
		std::make_heap(std_make.begin(), std_make.end());
		for (int i = 0; i < 150; ++i) {
			std::pop_heap(std_make.begin(), std_make.end());
			std_make.pop_back();
		}
		auto end_std_make = chrono::steady_clock::now();
		std_make_time += chrono::duration_cast<chrono::microseconds>(end_std_make - start_std_make).count();



		// Std vec push heap
		vector<node> std_push;
		std_push.reserve(beam_size);

		auto start_std_push = chrono::steady_clock::now();
		for (int i = 0; i < beam_size; ++i) {
			std_push.push_back(pre_data[i]);
			std::push_heap(std_push.begin(), std_push.end());
		}
		for (int i = 0; i < 150; ++i) {
			std::pop_heap(std_push.begin(), std_push.end());
			std_push.pop_back();
		}
		auto end_std_push = chrono::steady_clock::now();
		std_push_time += chrono::duration_cast<chrono::microseconds>(end_std_push - start_std_push).count();

	}

	cout << "my vec make heap:  " << vec_make_time << " microsecond" << endl;
	cout << "my vec push heap:  " << vec_push_time << " microsecond" << endl;
	cout << "std vec make heap: " << std_make_time << " microsecond" << endl;
	cout << "std vec push heap: " << std_push_time << " microsecond" << endl;
	//*/

	gamebot new_game;
	new_game.start();

	//cin.get();
	return 0;
}