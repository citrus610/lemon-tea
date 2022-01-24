#pragma once
#pragma comment(lib, "winmm.lib")

#ifndef UNICODE
#error Enable UNICODE for the compiler!
#endif

#include <windows.h>

#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace ConsoleEngine
{

enum Color
{
    COLOR_FG_BLACK = 0x0000,
    COLOR_FG_DARK_BLUE = 0x0001,    
    COLOR_FG_DARK_GREEN = 0x0002,
    COLOR_FG_DARK_CYAN = 0x0003,
    COLOR_FG_DARK_RED = 0x0004,
    COLOR_FG_DARK_MAGENTA = 0x0005,
    COLOR_FG_DARK_YELLOW = 0x0006,
    COLOR_FG_GREY = 0x0007,
    COLOR_FG_DARK_GREY = 0x0008,
    COLOR_FG_BLUE = 0x0009,
    COLOR_FG_GREEN = 0x000A,
    COLOR_FG_CYAN = 0x000B,
    COLOR_FG_RED = 0x000C,
    COLOR_FG_MAGENTA = 0x000D,
    COLOR_FG_YELLOW = 0x000E,
    COLOR_FG_WHITE = 0x000F,
    COLOR_BG_BLACK = 0x0000,
    COLOR_BG_DARK_BLUE = 0x0010,
    COLOR_BG_DARK_GREEN = 0x0020,
    COLOR_BG_DARK_CYAN = 0x0030,
    COLOR_BG_DARK_RED = 0x0040,
    COLOR_BG_DARK_MAGENTA = 0x0050,
    COLOR_BG_DARK_YELLOW = 0x0060,
    COLOR_BG_GREY = 0x0070,
    COLOR_BG_DARK_GREY = 0x0080,
    COLOR_BG_BLUE = 0x0090,
    COLOR_BG_GREEN = 0x00A0,
    COLOR_BG_CYAN = 0x00B0,
    COLOR_BG_RED = 0x00C0,
    COLOR_BG_MAGENTA = 0x00D0,
    COLOR_BG_YELLOW = 0x00E0,
    COLOR_BG_WHITE = 0x00F0,
};

enum PixelType
{
    PIXEL_SOLID = 0x2588,
    PIXEL_THREEQUARTERS = 0x2593,
    PIXEL_HALF = 0x2592,
    PIXEL_QUARTER = 0x2591,
};

enum KeyType
{
    KEY_NONE = 0,
    KEY_A = 1, 
    KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, 
    KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, 
    KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, 
    KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, 
    KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
    KEY_UP = 49, 
    KEY_DOWN, KEY_LEFT, KEY_RIGHT,
    KEY_SPACE = 53,
    KEY_BACK = 63, 
    KEY_ESCAPE = 64,
    KEY_ENTER = 66,
};

enum MouseType
{
    MOUSE_LEFT,
    MOUSE_RIGHT,
    MOUSE_MIDDLE
};

struct KeyState
{
    bool pressed = false;
    bool released = false;
    bool held = false;
};

class Game
{
protected:
    int width;
    int height;
    CHAR_INFO *screen;
    std::wstring title;
    HANDLE original_console_handle;
    CONSOLE_SCREEN_BUFFER_INFO original_console_info;
    HANDLE console_handle;
    HANDLE console_input_handle;
    SMALL_RECT window_rectangle;
protected:
    short keyboard_state_old[256];
    short keyboard_state_new[256];
    bool mouse_state_old[5];
    bool mouse_state_new[5];
    bool console_focus = true;
    KeyState keys[256];
    KeyState mouse[5];
    int mouse_x;
    int mouse_y;
protected:
    static std::atomic<bool> atomic_running;
    static std::condition_variable cv_exit;
    static std::mutex mutex;
public:
    Game(int w, int h, int pixel_size);
    ~Game();
public:
    void start();
    void end();
public:
    virtual void load() {};
    virtual void update(double dt) {};
    virtual void render() {};
    virtual void unload() {};
public:
    void clear();
    void draw(int x, int y, PixelType pixel, Color color);
    void draw_rectangle(int x, int y, int w, int h, PixelType pixel, Color color);
    void draw_text(int x, int y, std::wstring text, Color color);
public:
    int get_width();
    int get_height();
public:
    KeyState get_key(KeyType key_id);
    int get_mouse_x();
    int get_mouse_y();
    KeyState get_mouse(MouseType mouse_id);
    bool is_window_focus();
private:
    void thread_func();
protected:
    void error(const wchar_t *messages);
    static BOOL close_handler(DWORD event);
};

}; // Console Engine namespace