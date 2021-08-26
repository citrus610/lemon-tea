#include "piece.h"

tetris_piece::tetris_piece()
{
}

void tetris_piece::set_type(char _type)
{
	this->type = _type;
}

char tetris_piece::get_type()
{
	return this->type;
}

void tetris_piece::init()
{
	this->y = 18;
	this->x = tetris_piece_init_pos[this->type];
	this->form = 0;
	this->locked = false;

	on_floor_timer = 0.0;
	das_delay_timer = 0.0;
	das_repeat_timer = 0.0;
	soft_drop_timer = 0.0;
}

void tetris_piece::lock()
{
	this->locked = true;
}

bool tetris_piece::is_lock()
{
	return this->locked;
}

bool tetris_piece::is_colliding(int board[40][10], int x, int y)
{
	for (size_t _y = 0; _y < tetris_piece_form[this->type][this->form].size(); ++_y) {
		for (size_t _x = 0; _x < tetris_piece_form[this->type][this->form][_y].size(); ++_x) {
			if (tetris_piece_form[this->type][this->form][_y][_x] > 0) {
				if (_y + y > 39) return true;
				if (_x + x < 0 || _x + x > 9) return true;
				if (board[_y + y][_x + x] > 0) return true;
			}
		}
	}

	return false;
}

bool tetris_piece::is_lock_tspin(int board[40][10])
{
	return is_colliding(board, this->x - 1, this->y) && is_colliding(board, this->x + 1, this->y) && is_colliding(board, this->x, this->y - 1) && this->get_type() == 'T';
}

void tetris_piece::try_left(int board[40][10])
{
	if (!is_colliding(board, this->x - 1, this->y)) { this->x--; this->on_floor_timer = 0.0; }
}

void tetris_piece::try_right(int board[40][10])
{
	if (!is_colliding(board, this->x + 1, this->y)) { this->x++; this->on_floor_timer = 0.0; }
}

void tetris_piece::try_down(int board[40][10])
{
	if (!is_colliding(board, this->x, this->y + 1)) { this->y++; }
}

void tetris_piece::try_hard_drop(int board[40][10])
{
	while (!is_colliding(board, this->x, this->y + 1)) { this->y++; }
	this->lock();
}

void tetris_piece::try_rotate(int board[40][10], int dir)
{
	/*
	direction = 1 -> rotate right
	direction = 3 -> rotate left
	*/
	int oldForm = this->form;
	this->form = (this->form + dir) % (tetris_piece_form[this->get_type()].size());
	int k = 0;
	switch (this->get_type())
	{
	case 'I':
		if ((oldForm == 0 && this->form == 1) || (oldForm == 3 && this->form == 2)) //0 > R && L > 2
			k = 0;
		else if ((oldForm == 1 && this->form == 0) || (oldForm == 2 && this->form == 3)) //R > 0 && 2 > L
			k = 1;
		else if ((oldForm == 1 && this->form == 2) || (oldForm == 0 && this->form == 3)) //R > 2 && 0 > L
			k = 2;
		else if ((oldForm == 2 && this->form == 1) || (oldForm == 3 && this->form == 0)) //2 > R && L > 0
			k = 3;
		break;
	case 'O':
		break;
	default:
		if (this->form == 1) //From smt to R
			k = 0;
		else if (this->form == 3) //From smt to L
			k = 2;
		else
			if (oldForm == 1) //From R to smt
				k = 1;
			else if (oldForm == 3) //From R to smt
				k = 3;
		break;
	}
	if (this->get_type() != 'O') {
		if (this->is_colliding(board, this->x, this->y)) {
			int c = 0;
			for (int i = 0; i < 4; i++) {
				if (!this->is_colliding(board, this->x + tetris_piece_srs_data[this->get_type()][k][i][0], this->y + tetris_piece_srs_data[this->get_type()][k][i][1])) {
					this->x += tetris_piece_srs_data[this->get_type()][k][i][0];
					this->y += tetris_piece_srs_data[this->get_type()][k][i][1];
					break;
				}
				c++;
			}
			if (c == 4) {
				this->form = oldForm;
			}
		}
	}
	this->on_floor_timer = 0.0;
}

void tetris_piece::update_lock(double dt, int board[40][10])
{
	if (this->is_colliding(board, this->x, this->y + 1)) {
		this->on_floor_timer += dt;
	}
	else {
		this->on_floor_timer = 0;
	}
	if (this->on_floor_timer >= on_floor) {
		this->lock();
	}
}

void tetris_piece::update(double dt, int board[40][10])
{
	if (!this->enable_bot) {

		// das
		if (input::right.isPressed()) {
			if (this->das_delay_timer == 0) { this->try_right(board); }
			this->das_delay_timer += dt;
			if (this->das_delay_timer >= das_delay) {
				this->das_delay_timer = das_delay;
				if (this->das_repeat_timer == 0) { this->try_right(board); }
				this->das_repeat_timer += dt;
				if (this->das_repeat_timer >= das_interval) {
					this->das_repeat_timer = 0;
				}
			}
		}
		else if (input::left.isPressed()) {
			if (this->das_delay_timer == 0) { this->try_left(board); }
			this->das_delay_timer += dt;
			if (this->das_delay_timer >= das_delay) {
				this->das_delay_timer = das_delay;
				if (this->das_repeat_timer == 0) { this->try_left(board); }
				this->das_repeat_timer += dt;
				if (this->das_repeat_timer >= das_interval) {
					this->das_repeat_timer = 0;
				}
			}
		}
		else {
			this->das_repeat_timer = 0;
			this->das_delay_timer = 0;
		}

		// hard drop
		if (input::space.justPressed()) {
			this->try_hard_drop(board);
		}

		// soft drop
		if (input::down.isPressed()) {
			if (input::down.justPressed()) {
				this->try_down(board);
			}
			this->soft_drop_timer += dt;
			if (this->soft_drop_timer >= soft_drop_interval) {
				this->soft_drop_timer = 0;
				this->try_down(board);
			}
		}
		else { this->soft_drop_timer = 0; }

		// rotating
		if (input::up.justPressed()) {
			this->try_rotate(board, 1);
		}
		if (input::z.justPressed()) {
			this->try_rotate(board, 3);
		}

		update_lock(dt, board);
	}
}
