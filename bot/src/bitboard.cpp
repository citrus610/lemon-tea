#include "bitboard.h"

void BitBoard::get_height(int height[10])
{
	for (int i = 0; i < 10; ++i) {
		height[i] = 64 - std::countl_zero(column[i]);
	}
}

int BitBoard::get_drop_distance(PieceData& piece)
{
	int result = 64;
	for (int i = 0; i < 4; ++i) {
		int cell_distance = piece.y + PIECE_LUT[piece.type][piece.rotation][i][1] - 64 +
			std::countl_zero(
				column[piece.x + PIECE_LUT[piece.type][piece.rotation][i][0]] &
				(((uint64_t)1 << (piece.y + PIECE_LUT[piece.type][piece.rotation][i][1])) - 1)
			);
		result = std::min(result, cell_distance);
	}
	return result;
}

uint64_t BitBoard::get_mask()
{
	uint64_t result = column[0];
	for (int i = 1; i < 10; ++i)
		result &= column[i];
	return result;
}

int BitBoard::clear_line()
{
	int result;

	uint64_t mask = get_mask();
	if (mask == 0b0) return 0;

	uint64_t mask_tzcnt = std::countr_zero(mask);
	mask = mask >> mask_tzcnt;

	for (int i = 0; i < 10; ++i) {
		uint64_t low_part = column[i] & (((uint64_t)0b1 << mask_tzcnt) - 1);
		uint64_t high_part = column[i] >> mask_tzcnt;

		switch (mask)
		{
		case 0b0001:
			high_part = high_part >> 1;
			result = 1;
			break;
		case 0b0011:
			high_part = high_part >> 2;
			result = 2;
			break;
		case 0b0111:
			high_part = high_part >> 3;
			result = 3;
			break;
		case 0b1111:
			high_part = high_part >> 4;
			result = 4;
			break;
		case 0b0101:
			high_part = ((high_part >> 1) & 0b1) | ((high_part >> 3) << 2);
			result = 2;
			break;
		case 0b1001:
			high_part = ((high_part >> 1) & 0b11) | ((high_part >> 4) << 2);
			result = 2;
			break;
		case 0b1011:
			high_part = ((high_part >> 2) & 0b1) | ((high_part >> 4) << 1);
			result = 3;
			break;
		case 0b1101:
			high_part = ((high_part >> 1) & 0b1) | ((high_part >> 4) << 1);
			result = 3;
			break;
		default:
			return -1;
			break;
		}

		column[i] = low_part | (high_part << mask_tzcnt);
	}

	return result;
}

bool BitBoard::is_occupied(const int& x, const int& y)
{
	if (x < 0 || x > 9 || y < 0 || y > 39) return true;
	return (column[x] & ((uint64_t)0b1 << y)) != 0;
}

bool BitBoard::is_colliding(const int& x, const int& y, const PieceType& type, const PieceRotation& rotation)
{
	for (int i = 0; i < 4; ++i) {
		if (is_occupied(PIECE_LUT[type][rotation][i][0] + x, PIECE_LUT[type][rotation][i][1] + y)) return true;
	}
	return false;
}

bool BitBoard::is_above_stack(PieceData& piece)
{
	for (int i = 0; i < 4; ++i) {
		if ((PIECE_LUT[piece.type][piece.rotation][i][1] + piece.y) // current cell y position
			< // smaller than
			(64 - std::countl_zero(column[PIECE_LUT[piece.type][piece.rotation][i][0] + piece.x])) // current column height
			) {
			return false;
		}
	}
	return true;
}

bool BitBoard::is_t_spin(PieceData& piece)
{
	if (piece.type == PIECE_T) {
		switch (piece.rotation)
		{
		case PIECE_UP:
			return
				is_occupied(piece.x + 1, piece.y + 1) &&
				is_occupied(piece.x - 1, piece.y + 1) &&
				(is_occupied(piece.x + 1, piece.y - 1) || is_occupied(piece.x - 1, piece.y - 1));
			break;
		case PIECE_RIGHT:
			return
				is_occupied(piece.x + 1, piece.y + 1) &&
				(is_occupied(piece.x + 1, piece.y - 1) + is_occupied(piece.x - 1, piece.y - 1) + is_occupied(piece.x - 1, piece.y + 1) >= 2);
			break;
		case PIECE_DOWN:
			return
				is_occupied(piece.x - 1, piece.y - 1) &&
				is_occupied(piece.x + 1, piece.y - 1) &&
				(is_occupied(piece.x - 1, piece.y + 1) || is_occupied(piece.x + 1, piece.y + 1));
			break;
		case PIECE_LEFT:
			return
				is_occupied(piece.x - 1, piece.y + 1) &&
				(is_occupied(piece.x - 1, piece.y - 1) + is_occupied(piece.x + 1, piece.y - 1) + is_occupied(piece.x + 1, piece.y + 1) >= 2);
			break;
		default:
			return false;
			break;
		}
	}
	return false;
}

void BitBoard::place_piece(PieceData& piece)
{
	for (int i = 0; i < 4; ++i) {
		column[PIECE_LUT[piece.type][piece.rotation][i][0] + piece.x] |= ((uint64_t)0b1 << (PIECE_LUT[piece.type][piece.rotation][i][1] + piece.y));
	}
}

bool BitBoard::piece_try_right(PieceData& piece)
{
	int pre_x = piece.x;
	piece.x += !is_colliding(piece.x + 1, piece.y, piece.type, piece.rotation);
	return piece.x != pre_x;
}

bool BitBoard::piece_try_left(PieceData& piece)
{
	int pre_x = piece.x;
	piece.x -= !is_colliding(piece.x - 1, piece.y, piece.type, piece.rotation);
	return piece.x != pre_x;
}

bool BitBoard::piece_try_rotate(PieceData& piece, bool is_cw)
{
	// O piece should not be here
	// assert(piece.type != PIECE_O);

	PieceRotation new_rotation = (PieceRotation)(((int)piece.rotation + (!is_cw * 2 + 1)) % 4);
	int srs_piece_index = std::min((int)piece.type, 1);
	for (int i = 0; i < 5; ++i) {
		int offset_x = SRS_LUT[srs_piece_index][piece.rotation][i][0] - SRS_LUT[srs_piece_index][new_rotation][i][0];
		int offset_y = SRS_LUT[srs_piece_index][piece.rotation][i][1] - SRS_LUT[srs_piece_index][new_rotation][i][1];
		if (!is_colliding(piece.x + offset_x, piece.y + offset_y, piece.type, new_rotation)) {
			piece.x += offset_x;
			piece.y += offset_y;
			piece.rotation = new_rotation;
			return true;
		}
	}
	return false;
}

bool BitBoard::piece_try_down(PieceData& piece)
{
	int pre_y = piece.y;
	while (!is_colliding(piece.x, piece.y - 1, piece.type, piece.rotation)) --piece.y;
	return piece.y != pre_y;
}

/*
* Convert the I, Z, S pieces to their default forms
* Useful in move generator
*/
void PieceData::normalize()
{
	switch (type)
	{
	case PIECE_I:
		switch (rotation)
		{
		case PIECE_UP:
			break;
		case PIECE_RIGHT:
			break;
		case PIECE_DOWN:
			rotation = PIECE_UP;
			--x;
			break;
		case PIECE_LEFT:
			rotation = PIECE_RIGHT;
			++y;
			break;
		default:
			break;
		}
		break;
	case PIECE_S:
		switch (rotation)
		{
		case PIECE_UP:
			break;
		case PIECE_RIGHT:
			break;
		case PIECE_DOWN:
			rotation = PIECE_UP;
			--y;
			break;
		case PIECE_LEFT:
			rotation = PIECE_RIGHT;
			--x;
			break;
		default:
			break;
		}
		break;
	case PIECE_Z:
		switch (rotation)
		{
		case PIECE_UP:
			break;
		case PIECE_RIGHT:
			break;
		case PIECE_DOWN:
			rotation = PIECE_UP;
			--y;
			break;
		case PIECE_LEFT:
			rotation = PIECE_RIGHT;
			--x;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

/*
* Convert the I, Z, S pieces to their alternative forms
* Useful in move generator
*/
void PieceData::mirror()
{
	switch (type)
	{
	case PIECE_I:
		switch (rotation)
		{
		case PIECE_UP:
			rotation = PIECE_DOWN;
			++x;
			break;
		case PIECE_RIGHT:
			rotation = PIECE_LEFT;
			--y;
			break;
		default:
			break;
		}
		break;
	case PIECE_S:
		switch (rotation)
		{
		case PIECE_UP:
			rotation = PIECE_DOWN;
			++y;
			break;
		case PIECE_RIGHT:
			rotation = PIECE_LEFT;
			++x;
			break;
		default:
			break;
		}
		break;
	case PIECE_Z:
		switch (rotation)
		{
		case PIECE_UP:
			rotation = PIECE_DOWN;
			++y;
			break;
		case PIECE_RIGHT:
			rotation = PIECE_LEFT;
			++x;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}
