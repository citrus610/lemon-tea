#ifndef SFML_TYPE_H
#define SFML_TYPE_H

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

#endif