#ifndef SFML_TYPE_H
#define SFML_TYPE_H

#include <fstream>
#include <string>
#include <iomanip>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using order_json = nlohmann::ordered_json;

#include "bot.h"

static piece_type char_to_piece(char type) {
	switch (type)
	{
	case 'I':
		return PIECE_I;
		break;
	case 'J':
		return PIECE_J;
		break;
	case 'L':
		return PIECE_L;
		break;
	case 'T':
		return PIECE_T;
		break;
	case 'Z':
		return PIECE_Z;
		break;
	case 'S':
		return PIECE_S;
		break;
	case 'O':
		return PIECE_O;
		break;
	default:
		return PIECE_NONE;
		break;
	}
};

static char piece_to_char(piece_type type) {
	switch (type)
	{
	case PIECE_I:
		return 'I';
		break;
	case PIECE_J:
		return 'J';
		break;
	case PIECE_L:
		return 'L';
		break;
	case PIECE_T:
		return 'T';
		break;
	case PIECE_Z:
		return 'Z';
		break;
	case PIECE_S:
		return 'S';
		break;
	case PIECE_O:
		return 'O';
		break;
	default:
		return ' ';
		break;
	}
};

static bot_new_state board_to_bot_data(tetris_board& _board) {
	bot_new_state result;

	result.current = char_to_piece(_board.current_piece);
	result.hold = char_to_piece(_board.hold_piece);
	result.b2b = _board.b2b;
	result.ren = _board.ren;
	for (int i = 0; i < std::min((int)_board.next_piece.size(), 16); ++i) {
		result.next[i] = (char_to_piece(_board.next_piece[i]));
	}
	for (int i = 0; i < 10; ++i) {
		uint64_t a_column = 0b0;
		for (int k = 0; k < 40; ++k) {
			if (_board.data[k][i] > 0) {
				a_column = a_column | ((uint64_t)0b1 << (39 - k));
			}
		}
		result.board.column[i] = a_column;
	}

	return result;
};

static node board_to_node(tetris_board& _board) {
	node result;

	result.current = char_to_piece(_board.current_piece);
	result.hold = char_to_piece(_board.hold_piece);
	result.b2b = _board.b2b;
	result.ren = _board.ren;
	result.next = 0;
	for (int i = 0; i < 10; ++i) {
		uint64_t a_column = 0b0;
		for (int k = 0; k < 40; ++k) {
			if (_board.data[k][i] > 0) {
				a_column = a_column | ((uint64_t)0b1 << (39 - k));
			}
		}
		result.board.column[i] = a_column;
	}

	return result;
};

static void draw_board(bitboard& board) {
	for (int8_t _y = 21; _y >= 0; --_y) {
		for (int8_t _x = 0; _x < 10; ++_x) {
			if (board.is_occupied(_x, _y)) {
				std::cout << "#";
			}
			else {
				std::cout << ".";
			}
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
};

static void create_json() {

	// Check if existed?
	std::ifstream f("init.json");
	if (f.good()) {
		return;
	};
	f.close();

	// Setting data
	order_json js;

	int preview = 5;
	int speed = 100;
	bool forecast = true;
	weight heuristic;
	heuristic.standard();

	for (int i = 1; i < 3; ++i) {

		std::string player_id = std::string("player ") + std::to_string(i);

		js[player_id]["preview"] = 5;
		js[player_id]["speed"] = 100;
		js[player_id]["forecast"] = true;

		js[player_id]["heuristic"]["max_height"] = heuristic.max_height;
		js[player_id]["heuristic"]["max_height_top_half"] = heuristic.max_height_top_half;
		js[player_id]["heuristic"]["max_height_top_quarter"] = heuristic.max_height_top_quarter;
		js[player_id]["heuristic"]["bumpiness"] = heuristic.bumpiness;
		js[player_id]["heuristic"]["bumpiness_s"] = heuristic.bumpiness_s;
		js[player_id]["heuristic"]["bumpiness_t"] = heuristic.bumpiness_t;
		js[player_id]["heuristic"]["hole"] = heuristic.hole;
		js[player_id]["heuristic"]["hole_s"] = heuristic.hole_s;
		js[player_id]["heuristic"]["blocked_cell"] = heuristic.blocked_cell;
		js[player_id]["heuristic"]["blocked_cell_s"] = heuristic.blocked_cell_s;
		js[player_id]["heuristic"]["block_above_hole"] = heuristic.block_above_hole;
		js[player_id]["heuristic"]["block_above_hole_s"] = heuristic.block_above_hole_s;
		js[player_id]["heuristic"]["well"] = heuristic.well;
		js[player_id]["heuristic"]["well_index"] =
		{
			heuristic.well_index[0],
			heuristic.well_index[1],
			heuristic.well_index[2],
			heuristic.well_index[3]
		};
		js[player_id]["heuristic"]["structure"] = 
		{
			heuristic.structure[0],
			heuristic.structure[1]
		};
		js[player_id]["heuristic"]["waste_structure"] =
		{
			heuristic.waste_structure[0],
			heuristic.waste_structure[1]
		};
		js[player_id]["heuristic"]["b2b_chain"] = heuristic.b2b_chain;
		js[player_id]["heuristic"]["b2b_max_chain"] = heuristic.b2b_max_chain;
		js[player_id]["heuristic"]["ren_chain"] = heuristic.ren_chain;
		js[player_id]["heuristic"]["ren_max_chain"] = heuristic.ren_max_chain;
		js[player_id]["heuristic"]["clear"] =
		{
			heuristic.clear[0],
			heuristic.clear[1],
			heuristic.clear[2],
			heuristic.clear[3],
		};
		js[player_id]["heuristic"]["t_spin"] =
		{
			heuristic.t_spin[0],
			heuristic.t_spin[1],
			heuristic.t_spin[2],
		};
		js[player_id]["heuristic"]["perfect_clear"] = heuristic.perfect_clear;
		js[player_id]["heuristic"]["waste_time"] = heuristic.waste_time;
		js[player_id]["heuristic"]["waste_T"] = heuristic.waste_T;
		js[player_id]["heuristic"]["waste_I"] = heuristic.waste_I;
	}

	// Create file
	std::ofstream o("init.json");
	o << std::setw(4) << js << std::endl;
	o.close();
};

static void set_from_json(json& js, int player_id, weight& heuristic, int& speed, int& preview, bool& forecast) {

	std::string id = std::string("player ") + std::to_string(player_id);

	// Preview
	preview = js[id]["preview"];
	preview = std::max(preview, 1);
	preview = std::min(preview, 16);

	// Speed
	speed = js[id]["speed"];
	speed = std::max(speed, 10);
	speed = std::min(speed, 100);

	// Forecast mode
	forecast = js[id]["forecast"];

	// Heuristic
	heuristic.max_height = js[id]["heuristic"]["max_height"];
	heuristic.max_height_top_half = js[id]["heuristic"]["max_height_top_half"];
	heuristic.max_height_top_quarter = js[id]["heuristic"]["max_height_top_quarter"];
	heuristic.bumpiness = js[id]["heuristic"]["bumpiness"];
	heuristic.bumpiness_s = js[id]["heuristic"]["bumpiness_s"];
	heuristic.bumpiness_t = js[id]["heuristic"]["bumpiness_t"];
	heuristic.hole = js[id]["heuristic"]["hole"];
	heuristic.hole_s = js[id]["heuristic"]["hole_s"];
	heuristic.blocked_cell = js[id]["heuristic"]["blocked_cell"];
	heuristic.blocked_cell_s = js[id]["heuristic"]["blocked_cell_s"];
	heuristic.block_above_hole = js[id]["heuristic"]["block_above_hole"];
	heuristic.block_above_hole_s = js[id]["heuristic"]["block_above_hole_s"];
	heuristic.well = js[id]["heuristic"]["well"];
	for (int i = 0; i < 4; ++i) {
		heuristic.well_index[i] = js[id]["heuristic"]["well_index"][i];
	}
	for (int i = 0; i < 2; ++i) {
		heuristic.structure[i] = js[id]["heuristic"]["structure"][i];
	}
	for (int i = 0; i < 2; ++i) {
		heuristic.waste_structure[i] = js[id]["heuristic"]["waste_structure"][i];
	}
	heuristic.b2b_chain = js[id]["heuristic"]["b2b_chain"];
	heuristic.b2b_max_chain = js[id]["heuristic"]["b2b_max_chain"];
	heuristic.ren_chain = js[id]["heuristic"]["ren_chain"];
	heuristic.ren_max_chain = js[id]["heuristic"]["ren_max_chain"];
	for (int i = 0; i < 4; ++i) {
		heuristic.clear[i] = js[id]["heuristic"]["clear"][i];
	}
	for (int i = 0; i < 3; ++i) {
		heuristic.t_spin[i] = js[id]["heuristic"]["t_spin"][i];
	}
	heuristic.perfect_clear = js[id]["heuristic"]["perfect_clear"];
	heuristic.waste_time = js[id]["heuristic"]["waste_time"];
	heuristic.waste_T = js[id]["heuristic"]["waste_T"];
	heuristic.waste_I = js[id]["heuristic"]["waste_I"];
};

#endif