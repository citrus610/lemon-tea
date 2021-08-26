#ifndef SFML_BOARD_H
#define SFML_BOARD_H

#include <sstream>
#include <locale>

#include "piece.h"

class tetris_board
{
public:
	tetris_board(int x, int y);

	int x, y;
	int data[40][10];
	char current_piece = ' ';
	char hold_piece = ' ';
	std::vector<char> next_piece;
	bool b2b;
	int ren;
	char bag[7] = { 'I', 'J', 'L', 'S', 'T', 'Z', 'O' };
	tetris_piece real_piece;
	int garbage = 0;

	void init();
	void set_enemy(tetris_board* _enemy);
	void shuffle_bag();
	void fill_queue();
	void change_piece();

	void hold();
	void place_piece();
	std::vector<int> full_row();
	std::vector<int> full_line_before_lock();
	void clear_line(std::vector<int> full_row_vec);
	int count_garbage(bool t_spin, std::vector<int> full_row_vec);
	void sent_garbage(int line);
	void place_garbage();

	bool is_game_over();
	bool is_clearing_line();
	bool is_placing_piece();
	bool just_done_clear_line();
	void update_clear_line(double dt);

	void update(double dt);
	void render(sf::RenderTexture* _canvas);

	const int preview = 6;
	const double line_clear_time = 0.5, piece_place_time = 0.03;
	const sf::Color color[8] = {
		sf::Color(0, 255, 255, 255),
		sf::Color(12, 80, 255, 255),
		sf::Color(255, 162, 0, 255),
		sf::Color(255, 255, 0, 255),
		sf::Color(124, 252, 0, 255),
		sf::Color(153, 69, 255, 255),
		sf::Color(255, 0, 79, 255),
		sf::Color(192, 192, 192, 255) };

private:
	tetris_board* enemy = nullptr;
	bool game_over = false;
	double in_game_timer = 0.0, clear_line_timer = 0.6, place_piece_timer = 0.06;
	int hold_counter = 0, piece_taken_counter = 0, clear_line_counter = 2;
	int g_per_dist[8] = { 1, 1, 2, 2, 3, 4, 4, 4 };
	std::unordered_map<char, int> piece_index = {
		{'I', 0},
		{'J', 1},
		{'L', 2},
		{'S', 3},
		{'T', 4},
		{'Z', 5},
		{'O', 6}
	};

	sf::Texture img_block, img_block_overlay, img_ghost_block, img_board, img_clear_line, img_tetrismino;
	sf::Sprite sp_block, sp_block_overlay, sp_ghost_block, sp_board, sp_clear_line, sp_tetrismino;
	sf::VertexArray red_bar;
public:
	void enable_bot();
	int bot_node = 0, bot_depth = 0;
	double bot_elapse_time = 0.0;
private:
	sf::Font font;
	sf::Text text;
};

template <typename T>
static bool is_in_vec(std::vector<T> vec, T value) {
	for (size_t i = 0; i < vec.size(); ++i) {
		if (vec[i] == value) return true;
	}
	return false;
};

struct dotted : std::numpunct<char> {
	char do_thousands_sep()   const { return ','; }  // separate with dots
	std::string do_grouping() const { return "\3"; } // groups of 3 digits
	static void imbue(std::ostream& os) {
		os.imbue(std::locale(os.getloc(), new dotted));
	}
};

static std::string d_int_to_string(int num) {
	std::stringstream ss;
	dotted::imbue(ss);
	ss << num;
	return ss.str();
};

#endif