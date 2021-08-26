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

	/*
	bitboard b;
	b.column[9] = 0b00111111;
	b.column[8] = 0b01111111;
	b.column[7] = 0b01111111;
	b.column[6] = 0b00111111;
	b.column[5] = 0b00011111;
	b.column[4] = 0b00000111;
	b.column[3] = 0b01011111;
	b.column[2] = 0b01111111;
	b.column[1] = 0b00111111;
	b.column[0] = 0b00110000;
	draw_board(b);
	cout << sizeof(piece_data) << endl;
	cin.get();
	system("CLS");
	piece_type queue[5] = { PIECE_I, PIECE_Z, PIECE_S, PIECE_O, PIECE_L };

	beam a;
	a.init(5, true);
	a.set_root(b, PIECE_T, PIECE_NONE, queue, 0, 0);
	int node_count = 0;

	auto start = chrono::steady_clock::now();
	node_count = a.search(500);
	auto end = chrono::steady_clock::now();
	for (int i = 0; i < a.reward_count; ++i) {
		cout << a.reward[i].visit << " ";
	}cout << endl;

	int b_i = a.get_best_index();
	cout << "reward count: " << a.reward_count << endl;
	cout << "best org index: " << b_i << "    visit value: " << a.reward[b_i].visit << endl;
	cout << "node count: " << node_count << endl;
	cout << "time: " << (chrono::duration_cast<chrono::milliseconds>(end - start).count()) << " ms" << endl;
	cout << "nps: " << (double)((double)node_count / (double)(chrono::duration_cast<chrono::milliseconds>(end - start).count())) << " knode/s" << endl; //*/

	/*
	bitboard b;
	b.column[9] = 0b000000011;
	b.column[8] = 0b000000011;
	b.column[7] = 0b000000000;
	b.column[6] = 0b000000001;
	b.column[5] = 0b000000111;
	b.column[4] = 0b000001101;
	b.column[3] = 0b000000001;
	b.column[2] = 0b000000000;
	b.column[1] = 0b000000000;
	b.column[0] = 0b000000000;
	draw_board(b);
	piece_type queue[16] = { PIECE_T, PIECE_Z, PIECE_S, PIECE_O, PIECE_L, PIECE_I };

	weight w;
	w.standard();

	bot_new_state state;
	state.board = b;
	state.current = PIECE_L;
	state.hold = PIECE_NONE;
	memcpy(state.next, queue, 5 * sizeof(piece_type));

	this_thread::sleep_for(chrono::milliseconds(2000));

	bot a_bot;
	a_bot.start(5, w, true);
	a_bot.set_state(state);
	this_thread::sleep_for(chrono::milliseconds(500));
	bot_solution solution1 = a_bot.request_solution();

	bitboard board = b;
	board.place_piece(solution1.placement);
	board.clear_line();
	state.board = board;
	state.current = PIECE_L;
	state.hold = PIECE_T;
	for (int i = 0; i < 4; ++i) {
		queue[i] = queue[i + 1];
	}
	memcpy(state.next, queue, 5 * sizeof(piece_type));

	a_bot.set_state(state);
	this_thread::sleep_for(chrono::milliseconds(500));
	bot_solution solution2 = a_bot.request_solution();

	a_bot.destroy();

	cin.get();
	system("CLS");
	see_node_path(b, solution1);
	see_node_path(board, solution2);
	//*/

	/*
	bitboard board;
	board.column[9] = 0b000011111;
	board.column[8] = 0b000011111;
	board.column[7] = 0b000011111;
	board.column[6] = 0b000011111;
	board.column[5] = 0b000011111;
	board.column[4] = 0b000011111;
	board.column[3] = 0b000011111;
	board.column[2] = 0b000011111;
	board.column[1] = 0b000011111;
	board.column[0] = 0b000000000;

	piece_type queue[16] = { PIECE_L, PIECE_Z, PIECE_S, PIECE_O, PIECE_L, PIECE_I, PIECE_L, PIECE_Z, PIECE_S, PIECE_O, PIECE_L, PIECE_I };
	int queue_cnt = 12;

	node a;
	node b;
	node c;

	a.board = board;
	a.current = PIECE_I;
	a.hold = PIECE_NONE;

	piece_data piece;
	piece.x = 4;
	piece.y = 5;
	piece.type = PIECE_L;
	piece.rotation = PIECE_UP;

	b.attemp(a, piece, true, queue, queue_cnt);

	draw_board(b.board);
	cout << "cur: " << b.current << endl;
	cout << "hol: " << b.hold << endl;
	cout << "next: " << b.next << endl;

	piece.x = 0;
	piece.y = 3;
	piece.type = PIECE_I;
	piece.rotation = PIECE_RIGHT;

	c.attemp(b, piece, true, queue, queue_cnt);

	draw_board(c.board);
	cout << "cur: " << c.current << endl;
	cout << "hol: " << c.hold << endl;
	cout << "next: " << c.next << endl;
	//*/

	/*
	bitboard b;
	b.column[9] = 0b000000011;
	b.column[8] = 0b000000011;
	b.column[7] = 0b000000000;
	b.column[6] = 0b000000001;
	b.column[5] = 0b000000111;
	b.column[4] = 0b001001111;
	b.column[3] = 0b001111111;
	b.column[2] = 0b000000000;
	b.column[1] = 0b000000000;
	b.column[0] = 0b000000000;

	piece_type queue[35] = { PIECE_S, PIECE_O, PIECE_J, PIECE_L, PIECE_I, PIECE_T, PIECE_Z, PIECE_S, PIECE_O, PIECE_J, PIECE_L, PIECE_I, PIECE_T, PIECE_Z, PIECE_S, PIECE_O, PIECE_J, PIECE_L, PIECE_I, PIECE_T, PIECE_Z, PIECE_S, PIECE_O, PIECE_J, PIECE_L, PIECE_I };
	int q_index = 0;

	bot_new_state state;
	state.board = b;
	state.current = PIECE_Z;
	state.hold = PIECE_T;
	memcpy(state.next, queue, 5 * sizeof(piece_type));

	weight w;
	w.standard();

	this_thread::sleep_for(chrono::milliseconds(2000));
	bot a_bot;
	a_bot.start(5, w, false);

	const int max_iter = 20;
	vector<bot_solution> p_m;

	for (int i = 0; i < max_iter; ++i) {

		a_bot.set_state(state);

		this_thread::sleep_for(chrono::milliseconds(150));

		bot_solution solution = a_bot.request_solution();

		if (solution.is_hold) {
			if (state.hold == PIECE_NONE) {
				++q_index;
			}
			state.hold = state.current;
		}

		if (q_index < max_iter) {
			state.current = queue[q_index];
			++q_index;
		}
		else {
			state.current = PIECE_NONE;
		}

		state.board.place_piece(solution.placement);
		state.board.clear_line();

		memcpy(state.next, queue + q_index, 5 * sizeof(piece_type));

		p_m.push_back(solution);
	}

	a_bot.destroy();

	cin.get();
	for (int i = 0; i < max_iter; ++i) {
		see_node_path(b, p_m[i]);
		b.place_piece(p_m[i].placement);
		b.clear_line();
		cin.get();
		system("CLS");
	}*/
	
	gamebot new_game;
	new_game.enable_bot_2 = true;
	new_game.start();

	//cin.get();
	return 0;
}