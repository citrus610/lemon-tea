#ifndef SFML_GAME_BOT_H
#define SFML_GAME_BOT_H

#include "game.h"
#include "board.h"
#include "type.h"

class gamebot :
	public game
{
public:
	void load() override;
	void update(double dt) override;
	void render() override;
	void unload() override;
public:
	void start_game();
	void update_game(double dt);
	void end_game();
private:
	double gameover_timer = 0.0;
	int gameover_counter = 0;

	sf::Texture img_counter; sf::Sprite sp_counter;
public:
	tetris_board board_1 = tetris_board(80, 140), board_2 = tetris_board(1040, 140);
public:
	void handle_bot_input(double dt, int id, tetris_board& _board, bot& _bot, double& bot_input_timer, double& bot_elaspe_time, std::vector<move>& solution_vec);

	bool enable_bot_1 = true, enable_bot_2 = true;
	weight w_1, w_2;

	double const bot_input_delay = 0.0466;

	bot bot_1;
	double bot_input_timer_1 = 0.0;
	std::vector<move> solution_1;
	double elaspe_time_1 = 0.0;

	bot bot_2;
	double bot_input_timer_2 = 0.0;
	std::vector<move> solution_2;
	double elaspe_time_2 = 0.0;
};

#endif