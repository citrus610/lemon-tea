#pragma once

#include "genmove.h"
#include "node.h"
#include "pathfinder.h"

static char convert_piece_to_char(PieceType piece) {
	switch (piece)
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
	case PIECE_S:
		return 'S';
		break;
	case PIECE_Z:
		return 'Z';
		break;
	case PIECE_T:
		return 'T';
		break;
	case PIECE_O:
		return 'O';
		break;
	default:
		return 'X';
		break;
	}
}

static char convert_rotation_to_char(PieceRotation rotation) {
	switch (rotation)
	{
	case PIECE_UP:
		return 'U';
		break;
	case PIECE_DOWN:
		return 'D';
		break;
	case PIECE_RIGHT:
		return 'R';
		break;
	case PIECE_LEFT:
		return 'L';
		break;
	default:
		return 'X';
		break;
	}
}

static std::string convert_lock_to_string(LockType lock) {
	switch (lock)
	{
	case LOCK_CLEAR_1:
		return std::string("LOCK CLEAR 1");
		break;
	case LOCK_CLEAR_2:
		return std::string("LOCK CLEAR 2");
		break;
	case LOCK_CLEAR_3:
		return std::string("LOCK CLEAR 3");
		break;
	case LOCK_CLEAR_4:
		return std::string("LOCK CLEAR 4");
		break;
	case LOCK_TSPIN_1:
		return std::string("LOCK TSPIN 1");
		break;
	case LOCK_TSPIN_2:
		return std::string("LOCK TSPIN 2");
		break;
	case LOCK_TSPIN_3:
		return std::string("LOCK TSPIN 3");
		break;
	case LOCK_PC:
		return std::string("LOCK PERFECT CLEAR");
		break;
	default:
		return std::string("LOCK NONE");
		break;
	}
}

static std::string convert_move_to_string(MoveType move) {
	switch (move)
	{
	case MOVE_CCW:
		return std::string("CCW");
		break;
	case MOVE_CW:
		return std::string("CW");
		break;
	case MOVE_RIGHT:
		return std::string("RIGHT");
		break;
	case MOVE_LEFT:
		return std::string("LEFT");
		break;
	case MOVE_DOWN:
		return std::string("DROP");
		break;
	default:
		return std::string("MOVE NONE");
		break;
	}
}

static void draw_board(BitBoard& board) {
	for (int8_t _y = 21; _y >= 0; --_y) {
		std::cout << "     ";
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
};

/*
static void draw_state(GameState& state) {
	using namespace std;

	cout << " - Board: " << endl;
	draw_board(state.board);
	cout << " - Current:    " << convert_piece_to_char(state.current) << endl;
	cout << " - Hold:       " << convert_piece_to_char(state.hold) << endl;
	cout << " - Next index: " << state.next << endl;
	cout << " - B2b:        " << state.b2b << endl;
	cout << " - Ren:        " << state.ren << endl;
}
*/

static void draw_piece_data(PieceData& piece_data) {
	using namespace std;

	cout << "    + X:        " << piece_data.x << endl;
	cout << "    + Y:        " << piece_data.y << endl;
	cout << "    + Type:     " << convert_piece_to_char(piece_data.type) << endl;
	cout << "    + Rotation: " << convert_rotation_to_char(piece_data.rotation) << endl;
}

/*
static void draw_action(Action& action) {
	using namespace std;

	cout << " - Placement:  " << endl;
	draw_piece_data(action.placement);
	if (action.hold)
		cout << " - Hold:       " << "TRUE" << endl;
	else
		cout << " - Hold:       " << "FALSE" << endl;
	if (action.soft_drop)
		cout << " - Soft drop:  " << "TRUE" << endl;
	else
		cout << " - Soft drop:  " << "FALSE" << endl;
	cout << " - Lock:       " << convert_lock_to_string(action.lock) << endl;
}
*/

/*
static void draw_node(Node& node) {
	using namespace std;

	cout << "====== STATE ======" << endl;
	draw_state(node.state);
	cout << endl;

	cout << "====== ORIGIN =====" << endl;
	draw_action(node.origin);
	cout << endl;

	cout << "====== ACTION =====" << endl;
	draw_action(node.action);
	cout << endl;

	cout << "====== SCORE ======" << endl;
	cout << " - Attack:     " << node.score.attack << endl;
	cout << " - Defence:    " << node.score.defence << endl;
}
*/

static void draw_move_gen(BitBoard& board, PieceType piece) {
	PieceData list[MAX_MOVE_GENERATOR];
	int list_cnt = 0;

	MoveGenerator::generate(board, piece, list, list_cnt);

	for (int i = 0; i < list_cnt; ++i) {
		BitBoard copy = board;
		copy.place_piece(list[i]);
		draw_board(copy);
		draw_piece_data(list[i]);
		std::cin.get();
	}
};