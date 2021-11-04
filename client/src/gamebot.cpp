#include "gamebot.h"

void gamebot::load()
{
	// Init bot
	create_json();
	std::ifstream file;
	file.open("init.json");
	json js;
	file >> js;
	set_from_json(js, 1, enable_bot_1, w_1, bot_speed_percentage_1, bot_preview_1, bot_forecast_1);
	set_from_json(js, 2, enable_bot_2, w_2, bot_speed_percentage_2, bot_preview_2, bot_forecast_2);
	file.close();

	// Init game
	srand((unsigned int)time(NULL));
	this->img_counter.loadFromFile("res/graphic/counter.png");
	this->sp_counter.setTexture(this->img_counter);
	this->board_1.set_enemy(&this->board_2);
	this->board_2.set_enemy(&this->board_1);
	this->board_1.preview = bot_preview_1;
	this->board_2.preview = bot_preview_2;
	this->board_1.init();
	this->board_2.init();
	this->gameover_timer = 1.0;
	this->gameover_counter = 119;

	if (this->enable_bot_1) {
		this->w_1.standard();
		this->board_1.enable_bot();
	}
	if (this->enable_bot_2) {
		this->w_2.standard();
		this->board_2.enable_bot();
	}
}

void gamebot::update(double dt)
{
	this->gameover_counter++;

	if (this->gameover_counter == 120) {
		this->start_game();
		++this->gameover_counter;
	}

	if (this->gameover_counter >= 300) {
		this->gameover_counter = 301;
		this->update_game(dt);
		if (this->board_1.is_game_over() || this->board_2.is_game_over() || input::x.justPressed()) {
			this->gameover_counter = 0;
			this->end_game();
		}
	}
}

void gamebot::render()
{
	this->canvas.clear();
	this->canvas.setSmooth(true);
	this->board_1.render(&this->canvas);
	this->board_2.render(&this->canvas);

	this->sp_counter.setOrigin(10, 15);
	this->sp_counter.setPosition(960, 525);
	this->sp_counter.setScale(4, 4);
	if (this->gameover_counter > 120) {
		if (this->gameover_counter < 180) {
			this->sp_counter.setTextureRect(sf::IntRect(40, 0, 20, 30));
			this->canvas.draw(this->sp_counter);
		}
		else if (this->gameover_counter < 240) {
			this->sp_counter.setTextureRect(sf::IntRect(20, 0, 20, 30));
			this->canvas.draw(this->sp_counter);
		}
		else if (this->gameover_counter < 300) {
			this->sp_counter.setTextureRect(sf::IntRect(0, 0, 20, 30));
			this->canvas.draw(this->sp_counter);
		}
	}
}

void gamebot::unload()
{
	if (this->enable_bot_1) this->bot_1.end_thread();
	if (this->enable_bot_2) this->bot_2.end_thread();
}

void gamebot::start_game()
{
	this->board_1.init();
	this->board_2.init();

	this->piece_placed_count_1 = 0;
	this->piece_placed_count_2 = 0;

	if (this->enable_bot_1) {
		BotState data_1 = board_to_bot_data(this->board_1);
		this->bot_1.init_thread({this->w_1, false}, data_1);
	}

	if (this->enable_bot_2) {
		BotState data_2 = board_to_bot_data(this->board_2);
		this->bot_2.init_thread({this->w_2, false}, data_2);
	}
}

void gamebot::update_game(double dt)
{
	if (this->enable_bot_1) this->handle_bot_input(dt, 1, this->board_1, this->bot_1, this->piece_placed_count_1, this->bot_speed_percentage_1, this->bot_input_timer_1, this->elaspe_time_1, this->solution_1);
	if (this->enable_bot_2) this->handle_bot_input(dt, 2, this->board_2, this->bot_2, this->piece_placed_count_2, this->bot_speed_percentage_2, this->bot_input_timer_2, this->elaspe_time_2, this->solution_2);

	this->board_1.update(dt);
	this->board_2.update(dt);
}

void gamebot::end_game()
{
	this->solution_1.clear();
	this->bot_input_timer_1 = 0.0;
	if (this->enable_bot_1) this->bot_1.end_thread();

	this->solution_2.clear();
	this->bot_input_timer_2 = 0.0;
	if (this->enable_bot_2) this->bot_2.end_thread();
}

void gamebot::handle_bot_input(double dt, int id, tetris_board& _board, Bot& _bot, int& piece_placed_count, int& bot_speed_percentage, double& bot_input_timer, double& bot_elaspe_time, std::vector<MoveType>& solution_vec)
{
	bot_elaspe_time += dt;

	if (!_board.is_clearing_line() && !_board.is_placing_piece()) {
		if (solution_vec.empty()) {
			BotSolution solution = _bot.request_solution();
			_board.bot_elapse_time = bot_elaspe_time * 1000;

			// Check if misdrop
			// Disable for now
			///*
			BotState current_state = board_to_bot_data(_board);
			BitBoard solution_board_copy = solution.original_board;
			bool board_change = !(current_state.board == solution_board_copy);
			if (board_change) {
				Tree sub_tree;
				sub_tree.init();
				sub_tree.set(current_state.board, current_state.current, current_state.hold, current_state.next, current_state.next_count, current_state.b2b, current_state.ren);
				solution.node = sub_tree.search(7);
				solution.depth = 5;
				solution.action = sub_tree.get_best().origin;
				solution.move_count = 0;
				PathFinder::search(current_state.board, solution.action.placement, solution.move, solution.move_count);
			}
			//*/

			// Hold
			char before_board_hold = _board.hold_piece;
			if (solution.action.hold) _board.hold();

			// Push moves to bot input vec
			solution_vec.clear();
			for (int i = 0; i < solution.move_count; ++i) {
				solution_vec.push_back(solution.move[i]);
			}

			// Advance or set bot
			if (!board_change) {
				BotAction bot_action;
				bot_action.action = solution.action;
				bot_action.new_piece_count = 0;
				if (before_board_hold == ' ' && solution.action.hold) {
					++bot_action.new_piece_count;
				}
				if (piece_placed_count > 0) {
					++bot_action.new_piece_count;
				}
				++piece_placed_count;
				for (int i = 0; i < bot_action.new_piece_count; ++i) {
					bot_action.new_piece[i] = char_to_piece(_board.next_piece[(int)_board.preview - bot_action.new_piece_count + i]);
				}

				_bot.advance_state(bot_action);
			}
			else {
				BotState state;
				state.board = current_state.board;
				state.board.place_piece(solution.action.placement);
				state.board.clear_line();
				state.current = char_to_piece(_board.next_piece[0]);
				state.hold = char_to_piece(_board.hold_piece);
				state.next_count = _board.preview - 1;
				for (int i = 0; i < _board.preview - 1; ++i) {
					state.next[i] = char_to_piece(_board.next_piece[i + 1]);
				}
				state.b2b = current_state.b2b;
				state.ren = current_state.ren;
				_bot.set_state(state);
			}

			// Setting bot log thingy
			bot_input_timer = 0.0;
			_board.bot_node = solution.node;
			_board.bot_depth = solution.depth;
			bot_elaspe_time = 0.0;
		}
		else {
			bot_input_timer += dt;
			switch (solution_vec[0])
			{
			case MOVE_CW:
				_board.real_piece.try_rotate(_board.data, 1);
				bot_input_timer = 0.0;
				solution_vec.erase(solution_vec.begin());
				break;
			case MOVE_CCW:
				_board.real_piece.try_rotate(_board.data, 3);
				bot_input_timer = 0.0;
				solution_vec.erase(solution_vec.begin());
				break;
			case MOVE_LEFT:
				if (bot_input_timer >= this->bot_input_delay / (double)bot_speed_percentage * 100) {
					_board.real_piece.try_left(_board.data);
					bot_input_timer = 0.0;
					solution_vec.erase(solution_vec.begin());
				}
				break;
			case MOVE_RIGHT:
				if (bot_input_timer >= this->bot_input_delay / (double)bot_speed_percentage * 100) {
					_board.real_piece.try_right(_board.data);
					bot_input_timer = 0.0;
					solution_vec.erase(solution_vec.begin());
				}
				break;
			case MOVE_DOWN:
				if (solution_vec.size() == (size_t)1) {
					if (bot_input_timer >= this->bot_input_delay / (double)bot_speed_percentage * 100) {
						_board.real_piece.try_hard_drop(_board.data);
						bot_input_timer = 0.0;
						solution_vec.clear();
					}
				}
				else {
					if (bot_input_timer >= this->bot_input_delay) {
						_board.real_piece.try_down(_board.data);
						bot_input_timer = 0.0;
						if (_board.real_piece.is_colliding(_board.data, _board.real_piece.x, _board.real_piece.y + 1)) solution_vec.erase(solution_vec.begin());
					}
				}
				break;
			default:
				break;
			}
		}
	}
}
