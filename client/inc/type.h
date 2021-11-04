#ifndef SFML_TYPE_H
#define SFML_TYPE_H

#include <fstream>
#include <string>
#include <iomanip>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using order_json = nlohmann::ordered_json;

#include "bot.h"

static PieceType char_to_piece(char type) {
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

static char piece_to_char(PieceType type) {
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

static BotState board_to_bot_data(tetris_board& _board) {
	BotState result;

	result.current = char_to_piece(_board.current_piece);
	result.hold = char_to_piece(_board.hold_piece);
	result.b2b = _board.b2b;
	result.ren = _board.ren;
	for (int i = 0; i < std::min((int)_board.next_piece.size(), MAX_TREE_QUEUE); ++i) {
		result.next[i] = (char_to_piece(_board.next_piece[i]));
	}
	result.next_count = std::min((int)_board.next_piece.size(), MAX_TREE_QUEUE);
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

static Node board_to_node(tetris_board& _board) {
	Node result;

	result.state.current = char_to_piece(_board.current_piece);
	result.state.hold = char_to_piece(_board.hold_piece);
	result.state.b2b = _board.b2b;
	result.state.ren = _board.ren;
	result.state.next = 0;
	for (int i = 0; i < 10; ++i) {
		uint64_t a_column = 0b0;
		for (int k = 0; k < 40; ++k) {
			if (_board.data[k][i] > 0) {
				a_column = a_column | ((uint64_t)0b1 << (39 - k));
			}
		}
		result.state.board.column[i] = a_column;
	}

	return result;
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
	Weight heuristic;
	heuristic.standard();

	for (int i = 1; i < 3; ++i) {

		std::string player_id = std::string("player ") + std::to_string(i);

		js[player_id]["enable"] = true;
		js[player_id]["preview"] = 5;
		js[player_id]["speed"] = 100;
		js[player_id]["forecast"] = true;

		js[player_id]["heuristic"]["defence"]["max_height"] = heuristic.defence.max_height;
		js[player_id]["heuristic"]["defence"]["max_height_top_half"] = heuristic.defence.max_height_top_half;
		js[player_id]["heuristic"]["defence"]["max_height_top_quarter"] = heuristic.defence.max_height_top_quarter;
		js[player_id]["heuristic"]["defence"]["bumpiness"] = heuristic.defence.bumpiness;
		js[player_id]["heuristic"]["defence"]["bumpiness_s"] = heuristic.defence.bumpiness_s;
		js[player_id]["heuristic"]["defence"]["bumpiness_t"] = heuristic.defence.bumpiness_t;
		js[player_id]["heuristic"]["defence"]["hole"] = heuristic.defence.hole;
		js[player_id]["heuristic"]["defence"]["blocked"] = heuristic.defence.blocked;
		js[player_id]["heuristic"]["defence"]["well"] = heuristic.defence.well;
		js[player_id]["heuristic"]["defence"]["well_position"] = {
			heuristic.defence.well_position[0],
			heuristic.defence.well_position[1],
			heuristic.defence.well_position[2],
			heuristic.defence.well_position[3],
			heuristic.defence.well_position[4],
			heuristic.defence.well_position[5],
			heuristic.defence.well_position[6],
			heuristic.defence.well_position[7],
			heuristic.defence.well_position[8],
			heuristic.defence.well_position[9]
		};
		js[player_id]["heuristic"]["defence"]["structure"] = {
			heuristic.defence.structure[0],
			heuristic.defence.structure[1]
		};
		js[player_id]["heuristic"]["defence"]["b2b"] = heuristic.defence.b2b;
		js[player_id]["heuristic"]["defence"]["ren"] = heuristic.defence.ren;

		js[player_id]["heuristic"]["attack"]["clear"] = {
			heuristic.attack.clear[0],
			heuristic.attack.clear[1],
			heuristic.attack.clear[2],
			heuristic.attack.clear[3]
		};
		js[player_id]["heuristic"]["attack"]["tspin"] = {
			heuristic.attack.tspin[0],
			heuristic.attack.tspin[1],
			heuristic.attack.tspin[2]
		};
		js[player_id]["heuristic"]["attack"]["perfect_clear"] = heuristic.attack.perfect_clear;
		js[player_id]["heuristic"]["attack"]["ren"] = heuristic.attack.ren;
		js[player_id]["heuristic"]["attack"]["waste_time"] = heuristic.attack.waste_time;
		js[player_id]["heuristic"]["attack"]["waste_T"] = heuristic.attack.waste_T;
		js[player_id]["heuristic"]["attack"]["waste_I"] = heuristic.attack.waste_I;
	}

	// Create file
	std::ofstream o("init.json");
	o << std::setw(4) << js << std::endl;
	o.close();
};

static void set_from_json(json& js, int player_id, bool& enable_bot, Weight& heuristic, int& speed, int& preview, bool& forecast) {

	std::string id = std::string("player ") + std::to_string(player_id);

	// Enable bot
	enable_bot = js[id]["enable"];

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
	heuristic.defence.max_height = js[id]["heuristic"]["defence"]["max_height"];
	heuristic.defence.max_height_top_half = js[id]["heuristic"]["defence"]["max_height_top_half"];
	heuristic.defence.max_height_top_quarter = js[id]["heuristic"]["defence"]["max_height_top_quarter"];
	heuristic.defence.bumpiness = js[id]["heuristic"]["defence"]["bumpiness"];
	heuristic.defence.bumpiness_s = js[id]["heuristic"]["defence"]["bumpiness_s"];
	heuristic.defence.bumpiness_t = js[id]["heuristic"]["defence"]["bumpiness_t"];
	heuristic.defence.hole = js[id]["heuristic"]["defence"]["hole"];
	heuristic.defence.blocked = js[id]["heuristic"]["defence"]["blocked"];
	heuristic.defence.well = js[id]["heuristic"]["defence"]["well"];
	for (int i = 0; i < 10; ++i) {
		heuristic.defence.well_position[i] = js[id]["heuristic"]["defence"]["well_position"][i];
	}
	for (int i = 0; i < 2; ++i) {
		heuristic.defence.structure[i] = js[id]["heuristic"]["defence"]["structure"][i];
	}
	heuristic.defence.b2b = js[id]["heuristic"]["defence"]["b2b"];
	heuristic.defence.ren = js[id]["heuristic"]["defence"]["ren"];

	for (int i = 0; i < 4; ++i) {
		heuristic.attack.clear[i] = js[id]["heuristic"]["attack"]["clear"][i];
	}
	for (int i = 0; i < 3; ++i) {
		heuristic.attack.tspin[i] = js[id]["heuristic"]["attack"]["tspin"][i];
	}
	heuristic.attack.perfect_clear = js[id]["heuristic"]["attack"]["perfect_clear"];
	heuristic.attack.ren = js[id]["heuristic"]["attack"]["ren"];
	heuristic.attack.waste_time = js[id]["heuristic"]["attack"]["waste_time"];
	heuristic.attack.waste_T = js[id]["heuristic"]["attack"]["waste_T"];
	heuristic.attack.waste_I = js[id]["heuristic"]["attack"]["waste_I"];
};

#endif