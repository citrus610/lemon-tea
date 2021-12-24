#include "tetris.h"

namespace TetrisGame
{

void Tetris::load()
{
    create_json();
	std::ifstream file;
	file.open("config.json");
	json js;
	file >> js;
	set_from_json(js, this->battle.w_1);
    this->battle.w_2 = this->battle.w_1;
	file.close();
    this->battle.init();
};

void Tetris::update(double dt)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(34));
    this->battle.update();
    if (this->battle.is_gameover()) {
        this->battle.unload();
        this->draw_text(0, 0, std::wstring(L"GAME OVER! 3"), ConsoleEngine::COLOR_BG_WHITE);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        this->draw_text(0, 0, std::wstring(L"GAME OVER! 2"), ConsoleEngine::COLOR_BG_WHITE);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        this->draw_text(0, 0, std::wstring(L"GAME OVER! 1"), ConsoleEngine::COLOR_BG_WHITE);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        this->end();
    }
};

void Tetris::render()
{
    this->clear();
    this->render_battle(this->battle);
};

void Tetris::unload()
{
    this->battle.unload();
};

void Tetris::render_battle(TetrisBattle& tetris_battle)
{
    int board_1_x = 11;
    int board_2_x = 32 + 11;
    int board_y = 5;

    // Draw board incomming attack
    this->draw_rectangle(board_1_x - 2, board_y + 22 - tetris_battle.player_1.incomming_garbage, 14, tetris_battle.player_1.incomming_garbage, ConsoleEngine::PIXEL_SOLID, ConsoleEngine::COLOR_FG_RED);
    this->draw_rectangle(board_2_x - 2, board_y + 22 - tetris_battle.player_2.incomming_garbage, 14, tetris_battle.player_2.incomming_garbage, ConsoleEngine::PIXEL_SOLID, ConsoleEngine::COLOR_FG_RED);

    // Draw board outline
    this->draw_rectangle(board_1_x - 1, board_y - 1, 12, 24, ConsoleEngine::PIXEL_SOLID, ConsoleEngine::COLOR_FG_DARK_GREY);
    this->draw_rectangle(board_2_x - 1, board_y - 1, 12, 24, ConsoleEngine::PIXEL_SOLID, ConsoleEngine::COLOR_FG_DARK_GREY);

    // Draw board inside
    this->draw_rectangle(board_1_x, board_y, 10, 22, ConsoleEngine::PIXEL_SOLID, ConsoleEngine::COLOR_FG_BLACK);
    this->draw_rectangle(board_2_x, board_y, 10, 22, ConsoleEngine::PIXEL_SOLID, ConsoleEngine::COLOR_FG_BLACK);

    // Draw board danger height inside
    this->draw_rectangle(board_1_x, board_y, 10, 2, (ConsoleEngine::PixelType)L'X', ConsoleEngine::COLOR_FG_RED);
    this->draw_rectangle(board_2_x, board_y, 10, 2, (ConsoleEngine::PixelType)L'X', ConsoleEngine::COLOR_FG_RED);

    // Draw board data
    BitBoard board_1_copy = tetris_battle.player_1.board;
    BitBoard board_2_copy = tetris_battle.player_2.board;
    // board_1_copy.place_piece(tetris_battle.player_1.piece);
    // board_2_copy.place_piece(tetris_battle.player_2.piece);
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 22; ++y) {
            if (board_1_copy.is_occupied(x, y)) {
                this->draw(x + board_1_x, board_y + 21 - y, ConsoleEngine::PIXEL_SOLID, ConsoleEngine::COLOR_FG_WHITE);
            }
            if (board_2_copy.is_occupied(x, y)) {
                this->draw(x + board_2_x, board_y + 21 - y, ConsoleEngine::PIXEL_SOLID, ConsoleEngine::COLOR_FG_WHITE);
            }
        }
    }

    // Draw current piece
    for (int i = 0; i < 4; ++i) {
        if (tetris_battle.player_1.current != PIECE_NONE) {
            int x_off = PIECE_LUT[tetris_battle.player_1.current][tetris_battle.player_1.piece.rotation][i][0];
            int y_off = -PIECE_LUT[tetris_battle.player_1.current][tetris_battle.player_1.piece.rotation][i][1];
            this->draw(board_1_x + x_off + tetris_battle.player_1.piece.x, board_y + 21 - tetris_battle.player_1.piece.y + y_off, ConsoleEngine::PIXEL_SOLID, ConsoleEngine::COLOR_FG_GREY);
        }
        if (tetris_battle.player_2.current != PIECE_NONE) {
            int x_off = PIECE_LUT[tetris_battle.player_2.current][tetris_battle.player_2.piece.rotation][i][0];
            int y_off = -PIECE_LUT[tetris_battle.player_2.current][tetris_battle.player_2.piece.rotation][i][1];
            this->draw(board_2_x + x_off + tetris_battle.player_2.piece.x, board_y + 21 - tetris_battle.player_2.piece.y + y_off, ConsoleEngine::PIXEL_SOLID, ConsoleEngine::COLOR_FG_GREY);
        }
    }

    // Draw hold
    int hold_1_x = 5;
    int hold_2_x = 32 + 5;
    int hold_y = 7;
    for (int i = 0; i < 4; ++i) {
        if (tetris_battle.player_1.hold != PIECE_NONE) {
            int x_off = PIECE_LUT[tetris_battle.player_1.hold][PIECE_UP][i][0];
            int y_off = -PIECE_LUT[tetris_battle.player_1.hold][PIECE_UP][i][1];
            this->draw(hold_1_x + x_off, hold_y + y_off, ConsoleEngine::PIXEL_SOLID, ConsoleEngine::COLOR_FG_WHITE);
        }
        if (tetris_battle.player_2.hold != PIECE_NONE) {
            int x_off = PIECE_LUT[tetris_battle.player_2.hold][PIECE_UP][i][0];
            int y_off = -PIECE_LUT[tetris_battle.player_2.hold][PIECE_UP][i][1];
            this->draw(hold_2_x + x_off, hold_y + y_off, ConsoleEngine::PIXEL_SOLID, ConsoleEngine::COLOR_FG_WHITE);
        }
    }

    // Draw next
    int next_1_x = 26;
    int next_2_x = 32 + 26;
    int next_y = 7;
    for (int y = 0; y < std::min(5, PLAYER_NEXT_COUNT); ++y) {
        for (int i = 0; i < 4; ++i) {
            if (tetris_battle.player_1.next[y] != PIECE_NONE) {
                int x_off = PIECE_LUT[tetris_battle.player_1.next[y]][PIECE_UP][i][0];
                int y_off = -PIECE_LUT[tetris_battle.player_1.next[y]][PIECE_UP][i][1];
                this->draw(next_1_x + x_off, next_y + y * 4 + y_off, ConsoleEngine::PIXEL_SOLID, ConsoleEngine::COLOR_FG_WHITE);
            }
            if (tetris_battle.player_2.next[y] != PIECE_NONE) {
                int x_off = PIECE_LUT[tetris_battle.player_2.next[y]][PIECE_UP][i][0];
                int y_off = -PIECE_LUT[tetris_battle.player_2.next[y]][PIECE_UP][i][1];
                this->draw(next_2_x + x_off, next_y + y * 4 + y_off, ConsoleEngine::PIXEL_SOLID, ConsoleEngine::COLOR_FG_WHITE);
            }
        }
    }
};

}