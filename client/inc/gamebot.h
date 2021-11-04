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
	void handle_bot_input(double dt, int id, tetris_board& _board, Bot& _bot, int& piece_placed_count, int& bot_speed_percentage, double& bot_input_timer, double& bot_elaspe_time, std::vector<MoveType>& solution_vec);

	bool enable_bot_1 = true, enable_bot_2 = true;

	double const bot_input_delay = 0.0466;


	// BOT 1
	Bot bot_1;
	double bot_input_timer_1 = 0.0;
	std::vector<MoveType> solution_1;
	double elaspe_time_1 = 0.0;
	int piece_placed_count_1 = 0;

	// Json setting
	Weight w_1;
	int bot_speed_percentage_1 = 100;
	int bot_preview_1 = 4;
	bool bot_forecast_1 = true;



	// BOT 2
	Bot bot_2;
	double bot_input_timer_2 = 0.0;
	std::vector<MoveType> solution_2;
	double elaspe_time_2 = 0.0;
	int piece_placed_count_2 = 0;

	// Json setting
	Weight w_2;
	int bot_speed_percentage_2 = 100;
	int bot_preview_2 = 4;
	bool bot_forecast_2 = true;
};

#endif