
#ifndef graphics_hpp
#define graphics_hpp

#include "special_keys.cpp"
#include "tools.hpp"

#include <ncurses.h>

#include <sys/ioctl.h>
#include <unistd.h>
#include <map>
#include <vector>

using namespace std;

struct Graphics {
    vector<vector<int>> keyboard_order_lower = {
        {KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0,
         KEY_DASH, KEY_EQUAL, KEY_DEL},
        {KEY_q, KEY_w, KEY_e, KEY_r, KEY_t, KEY_y, KEY_u, KEY_i, KEY_o, KEY_p,
         KEY_BOX_BRACKET_L, KEY_BOX_BRACKET_R},
        {KEY_a, KEY_s, KEY_d, KEY_f, KEY_g, KEY_h, KEY_j, KEY_k, KEY_l,
         KEY_SEMICOLON, KEY_quotation},
        {KEY_z, KEY_x, KEY_c, KEY_v, KEY_b, KEY_n, KEY_m, KEY_COMMA, KEY_PERIOD,
         KEY_SLASH, KEY_SPACE}};

    vector<vector<int>> keyboard_order_upper = {
        {KEY_EXCLAMATION, KEY_AT, KEY_POUND, KEY_DOLLAR, KEY_PERCENT, KEY_CARET,
         KEY_AMP, KEY_ASTERISK, KEY_BRACKET_L, KEY_BRACKET_R, KEY_UNDERSCORE,
         KEY_PLUS, KEY_DEL},
        {KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P,
         KEY_CUR_BRACKET_L, KEY_CUR_BRACKET_R},
        {KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L,
         KEY_COLON, KEY_QUOTATION},
        {KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M, KEY_LESS, KEY_GREAT,
         KEY_QUESTION, KEY_SPACE}};

    void init_curses();
    void init_colors();
    bool support_change_color = true;
    struct winsize size;
    /*--------------------*/
    // text window graphic components
    WINDOW* text_frame;
    WINDOW* inner_text_frame;

    /*--------------------*/
    // info window graphic components
    WINDOW* info_frame;
    WINDOW* inner_info_frame;

    /*--------------------*/
    // menu graphic components
    WINDOW* menu_frame;
    WINDOW* inner_menu_frame;

    /*--------------------*/
    // text window sizes params

    // default window adjustment won't go larger than this
    const int win_w_defaultmax = 110;
    const int win_h_defaultmax = 110;

    // all other params can be deduced from these 4 values
    int win_w = 55;
    int win_h = 10;

    const int info_h = 7;
    const int menu_h = 3;

    // terminal absolutely shouldn't be smaller than this
    const int term_w_absolutemin = 55;
    const int term_h_absolutemin = 10 + info_h + menu_h;

    int inner_win_h;
    int inner_win_w;

    unsigned int starty;
    unsigned int startx;

    // info win sizes

    int info_w;

    int inner_info_h;
    int inner_info_w;

    /*--------------------*/
    // text window info colors
    string frame_color = "white";
    string info_color = "white";

    /*--------------------*/
    // menu size, position, color

    int menu_w;
    int inner_menu_h;
    int inner_menu_w;
    int menu_startx, menu_starty;
    string menu_frame_color = "white";
    string menu_text_color = "white";

    // container for color settings
    map<string, int> colors{};

    void init_size_params(const int& w, const int& h);

    void init_graphics();

    void create_windows();

    // change color
    void set_text_color(const string& c);
    void set_info_color(const string& c);

    /*--------------------*/
    // refresh & display
    void refresh_text_display();
    void refresh_info_display();

    /*--------------------*/
    // write text
    //    void add_msg(const vector<string>& contents, const vector<string>
    //    color, const string& win, const bool& clear_prev=true, const bool&
    //    refresh=true);

    void text_add_str(const string& str,
                      const string& c,
                      const bool clear_prev = false,
                      const bool& refresh = false);

    void text_mvadd_str(const int& y,
                        const int& x,
                        const string& str,
                        const string& c,
                        const bool clear_prev = false,
                        const bool& refresh = false);

    void text_add_ch(const char& ch,
                     const string& c,
                     const bool clear_prev = false,
                     const bool& refresh = false);

    /*--------------------*/
    // write msg
    void info_add_str(const string& str,
                      const string& c,
                      const bool& clear_prev = false,
                      const bool& refresh = true,
                      const string& sep = "");

    void info_mvadd_str(const int& y,
                        const int& x,
                        const string& str,
                        const string& c,
                        const bool& clear_prev = false,
                        const bool& refresh = true);

    void info_add_ch(const char& ch,
                     const string& c,
                     const bool& clear_prev = false,
                     const bool& refresh = true);

    void info_mvadd_ch(const int& y,
                       const int& x,
                       const char& ch,
                       const string& c,
                       const bool& clear_prev = false,
                       const bool& refresh = true);

    /*--------------------*/
    // erase text & msg
    void erase_text(const bool& refresh = false);

    void erase_info(const bool& refresh = false);

    /*--------------------*/
    // calculate menu size and position
    void init_menu();

    /*--------------------*/
    // options to be displayed on menu
    vector<string> preGame_menu_options{"Next", "History", "Quit", "Help"};
    vector<string> postGame_menu_options{"Restart", "Next", "History", "Quit",
                                         "Help"};
    vector<string> inGame_menu_options{"Restart", "Next", "Quit"};
    vector<string> inHistory_menu_options{"Return", "Quit"};
    vector<string> inReplay_menu_options{"SkipToEnd", "Quit"};
    vector<string> inHelp_menu_options{"Return", "Quit"};

    /*--------------------*/
    // print menu and highlight choice
    void print_menu(const string& type, int highlight);

    void erase_menu(const bool& refresh = true);

    /*--------------------*/
    // replay utilites
    void draw_keyboard(const int& y,
                       const int& x,
                       const int& key,
                       const string& c);

    void mvclrtobot(const string& win,
                    const int& y,
                    const int& x,
                    const bool& refresh = false);

    int actual_term_w;
    int actual_term_h;
    int min_terminal_w;
    int min_terminal_h;
    void verify_terminal_size();

    Graphics(const int& user_defined_w, const int& user_defined_h);
};

#endif
