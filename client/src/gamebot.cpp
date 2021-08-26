#include "gamebot.h"

void gamebot::load()
{
	srand((unsigned int)time(NULL));
	this->img_counter.loadFromFile("res/graphic/counter.png");
	this->sp_counter.setTexture(this->img_counter);
	this->board_1.set_enemy(&this->board_2);
	this->board_2.set_enemy(&this->board_1);
	this->board_1.init();
	this->board_2.init();
	this->gameover_timer = 1.0;
	this->gameover_counter = 119;

	/*
	create_init_js();
	std::ifstream file;
	file.open("init.json");
	json js;
	file >> js;
	set_weight_js(this->w_1, js, 1);
	set_weight_js(this->w_2, js, 2);
	file.close();
	*/

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
	//this->gameover_timer += dt;
	//if (this->gameover_timer > 0.016) {
		this->gameover_counter++;
	//	this->gameover_timer = 0.0;
	//}

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
	if (this->enable_bot_1) this->bot_1.destroy();
	if (this->enable_bot_2) this->bot_2.destroy();
}

void gamebot::start_game()
{
	this->board_1.init();
	this->board_2.init();

	if (this->enable_bot_1) {
		this->bot_1.start(5, this->w_1, true);
		bot_new_state data_1 = board_to_bot_data(this->board_1);
		this->bot_1.set_state(data_1);
	}

	if (this->enable_bot_2) {
		this->bot_2.start(5, this->w_2, true);
		bot_new_state data_2 = board_to_bot_data(this->board_2);
		this->bot_2.set_state(data_2);
	}
}

void gamebot::update_game(double dt)
{
	if (this->enable_bot_1) this->handle_bot_input(dt, 1, this->board_1, this->bot_1, this->bot_input_timer_1, this->elaspe_time_1, this->solution_1);
	if (this->enable_bot_2) this->handle_bot_input(dt, 2, this->board_2, this->bot_2, this->bot_input_timer_2, this->elaspe_time_2, this->solution_2);

	this->board_1.update(dt);
	this->board_2.update(dt);
}

void gamebot::end_game()
{
	this->solution_1.clear();
	this->bot_input_timer_1 = 0.0;
	if (this->enable_bot_1) this->bot_1.destroy();

	this->solution_2.clear();
	this->bot_input_timer_2 = 0.0;
	if (this->enable_bot_2) this->bot_2.destroy();
}

void gamebot::handle_bot_input(double dt, int id, tetris_board& _board, bot& _bot, double& bot_input_timer, double& bot_elaspe_time, std::vector<move>& solution_vec)
{
	bot_elaspe_time += dt;

	if (!_board.is_clearing_line() && !_board.is_placing_piece()) {
		if (solution_vec.empty()) {
			bot_solution solution = _bot.request_solution();
			_board.bot_elapse_time = bot_elaspe_time * 1000;
			if (solution.is_hold) _board.hold();

			// Check if misdrop
			// Disable for now
			/*
			bot_new_state current_state = board_to_bot_data(_board);
			bitboard solution_board_copy = solution.board;
			if (!(current_state.board == solution_board_copy) && id == 1) {
				std::cout << "misdrop board " << id << std::endl << std::endl;

				std::cout << "what bot receive" << std::endl;
				draw_board(solution_board_copy);

				std::cout << "real board" << std::endl;
				draw_board(current_state.board);
				std::cout << std::endl;



				std::cout << "what bot prediect" << std::endl;
				solution_board_copy.place_piece(solution.placement);
				solution_board_copy.clear_line();
				draw_board(solution_board_copy);

				std::cout << "real board place piece" << std::endl;
				piece_data s_p; s_p.x = 4; s_p.y = 19; s_p.rotation = PIECE_UP; s_p.type = solution.placement.type;
				for (int i = 0; i < solution.move_list_count; ++i) {
					switch (solution.move_list[i])
					{
					case MOVE_RIGHT:
						current_state.board.piece_try_right(s_p);
						break;
					case MOVE_LEFT:
						current_state.board.piece_try_left(s_p);
						break;
					case MOVE_CW:
						current_state.board.piece_try_rotate(s_p, true);
						break;
					case MOVE_CCW:
						current_state.board.piece_try_rotate(s_p, false);
						break;
					case MOVE_DOWN:
						current_state.board.piece_try_down(s_p);
						break;
					}
				}
				current_state.board.place_piece(s_p);
				current_state.board.clear_line();
				draw_board(current_state.board);
				std::cout << std::endl;
				std::cout << std::endl << std::endl;
			}
			//*/

			// Push moves to bot input vec
			solution_vec.clear();
			for (int i = 0; i < solution.move_list_count; ++i) {
				solution_vec.push_back(solution.move_list[i]);
			}
			if (solution_vec.back() != MOVE_DOWN) {
				solution_vec.push_back(MOVE_DOWN);
			}

			// Predict next board
			piece_type next_q[16];
			int next_q_count = 0;
			for (int i = 0; i < std::min((int)_board.next_piece.size(), 16); ++i) {
				next_q[i] = (char_to_piece(_board.next_piece[i]));
				next_q_count++;
			}

			node old_n;
			node new_n;
			old_n = board_to_node(_board);
			piece_data s_p; s_p.x = 4; s_p.y = 19; s_p.rotation = PIECE_UP; s_p.type = solution.placement.type;
			for (int i = 0; i < solution.move_list_count; ++i) {
				switch (solution.move_list[i])
				{
				case MOVE_RIGHT:
					old_n.board.piece_try_right(s_p);
					break;
				case MOVE_LEFT:
					old_n.board.piece_try_left(s_p);
					break;
				case MOVE_CW:
					old_n.board.piece_try_rotate(s_p, true);
					break;
				case MOVE_CCW:
					old_n.board.piece_try_rotate(s_p, false);
					break;
				case MOVE_DOWN:
					old_n.board.piece_try_down(s_p);
					break;
				}
			}
			old_n.board.piece_try_down(s_p);
			new_n.attemp(old_n, s_p, false, next_q, next_q_count);
			for (int i = 0; i < next_q_count - 1; ++i) {
				next_q[i] = next_q[i + 1];
			}
			next_q_count--;

			// Send bot predicted board state
			bot_new_state state;
			state.board = new_n.board;
			state.current = char_to_piece(_board.next_piece[0]);
			state.hold = new_n.hold;
			memcpy(state.next, next_q, next_q_count * sizeof(piece_type));
			state.b2b = new_n.b2b;
			state.ren = new_n.ren;
			_bot.set_state(state);

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
				if (bot_input_timer >= this->bot_input_delay) {
					_board.real_piece.try_left(_board.data);
					bot_input_timer = 0.0;
					solution_vec.erase(solution_vec.begin());
				}
				break;
			case MOVE_RIGHT:
				if (bot_input_timer >= this->bot_input_delay) {
					_board.real_piece.try_right(_board.data);
					bot_input_timer = 0.0;
					solution_vec.erase(solution_vec.begin());
				}
				break;
			case MOVE_DOWN:
				if (solution_vec.size() == (size_t)1) {
					if (bot_input_timer >= this->bot_input_delay) {
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
