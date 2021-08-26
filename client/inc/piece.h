#ifndef SFML_PIECE_H
#define SFML_PIECE_H

#include <iostream>
#include <unordered_map>
#include <vector>
#include <array>
#include <random>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "input.h"

class tetris_piece
{
public:
	tetris_piece();

	int x = 0, y = 0, form = 0;

	void set_type(char _type);
	char get_type();

	void init();
	void lock();
	bool is_lock();
	bool is_colliding(int board[40][10], int x, int y);
	bool is_lock_tspin(int board[40][10]);

	void try_left(int board[40][10]);
	void try_right(int board[40][10]);
	void try_down(int board[40][10]);
	void try_hard_drop(int board[40][10]);
	void try_rotate(int board[40][10], int dir);

	void update_lock(double dt, int board[40][10]);
	void update(double dt, int board[40][10]);

	const double on_floor = 0.5, das_delay = 0.25, das_interval = 0.033, soft_drop_interval = 0.033;
private:
	char type = ' ';
	bool locked = false;

	double on_floor_timer = 0.0, das_delay_timer = 0.0, das_repeat_timer = 0.0, soft_drop_timer = 0.0;
public:
	bool enable_bot = false;
};

static std::unordered_map<char, int> tetris_piece_init_pos{
	{'I', 3},
	{'J', 3},
	{'L', 3},
	{'S', 3},
	{'T', 3},
	{'Z', 3},
	{'O', 4}
};
static std::unordered_map<char, std::vector<std::vector<std::vector<int>>>> tetris_piece_form{
	{'I',
		{
			{
				{0, 0, 0, 0},
				{1, 1, 1, 1},
				{0, 0, 0, 0},
				{0, 0, 0, 0}
			},
			{
				{0, 0, 1, 0},
				{0, 0, 1, 0},
				{0, 0, 1, 0},
				{0, 0, 1, 0}
			},
			{
				{0, 0, 0, 0},
				{0, 0, 0, 0},
				{1, 1, 1, 1},
				{0, 0, 0, 0}
			},
			{
				{0, 1, 0, 0},
				{0, 1, 0, 0},
				{0, 1, 0, 0},
				{0, 1, 0, 0}
			}
		}
	},
	{'J',
		{
			{
				{2, 0, 0},
				{2, 2, 2},
				{0, 0, 0}
			},
			{
				{0, 2, 2},
				{0, 2, 0},
				{0, 2, 0}
			},
			{
				{0, 0, 0},
				{2, 2, 2},
				{0, 0, 2}
			},
			{
				{0, 2, 0},
				{0, 2, 0},
				{2, 2, 0}
			}
		}
	},
	{'L',
		{
			{
				{0, 0, 3},
				{3, 3, 3},
				{0, 0, 0}
			},
			{
				{0, 3, 0},
				{0, 3, 0},
				{0, 3, 3}
			},
			{
				{0, 0, 0},
				{3, 3, 3},
				{3, 0, 0}
			},
			{
				{3, 3, 0},
				{0, 3, 0},
				{0, 3, 0}
			}
		}
	},
	{'S',
		{
			{
				{0, 5, 5},
				{5, 5, 0},
				{0, 0, 0}
			},
			{
				{0, 5, 0},
				{0, 5, 5},
				{0, 0, 5}
			},
			{
				{0, 0, 0},
				{0, 5, 5},
				{5, 5, 0}
			},
			{
				{5, 0, 0},
				{5, 5, 0},
				{0, 5, 0}
			}
		}
	},
	{'T',
		{
			{
				{0, 6, 0},
				{6, 6, 6},
				{0, 0, 0}
			},
			{
				{0, 6, 0},
				{0, 6, 6},
				{0, 6, 0}
			},
			{
				{0, 0, 0},
				{6, 6, 6},
				{0, 6, 0}
			},
			{
				{0, 6, 0},
				{6, 6, 0},
				{0, 6, 0}
			}
		}
	},
	{'Z',
		{
			{
				{7, 7, 0},
				{0, 7, 7},
				{0, 0, 0}
			},
			{
				{0, 0, 7},
				{0, 7, 7},
				{0, 7, 0}
			},
			{
				{0, 0, 0},
				{7, 7, 0},
				{0, 7, 7}
			},
			{
				{0, 7, 0},
				{7, 7, 0},
				{7, 0, 0}
			}
		}
	},
	{'O',
		{
			{
				{4, 4},
				{4, 4}
			}
		}
	}
};;
static std::unordered_map<char, std::vector<std::vector<std::vector<int>>>> tetris_piece_srs_data{
	{'I',{
	{{ -2, 0 }, { 1, 0 }, { -2, 1 }, { 1, -2 }},
	{{ 2, 0 }, { -1, 0 }, { 2, -1 }, { -1, 2 }},
	{{ -1, 0 }, { 2, 0 }, { -1, -2 }, { 2, 1 }},
	{{ 1, 0 }, { -2, 0 }, { 1, 2 }, { -2, -1 }}
	}},
	{'J',{
	{{ -1, 0 }, { -1, -1 }, { 0, 2 }, { -1, 2 }},
	{{ 1, 0 }, { 1, 1 }, { 0, -2 }, { 1, -2 }},
	{{ 1, 0 }, { 1, -1 }, { 0, 2 }, { 1, 2 }},
	{{ -1, 0 }, { -1, 1 }, { 0, -2 }, { -1, -2 }}
	}},
	{'L', {
	{{ -1, 0 }, { -1, -1 }, { 0, 2 }, { -1, 2 }},
	{{ 1, 0 }, { 1, 1 }, { 0, -2 }, { 1, -2 }},
	{{ 1, 0 }, { 1, -1 }, { 0, 2 }, { 1, 2 }},
	{{ -1, 0 }, { -1, 1 }, { 0, -2 }, { -1, -2 }}
	}},
	{'S', {
	{{ -1, 0 }, { -1, -1 }, { 0, 2 }, { -1, 2 }},
	{{ 1, 0 }, { 1, 1 }, { 0, -2 }, { 1, -2 }},
	{{ 1, 0 }, { 1, -1 }, { 0, 2 }, { 1, 2 }},
	{{ -1, 0 }, { -1, 1 }, { 0, -2 }, { -1, -2 }}
	}},
	{'T', {
	{{ -1, 0 }, { -1, -1 }, { 0, 2 }, { -1, 2 }},
	{{ 1, 0 }, { 1, 1 }, { 0, -2 }, { 1, -2 }},
	{{ 1, 0 }, { 1, -1 }, { 0, 2 }, { 1, 2 }},
	{{ -1, 0 }, { -1, 1 }, { 0, -2 }, { -1, -2 }}
	}},
	{'Z', {
	{{ -1, 0 }, { -1, -1 }, { 0, 2 }, { -1, 2 }},
	{{ 1, 0 }, { 1, 1 }, { 0, -2 }, { 1, -2 }},
	{{ 1, 0 }, { 1, -1 }, { 0, 2 }, { 1, 2 }},
	{{ -1, 0 }, { -1, 1 }, { 0, -2 }, { -1, -2 }}
	}},
	{'O', {
	{{ -1, 0 }, { -1, -1 }, { 0, 2 }, { -1, 2 }},
	{{ 1, 0 }, { 1, 1 }, { 0, -2 }, { 1, -2 }},
	{{ 1, 0 }, { 1, -1 }, { 0, 2 }, { 1, 2 }},
	{{ -1, 0 }, { -1, 1 }, { 0, -2 }, { -1, -2 }}
	}}
};

#endif