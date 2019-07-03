
#include "graphics.hpp"

map<int, string> keyboard_lower = {{KEY_1, "1"},
                                   {KEY_2, "2"},
                                   {KEY_3, "3"},
                                   {KEY_4, "4"},
                                   {KEY_5, "5"},
                                   {KEY_6, "6"},
                                   {KEY_7, "7"},
                                   {KEY_8, "8"},
                                   {KEY_9, "9"},
                                   {KEY_0, "0"},
                                   {KEY_DASH, "-"},
                                   {KEY_EQUAL, "="},
                                   {KEY_DEL, "DEL"},

                                   {KEY_q, "q"},
                                   {KEY_w, "w"},
                                   {KEY_e, "e"},
                                   {KEY_r, "r"},
                                   {KEY_t, "t"},
                                   {KEY_y, "y"},
                                   {KEY_u, "u"},
                                   {KEY_i, "i"},
                                   {KEY_o, "o"},
                                   {KEY_p, "p"},
                                   {KEY_BOX_BRACKET_L, "["},
                                   {KEY_BOX_BRACKET_R, "]"},

                                   {KEY_a, "a"},
                                   {KEY_s, "s"},
                                   {KEY_d, "d"},
                                   {KEY_f, "f"},
                                   {KEY_g, "g"},
                                   {KEY_h, "h"},
                                   {KEY_j, "j"},
                                   {KEY_k, "k"},
                                   {KEY_l, "l"},
                                   {KEY_SEMICOLON, ";"},
                                   {KEY_quotation, "'"},

                                   {KEY_z, "z"},
                                   {KEY_x, "x"},
                                   {KEY_c, "c"},
                                   {KEY_v, "v"},
                                   {KEY_b, "b"},
                                   {KEY_n, "n"},
                                   {KEY_m, "m"},
                                   {KEY_COMMA, ","},
                                   {KEY_PERIOD, "."},
                                   {KEY_SLASH, "/"},
                                   {KEY_SPACE, "SPACE"}};

map<int, string> keyboard_upper = {{KEY_EXCLAMATION, "!"},
                                   {KEY_AT, "@"},
                                   {KEY_POUND, "#"},
                                   {KEY_DOLLAR, "$"},
                                   {KEY_PERCENT, "%"},
                                   {KEY_CARET, "^"},
                                   {KEY_AMP, "&"},
                                   {KEY_ASTERISK, "*"},
                                   {KEY_BRACKET_L, "("},
                                   {KEY_BRACKET_R, ")"},
                                   {KEY_UNDERSCORE, "_"},
                                   {KEY_PLUS, "+"},
                                   {KEY_DEL, "DEL"},

                                   {KEY_Q, "Q"},
                                   {KEY_W, "W"},
                                   {KEY_E, "E"},
                                   {KEY_R, "R"},
                                   {KEY_T, "T"},
                                   {KEY_Y, "Y"},
                                   {KEY_U, "U"},
                                   {KEY_I, "I"},
                                   {KEY_O, "O"},
                                   {KEY_P, "P"},
                                   {KEY_CUR_BRACKET_L, "{"},
                                   {KEY_CUR_BRACKET_R, "}"},

                                   {KEY_A, "A"},
                                   {KEY_S, "S"},
                                   {KEY_D, "D"},
                                   {KEY_F, "F"},
                                   {KEY_G, "G"},
                                   {KEY_H, "H"},
                                   {KEY_J, "J"},
                                   {KEY_K, "K"},
                                   {KEY_L, "L"},
                                   {KEY_COLON, ":"},
                                   {KEY_QUOTATION, "\""},

                                   {KEY_Z, "Z"},
                                   {KEY_X, "X"},
                                   {KEY_C, "C"},
                                   {KEY_V, "V"},
                                   {KEY_B, "B"},
                                   {KEY_N, "N"},
                                   {KEY_M, "M"},
                                   {KEY_LESS, "<"},
                                   {KEY_GREAT, ">"},
                                   {KEY_QUESTION, "?"},
                                   {KEY_SPACE, "SPACE"}};

/*--------------------*/
// curses library initializations
void Graphics::init_curses() {
    initscr();
    // mute echo
    noecho();
    // make cursor invisible
    curs_set(0);
    // start color
    start_color();
}

void Graphics::init_colors() {
    if (not has_colors()) {
        Exit(1, "Your terminal does not support color\n");
    }

    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_CYAN, COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(7, COLOR_WHITE, COLOR_BLACK);
    init_pair(8, COLOR_WHITE, COLOR_BLUE);
    init_pair(9, COLOR_WHITE, COLOR_RED);
    init_pair(10, COLOR_BLACK, COLOR_GREEN);
    init_pair(11, COLOR_BLACK, COLOR_WHITE);
    init_pair(12, COLOR_BLACK, COLOR_YELLOW);
    init_pair(13, COLOR_YELLOW, COLOR_MAGENTA);
    init_pair(14, COLOR_YELLOW, COLOR_BLUE);
    init_pair(15, COLOR_MAGENTA, COLOR_YELLOW);
    init_pair(16, COLOR_BLACK, COLOR_CYAN);
    init_pair(17, COLOR_CYAN, COLOR_BLUE);

    colors = {
        {"red", COLOR_PAIR(1)},           {"green", COLOR_PAIR(2)},
        {"yellow", COLOR_PAIR(3)},        {"blue", COLOR_PAIR(4)},
        {"cyan", COLOR_PAIR(5)},          {"magenta", COLOR_PAIR(6)},
        {"white", COLOR_PAIR(7)},         {"white-blue", COLOR_PAIR(8)},
        {"white-red", COLOR_PAIR(9)},     {"black-green", COLOR_PAIR(10)},
        {"white cursor", COLOR_PAIR(11)}, {"yellow cursor", COLOR_PAIR(12)},
        {"black-yellow", COLOR_PAIR(12)}, {"yellow-magenta", COLOR_PAIR(13)},
        {"yellow-blue", COLOR_PAIR(14)},  {"magenta-yellow", COLOR_PAIR(15)},
        {"black-cyan", COLOR_PAIR(16)},   {"cyan-blue", COLOR_PAIR(17)}};
}

void Graphics::init_size_params(const int& user_defined_w,
                                const int& user_defined_h) {
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    actual_term_w = size.ws_col;
    actual_term_h = size.ws_row;

    if (actual_term_w < term_w_absolutemin or
        actual_term_h < term_h_absolutemin) {
        Exit(1, "Current Terminal size COL x ROW\n(Width x Height): " +
                    to_string(size.ws_col) + "x" + to_string(size.ws_row) +
                    "\nPlease resize Terminal window to\n" +
                    to_string(term_w_absolutemin) + "x" +
                    to_string(term_h_absolutemin) + " or larger.");
    } else if (user_defined_w != 0 and user_defined_h != 0) {
        if (user_defined_w < term_w_absolutemin or
            user_defined_h < term_h_absolutemin) {
            Exit(1,
                 "Typon window can't be\nsmaller than COL x ROW\n(Width x "
                 "Height): " +
                     to_string(term_w_absolutemin) + "x" +
                     to_string(term_h_absolutemin));
        } else if (user_defined_w > actual_term_w or
                   user_defined_h > actual_term_h) {
            Exit(1, "Please resize Terminal window to\n" +
                        to_string(user_defined_w) + "x" +
                        to_string(user_defined_h) + " or larger.");
        } else {
            win_w = user_defined_w - 2;
            win_h = user_defined_h - info_h - menu_h;
        }
    } else {
        auto temp_win_w = actual_term_w - 2;  // leave small spaces on the sides
        auto temp_win_h = actual_term_h - info_h - menu_h;
        win_w = (temp_win_w > win_w_defaultmax) ? win_w_defaultmax : temp_win_w;
        win_h = (temp_win_h > win_h_defaultmax) ? win_h_defaultmax : temp_win_h;
    }

    min_terminal_w = win_w + 2;
    min_terminal_h = win_h + info_h + menu_h;  //+1
}

Graphics::Graphics(const int& w, const int& h) {
    init_size_params(w, h);
}

void Graphics::init_graphics()
// game_engine need to call this at the beginning of its run() function
{
    min_terminal_w = win_w + 2;
    min_terminal_h = win_h + info_h + menu_h;  //+1

    // verify_terminal_size();

    init_curses();
    // first initialize color
    init_colors();

    create_windows();

    /*--------------------*/
    // menu ready
    init_menu();

    // get speical keys
    keypad(inner_text_frame, true);
    keypad(inner_info_frame, true);
    keypad(inner_menu_frame, true);

    /*--------------------------------------------------*/
    // show windows
    refresh_text_display();
    refresh_info_display();
}

void Graphics::create_windows() {
    /*--------------------------------------------------*/
    // window params
    /*--------------------*/
    // window sizes
    inner_win_h = win_h - 2;
    inner_win_w = win_w - 2;
    starty = (LINES - win_h) / 2 - 2;  // slightly above middle
    startx = (COLS - win_w) / 2;

    info_w = win_w;

    inner_info_h = info_h - 2;
    inner_info_w = info_w - 2;

    /*--------------------------------------------------*/
    // creating objects

    /* size.ws_row is the number of rows, size.ws_col is the number of columns.
     */

    /*--------------------*/
    // creating windows
    text_frame = newwin(win_h, win_w, starty, startx);
    inner_text_frame = newwin(inner_win_h, inner_win_w, starty + 1, startx + 1);

    info_frame = newwin(info_h, info_w, starty + win_h, startx);
    inner_info_frame =
        newwin(inner_info_h, inner_info_w, starty + win_h + 1, startx + 1);

    /*--------------------*/
    // paint windows with colors
    wattrset(text_frame, colors[frame_color]);
    wattrset(info_frame, colors[info_color]);

    /*--------------------*/
    // create the outer frame
    // then create the invisible inner frame to restrict text in a smaller
    // region
    box(text_frame, 0, 0);
    box(inner_text_frame, 0, 0);
    wborder(inner_text_frame, ' ', ' ', ' ', ' ', ' ', ' ', ' ',
            ' ');  // make invisible

    // create info box and its invisible inner frame
    box(info_frame, 0, 0);
    box(inner_info_frame, 0, 0);
    wborder(inner_info_frame, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
}
/*--------------------*/
// change color
void Graphics::set_text_color(const string& c) {
    wattrset(inner_text_frame, colors[c]);
}

void Graphics::set_info_color(const string& c) {
    wattrset(inner_info_frame, colors[c]);
}

/*--------------------*/
// refresh
void Graphics::refresh_text_display() {
    wrefresh(text_frame);
    wrefresh(inner_text_frame);
}

void Graphics::refresh_info_display() {
    wrefresh(info_frame);
    wrefresh(inner_info_frame);
}

/*--------------------*/
// add text

void Graphics::text_add_str(const string& str,
                            const string& c,
                            const bool clear_prev,
                            const bool& refresh) {
    if (clear_prev)
        erase_text();

    wattron(inner_text_frame, colors[c]);
    waddstr(inner_text_frame, str.c_str());
    wattroff(inner_text_frame, colors[c]);

    if (refresh)
        refresh_text_display();
}

void Graphics::text_mvadd_str(const int& y,
                              const int& x,
                              const string& str,
                              const string& c,
                              const bool clear_prev,
                              const bool& refresh) {
    if (clear_prev)
        erase_text();

    wattron(inner_text_frame, colors[c]);
    mvwaddstr(inner_text_frame, y, x, str.c_str());
    wattroff(inner_text_frame, colors[c]);

    if (refresh)
        refresh_text_display();
}

void Graphics::text_add_ch(const char& ch,
                           const string& c,
                           const bool clear_prev,
                           const bool& refresh) {
    if (clear_prev)
        erase_text();

    wattron(inner_text_frame, colors[c]);
    waddch(inner_text_frame, ch);
    wattroff(inner_text_frame, colors[c]);

    if (refresh)
        refresh_text_display();
}

/*--------------------*/
// write to info

void Graphics::info_add_str(const string& str,
                            const string& c,
                            const bool& clear_prev,
                            const bool& refresh,
                            const string& sep) {
    if (clear_prev)
        erase_info();

    wattron(inner_info_frame, colors[c]);
    waddstr(inner_info_frame, (sep + str).c_str());
    wattroff(inner_info_frame, colors[c]);

    if (refresh)
        refresh_info_display();
}

void Graphics::info_mvadd_str(const int& y,
                              const int& x,
                              const string& str,
                              const string& c,
                              const bool& clear_prev,
                              const bool& refresh) {
    if (clear_prev)
        erase_info();

    wattron(inner_info_frame, colors[c]);
    mvwaddstr(inner_info_frame, y, x, str.c_str());
    wattroff(inner_info_frame, colors[c]);

    if (refresh)
        refresh_info_display();
}

void Graphics::info_add_ch(const char& ch,
                           const string& c,
                           const bool& clear_prev,
                           const bool& refresh) {
    if (clear_prev)
        erase_info();

    wattron(inner_info_frame, colors[c]);
    waddch(inner_info_frame, ch);
    wattroff(inner_info_frame, colors[c]);

    if (refresh)
        refresh_info_display();
}

void Graphics::info_mvadd_ch(const int& y,
                             const int& x,
                             const char& ch,
                             const string& c,
                             const bool& clear_prev,
                             const bool& refresh) {
    if (clear_prev)
        erase_info();

    wattron(inner_info_frame, colors[c]);
    mvwaddch(inner_info_frame, y, x, ch);
    wattroff(inner_info_frame, colors[c]);

    if (refresh)
        refresh_info_display();
}

/*--------------------*/
// erase text & msg
void Graphics::erase_text(const bool& refresh) {
    // cout << "HERE!! erase1" << endl;//debug
    werase(inner_text_frame);
    // cout << "HERE!! erase2" << endl;//debug
    if (refresh)
        refresh_text_display();
    // cout << "HERE!! erase3" << endl;//debug
}

void Graphics::erase_info(const bool& refresh) {
    werase(inner_info_frame);

    if (refresh)
        refresh_info_display();
}

void Graphics::init_menu() {
    // Should I have different menu for different game modes? (not sure)
    // First try with the easier case (one menu for all game mode)
    // If later special menu is needed to be implemented for different modes
    // simply move this function and it's associates to that game mode
    // then call current_game->open_menu() here.

    // open menu at the right lower corner of the game window
    // initialize menu parameters
    menu_w = win_w;  // magic number (now testing)
    inner_menu_h = menu_h - 2;
    inner_menu_w = menu_w - 2;
    // locate at right lower corner
    menu_starty = starty - menu_h;
    menu_startx = startx;

    menu_frame = newwin(menu_h, menu_w, menu_starty, menu_startx);
    inner_menu_frame =
        newwin(inner_menu_h, inner_menu_w, menu_starty + 1, menu_startx + 1);
}

/*--------------------*/
// print menu and highlight choice
void Graphics::print_menu(const string& type, int highlight) {
    verify_terminal_size();

    vector<string> menu_options;

    if (type == "inGame") {
        menu_options = inGame_menu_options;
    } else if (type == "inHistory") {
        menu_options = inHistory_menu_options;
    } else if (type == "preGame") {
        menu_options = preGame_menu_options;
    } else if (type == "postGame") {
        menu_options = postGame_menu_options;
    } else if (type == "inReplay") {
        menu_options = inReplay_menu_options;
    } else if (type == "inHelp") {
        menu_options = inHelp_menu_options;
    } else {
        throw runtime_error("undefined menu type: " + type);
    }

    wattrset(menu_frame, colors[menu_frame_color]);

    box(menu_frame, 0, 0);
    box(inner_menu_frame, 0, 0);
    wborder(inner_menu_frame, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');

    for (auto i = 0; i < menu_options.size(); ++i) {
        if (highlight == i)
            wattron(inner_menu_frame, A_REVERSE);

        mvwaddstr(inner_menu_frame, 0, i * 12, menu_options[i].c_str());

        if (highlight == i)
            wattroff(inner_menu_frame, A_REVERSE);
    }
    wrefresh(menu_frame);
    wrefresh(inner_menu_frame);
}

void Graphics::erase_menu(const bool& refresh) {
    werase(menu_frame);
    werase(inner_menu_frame);

    if (refresh) {
        wrefresh(menu_frame);
        wrefresh(inner_menu_frame);
    }
}

void Graphics::draw_keyboard(const int& y,
                             const int& x,
                             const int& key,
                             const string& c) {
    vector<vector<int>> kbo;
    map<int, string> kb;

    if (isShifted(key)) {
        kbo = keyboard_order_upper;
        kb = keyboard_upper;
    } else {
        kbo = keyboard_order_lower;
        kb = keyboard_lower;
    }

    for (auto i = 0; i < kbo.size(); ++i) {
        for (auto j = 0; j < kbo[i].size(); ++j) {
            auto k = kbo[i][j];
            if (k == key)
                info_mvadd_str(y + i, x + 2 * j, kb[k], "black-green", false,
                               false);
            else
                info_mvadd_str(y + i, x + 2 * j, kb[k], c, false, false);
        }
        if (i != kbo.size() - 1)
            info_add_ch('\n', "white", false, false);
        else
            refresh_info_display();
    }
}

void Graphics::mvclrtobot(const string& win,
                          const int& y,
                          const int& x,
                          const bool& refresh) {
    int temp_y, temp_x;

    if (win == "text") {
        getyx(inner_text_frame, temp_y, temp_x);
        wmove(inner_text_frame, y, x);
        wclrtobot(inner_text_frame);
        wmove(inner_text_frame, temp_y, temp_x);
        if (refresh)
            refresh_text_display();
    } else if (win == "info") {
        getyx(inner_info_frame, temp_y, temp_x);
        wmove(inner_info_frame, y, x);
        wclrtobot(inner_info_frame);
        wmove(inner_info_frame, temp_y, temp_x);
        if (refresh)
            refresh_info_display();
    } else {
        Exit(1, "Graphics::mvcursor: Can't apply to window named:" + win);
    }
}

void Graphics::verify_terminal_size() {
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    if (actual_term_w != size.ws_col or
        actual_term_h != size.ws_row) {  // if terminal size changed
        // re-evaluate location
        starty = (LINES - win_h) / 2 - 2;
        startx = (COLS - win_w) / 2;

        menu_starty = starty - menu_h;
        menu_startx = startx;

        // copy all windows
        auto temp_tf = dupwin(text_frame);
        auto temp_itf = dupwin(inner_text_frame);
        auto temp_if = dupwin(info_frame);
        auto temp_iif = dupwin(inner_info_frame);
        auto temp_mf = dupwin(menu_frame);
        auto temp_imf = dupwin(inner_menu_frame);
        // erase all windows
        werase(text_frame);
        werase(inner_text_frame);
        werase(info_frame);
        werase(inner_info_frame);
        werase(menu_frame);
        werase(inner_menu_frame);
        // unshow them
        wrefresh(text_frame);
        wrefresh(inner_text_frame);
        wrefresh(info_frame);
        wrefresh(inner_info_frame);
        wrefresh(menu_frame);
        wrefresh(inner_menu_frame);
        // remove them
        delwin(text_frame);
        delwin(inner_text_frame);
        delwin(info_frame);
        delwin(inner_info_frame);
        delwin(menu_frame);
        delwin(inner_menu_frame);
        // move copies
        mvwin(temp_tf, starty, startx);
        mvwin(temp_itf, starty + 1, startx + 1);
        mvwin(temp_if, starty + win_h, startx);
        mvwin(temp_iif, starty + win_h + 1, startx + 1);
        mvwin(temp_mf, menu_starty, menu_startx);
        mvwin(temp_imf, menu_starty + 1, menu_startx + 1);
        // restore
        text_frame = temp_tf;
        inner_text_frame = temp_itf;
        info_frame = temp_if;
        inner_info_frame = temp_iif;
        menu_frame = temp_mf;
        inner_menu_frame = temp_imf;
        // show
        wrefresh(text_frame);
        wrefresh(inner_text_frame);
        wrefresh(info_frame);
        wrefresh(inner_info_frame);
        wrefresh(menu_frame);
        wrefresh(inner_menu_frame);
    }

    actual_term_w = size.ws_col;
    actual_term_h = size.ws_row;

    if ((actual_term_w < min_terminal_w) or (actual_term_h < min_terminal_h)) {
        Exit(1, "Current Terminal size COL x ROW (Width x Height): " +
                    to_string(size.ws_col) + "x" + to_string(size.ws_row) +
                    "\nPlease resize Terminal window to " +
                    to_string(min_terminal_w) + "x" +
                    to_string(min_terminal_h) + " or larger.");
    }
}
