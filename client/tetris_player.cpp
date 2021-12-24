#include "tetris_player.h"

namespace TetrisGame
{

void TetrisPlayer::init(std::vector<PieceType>& init_bag)
{
	this->board = BitBoard();
	this->current = PIECE_NONE;
	this->hold = PIECE_NONE;
	this->next.clear();
	this->b2b = 0;
	this->ren = 0;

	this->piece = PieceData();
	this->piece_locked = false;

	this->bag = init_bag;
	// this->bag_index = 0;
	this->bag_index = (rand() % 1000) * 7;

	this->incomming_garbage = 0;
	this->line_sent = 0;

	this->clearline_cnter = PLAYER_DELAY_LINE_CLEAR + 2;
	this->non_clearline_cnter = 2;
	this->gameover = false;

	this->fill_queue();
	this->update_current();
}

void TetrisPlayer::update()
{
	if (this->gameover) return;
	this->update_clearline();
	if (this->just_clearline()) {
		int attack = this->count_garbage();
		this->board.place_piece(this->piece);
		int line_clear = this->board.clear_line();
		if (line_clear < 1) {
			this->place_garbage();
		}
		else {
			if (this->incomming_garbage > attack) {
				this->incomming_garbage -= attack;
				this->place_garbage();
			}
			else {
				this->send_garbage(attack - this->incomming_garbage);
				this->incomming_garbage = 0;
			}
		}
		this->update_current();
	}
	if (!this->is_clearline()) {
		if (this->piece_locked) {
			if (this->piece.y >= 20) this->gameover = true;
			BitBoard copy = this->board;
			copy.place_piece(this->piece);
			int fullrow_cnt = copy.clear_line();
			if (fullrow_cnt < 1) {
				this->board.place_piece(this->piece);
				this->update_current();
				this->place_garbage();
				this->ren = 0;
			}
			else {
				this->start_clearline();
			}
		}
	}
}

void TetrisPlayer::set_enemy(TetrisPlayer* other)
{
	this->enemy = other;
}

void TetrisPlayer::fill_queue()
{
	int current_queue_count = (int)this->next.size();
	for (int i = 0; i < PLAYER_NEXT_COUNT - current_queue_count; ++i) {
		this->next.push_back(this->bag[this->bag_index]);
		++this->bag_index;
		this->bag_index = this->bag_index % PLAYER_PRE_GENERATE_BAG_SIZE;
	}
}

void TetrisPlayer::update_current()
{
	this->current = this->next[0];
	this->next.erase(this->next.begin() + 0);
	this->fill_queue();

	this->piece = {
		4,
		19,
		this->current,
		PIECE_UP
	};
	if (this->board.is_colliding(4, 19, this->current, PIECE_UP)) this->piece.y = 20;
	this->piece_locked = false;
}

void TetrisPlayer::do_hold()
{
	if (this->hold == PIECE_NONE) {
		this->hold = this->current;
		this->update_current();
	}
	else {
		PieceType place_holder = this->hold;
		this->hold = this->current;
		this->current = place_holder;

		this->piece = {
			4,
			19,
			this->current,
			PIECE_UP
		};
		if (this->board.is_colliding(4, 19, this->current, PIECE_UP)) this->piece.y = 20;
		this->piece_locked = false;
	}
}

void TetrisPlayer::do_right()
{
	this->board.piece_try_right(this->piece);
}

void TetrisPlayer::do_left()
{
	this->board.piece_try_left(this->piece);
}

void TetrisPlayer::do_rotate(bool cw)
{
	this->board.piece_try_rotate(this->piece, cw);
}

void TetrisPlayer::do_down()
{
	this->piece.y -= !this->board.is_colliding(this->piece.x, this->piece.y - 1, this->piece.type, this->piece.rotation);
}

void TetrisPlayer::do_drop()
{
	this->piece.y -= this->board.get_drop_distance(this->piece);
	this->piece_locked = true;
}

int TetrisPlayer::count_garbage()
{
	int result = 0;

	// Check t spin, place piece and clear line
	BitBoard copy = this->board;
	bool is_t_spin = copy.is_t_spin(this->piece);
	copy.place_piece(this->piece);
	int line_cleared = copy.clear_line();

	// Update ren, b2b and lock data
	if (line_cleared > 0) {
		// T spin
		if (is_t_spin) {
			result += line_cleared * 2 + (this->b2b > 0);
			++this->b2b;
		}
		else {
			// Perfect clear
			if (copy.column[0] == (uint64_t)0 &&
				copy.column[1] == (uint64_t)0 &&
				copy.column[2] == (uint64_t)0 &&
				copy.column[3] == (uint64_t)0 &&
				copy.column[4] == (uint64_t)0 &&
				copy.column[5] == (uint64_t)0 &&
				copy.column[6] == (uint64_t)0 &&
				copy.column[7] == (uint64_t)0 &&
				copy.column[8] == (uint64_t)0 &&
				copy.column[9] == (uint64_t)0) {
				result += 10;
				this->b2b = 0;
			}
			else {
				// Normal clear line
				if (line_cleared == 4) {
					result += 4 + (this->b2b > 0);
					++this->b2b;
				}
				else {
					result += line_cleared - 1;
					this->b2b = 0;
				}
			}
		}

		// Add & update combo to line clear
		result += std::min(this->ren / 2, 5);
		++this->ren;
	}
	else {
		this->ren = 0;
	}

	return result;
}

void TetrisPlayer::place_garbage()
{
	this->incomming_garbage = std::min(this->incomming_garbage, 22);

	int garbage_position = rand() % 10;
	for (int i = 0; i < this->incomming_garbage; ++i) {
		for (int k = 0; k < 10; ++k) {
			this->board.column[k] = (this->board.column[k] << 1) | (uint64_t)0b1;
		}
		this->board.column[garbage_position] = this->board.column[garbage_position] & (~((uint64_t)0b1));

		if (rand() % 100 >= 70) garbage_position = rand() % 10;
	}

	this->incomming_garbage = 0;
}

void TetrisPlayer::send_garbage(int line)
{
	if (this->enemy == nullptr) return;
	this->enemy->incomming_garbage += line;
	this->line_sent += line;
}

int TetrisPlayer::count_fullline()
{
	BitBoard copy = this->board;
	return copy.clear_line();
}

void TetrisPlayer::update_clearline()
{
	++this->clearline_cnter;
	if (this->clearline_cnter >= PLAYER_DELAY_LINE_CLEAR) {
		this->clearline_cnter = PLAYER_DELAY_LINE_CLEAR + 1;
		++this->non_clearline_cnter;
		if (this->non_clearline_cnter >= 2) this->non_clearline_cnter = 2;
	}
	else {
		this->non_clearline_cnter = 0;
	}
}

bool TetrisPlayer::is_clearline()
{
	return this->clearline_cnter < PLAYER_DELAY_LINE_CLEAR;
}

bool TetrisPlayer::just_clearline()
{
	return this->non_clearline_cnter == 1;
}

void TetrisPlayer::start_clearline()
{
	this->clearline_cnter = 0;
	this->non_clearline_cnter = 0;
}

};