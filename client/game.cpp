#include "game.h"

namespace ConsoleEngine
{

std::atomic<bool> Game::atomic_running(false);
std::condition_variable Game::cv_exit;
std::mutex Game::mutex;

Game::Game(int w, int h, int pixel_size)
{
    this->width = w;
    this->height = h;
    this->screen = new CHAR_INFO[width * height];
    memset(this->screen, 0, sizeof(CHAR_INFO) * this->width * this->height);

    this->console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    this->console_input_handle = GetStdHandle(STD_INPUT_HANDLE);

    for (int i = 0; i < 256; ++i) this->keyboard_state_new[i] = 0;
    for (int i = 0; i < 256; ++i) this->keyboard_state_old[i] = 0;
    for (int i = 0; i < 256; ++i) this->keys[i] = KeyState();
    for (int i = 0; i < 5; ++i) this->mouse_state_new[i] = false;
    for (int i = 0; i < 5; ++i) this->mouse_state_old[i] = false;
    for (int i = 0; i < 5; ++i) this->mouse[i] = KeyState();
    this->mouse_x = 0;
    this->mouse_y = 0;
    this->console_focus = true;

    this->title = L"Game Console";

    if (this->console_handle == INVALID_HANDLE_VALUE) {
        this->error(L"Bad Handle");
        return;
    }

    this->window_rectangle = { 0, 0, 1, 1 };
    SetConsoleWindowInfo(this->console_handle, TRUE, &this->window_rectangle);

    COORD coord = { (short)this->width, (short)this->height };
    if (!SetConsoleScreenBufferSize(this->console_handle, coord)) {
        this->error(L"SetConsoleScreenBufferSize failed");
    }
    if (!SetConsoleActiveScreenBuffer(this->console_handle)) {
        this->error(L"SetConsoleActiveScreenBuffer failed");
    }
    
    CONSOLE_FONT_INFOEX console_font_info;
    console_font_info.cbSize = sizeof(console_font_info);
    console_font_info.nFont = 0;
    console_font_info.dwFontSize.X = pixel_size;
    console_font_info.dwFontSize.Y = pixel_size;
    console_font_info.FontFamily = FF_DONTCARE;
    console_font_info.FontWeight = FW_NORMAL;
    wcscpy_s(console_font_info.FaceName, L"Consolas");
    if (!SetCurrentConsoleFontEx(this->console_handle, false, &console_font_info)) {
        this->error(L"SetCurrentConsoleFontEx failed");
    }

    CONSOLE_SCREEN_BUFFER_INFO console_screen_buffer_infor;
    if (!GetConsoleScreenBufferInfo(this->console_handle, &console_screen_buffer_infor)) {
        this->error(L"GetConsoleScreenBufferInfo failed");
    }
    if (this->height > console_screen_buffer_infor.dwMaximumWindowSize.Y) {
        this->error(L"Screen Height / Font Height Too Big");
    }
    if (this->width > console_screen_buffer_infor.dwMaximumWindowSize.X) {
        this->error(L"Screen Width / Font Width Too Big");
    }

    this->window_rectangle = { 0, 0, (short)(this->width - 1), (short)(this->height - 1) };
    if (!SetConsoleWindowInfo(this->console_handle, TRUE, &this->window_rectangle)) {
        this->error(L"SetConsoleWindowInfo failed");
    }        
    if (!SetConsoleMode(this->console_handle, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT)) {
        this->error(L"SetConsoleMode failed");
    }

    HWND console_window = GetConsoleWindow();
    long console_window_style = GetWindowLong(console_window, GWL_STYLE);
    console_window_style ^= WS_SIZEBOX;
    console_window_style ^= WS_MAXIMIZEBOX;
    SetWindowLong(console_window, GWL_STYLE, console_window_style);

    SetConsoleCtrlHandler((PHANDLER_ROUTINE)this->close_handler, TRUE);
}

Game::~Game()
{
    SetConsoleActiveScreenBuffer(this->original_console_handle);
    delete[] this->screen;
};

void Game::start()
{
    Game::atomic_running = true;
    std::thread thread = std::thread(&Game::thread_func, this);
    thread.join();
};

void Game::end()
{
    Game::atomic_running = false;
};

void Game::clear()
{
    memset(this->screen, 0, sizeof(CHAR_INFO) * this->width * this->height);
};

void Game::draw(int x, int y, PixelType pixel, Color color)
{
    if (x >= 0 && x < this->width && y >= 0 && y < this->height)
    {
        this->screen[y * this->width + x].Char.UnicodeChar = pixel;
        this->screen[y * this->width + x].Attributes = color;
    }
};

void Game::draw_rectangle(int x, int y, int w, int h, PixelType pixel, Color color)
{
    x = std::min(x, this->width);
    x = std::max(x, 0);

    y = std::min(y, this->height);
    y = std::max(y, 0);

    w = std::min(w, this->width - x);
    w = std::max(w, 0);

    h = std::min(h, this->height - y);
    h = std::max(h, 0);

    for (int draw_x = x; draw_x < x + w; ++draw_x) {
        for (int draw_y = y; draw_y < y + h; ++draw_y) {
            this->draw(draw_x, draw_y, pixel, color);
        }
    }
};

void Game::draw_text(int x, int y, std::wstring text, Color color)
{
    for (size_t i = 0; i < text.size(); ++i)
    {
        if (y * this->width + x + i < 0 || y * this->width + x + i >= this->width * this->height) continue;
        this->screen[y * this->width + x + i].Char.UnicodeChar = text[i];
        this->screen[y * this->width + x + i].Attributes = color;
    }
};

int Game::get_width()
{
    return this->width;
};

int Game::get_height()
{
    return this->height;
};

KeyState Game::get_key(KeyType key_id)
{
    return this->keys[key_id];
};

int Game::get_mouse_x()
{
    return this->mouse_x;
};

int Game::get_mouse_y()
{
    return this->mouse_y;
};

KeyState Game::get_mouse(MouseType mouse_id)
{
    return this->mouse[mouse_id];
};

bool Game::is_window_focus()
{
    return this->console_focus;
};

void Game::thread_func()
{
    // Load
    this->load();

    // Time point
    auto time_point_1 = std::chrono::system_clock::now();
    auto time_point_2 = std::chrono::system_clock::now();

    // Game loop
    while (Game::atomic_running) {
        // Handle timing
        time_point_2 = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_time = time_point_2 - time_point_1;
        time_point_1 = time_point_2;
        double delta_time = elapsed_time.count();

        // Handle keyboard input
        for (int i = 0; i < 256; i++) {
            this->keyboard_state_new[i] = GetAsyncKeyState(i);

            this->keys[i].pressed = false;
            this->keys[i].released = false;

            if (this->keyboard_state_new[i] != this->keyboard_state_old[i]) {
                if (this->keyboard_state_new[i] & 0x8000) {
                    this->keys[i].pressed = !this->keys[i].held;
                    this->keys[i].held = true;
                }
                else {
                    this->keys[i].released = true;
                    this->keys[i].held = false;
                }
            }

            this->keyboard_state_old[i] = this->keyboard_state_new[i];
        }

        // Handle mouse input
        INPUT_RECORD input_buffer[32];
        DWORD events = 0;
        GetNumberOfConsoleInputEvents(this->console_input_handle, &events);
        if (events > 0) {
            ReadConsoleInput(this->console_input_handle, input_buffer, events, &events);
        }

        // Handle events
        for (DWORD i = 0; i < events; i++) {
            switch (input_buffer[i].EventType) 
            {
            case FOCUS_EVENT:
                this->console_focus = input_buffer[i].Event.FocusEvent.bSetFocus;
                break;
            case MOUSE_EVENT:
                switch (input_buffer[i].Event.MouseEvent.dwEventFlags)
                {
                case MOUSE_MOVED:
                    this->mouse_x = input_buffer[i].Event.MouseEvent.dwMousePosition.X;
                    this->mouse_y = input_buffer[i].Event.MouseEvent.dwMousePosition.Y;
                    break;
                case 0:
                    for (int m = 0; m < 5; m++) {
                        this->mouse_state_new[m] = (input_buffer[i].Event.MouseEvent.dwButtonState & (1 << m)) > 0;
                    }
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
        }

        for (int m = 0; m < 5; ++m) {
            this->mouse[m].pressed = false;
            this->mouse[m].released = false;

            if (this->mouse_state_new[m] != this->mouse_state_old[m]) {
                if (mouse_state_new[m]) {
                    this->mouse[m].pressed = true;
                    this->mouse[m].held = true;
                }
                else {
                    this->mouse[m].released = true;
                    this->mouse[m].held = false;
                }
            }

            this->mouse_state_old[m] = mouse_state_new[m];
        }

        // Update
        this->update(delta_time);

        // Render
        this->render();
        wchar_t s[256];
        swprintf_s(s, 256, L"Console Game Engine - %s - FPS: %3.2f", this->title.c_str(), 1.0f / (float)delta_time);
        // SetConsoleTitle(s);
        this->draw_text(0, 0, std::to_wstring(1.0 / delta_time), ConsoleEngine::COLOR_FG_WHITE);
        WriteConsoleOutput(this->console_handle, this->screen, { (short)this->width, (short)this->height }, { 0, 0 }, &this->window_rectangle);
    }

    // Unload
    this->unload();
    SetConsoleActiveScreenBuffer(this->original_console_handle);
    Game::cv_exit.notify_one();
};

void Game::error(const wchar_t *messages)
{
    wchar_t buffer[256];
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, 256, NULL);
    SetConsoleActiveScreenBuffer(this->original_console_handle);
    wprintf(L"ERROR: %s\n\t%s\n", messages, buffer);
};

BOOL Game::close_handler(DWORD event)
{
    if (event == CTRL_CLOSE_EVENT)
    {
        Game::atomic_running = false;
        std::unique_lock<std::mutex> lk(Game::mutex);
        Game::cv_exit.wait(lk);
    }
    return TRUE;
};

};