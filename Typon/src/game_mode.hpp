#ifndef game_mode_hpp
#define game_mode_hpp

#include "graphics.hpp"
#include "tools.hpp"

#include <exception>
#include <map>
#include <mutex>
#include <string>
#include <vector>

using namespace std;

struct Game_mode {
    mutex locker;

    int opponents_len = 0;
    /*--------------------------------------------------*/
    // class properties

    /*--------------------*/
    // quote information
    string current_quote;
    string current_quote_for_display;

    int char_per_row;

    unsigned long current_lc;
    double best_WPM;

    /*--------------------*/
    // prepare a vector object to record all input chars except those were
    // correct but deleted and retyped mark correct chars as 0 incorrect chars
    // as 1
    vector<pair<int, int>> total_input;
    vector<pair<int, int>> get_total_input();

    /*--------------------------------------------------*/
    // common tools
    void load_game_info(const string& c_quote,
                        const unsigned long& c_lc,
                        const double& best_wpm,
                        const int& win_w,
                        const bool& do_prettify = true);

    void quote_prettify();

    char key_replace(char in, char tar, char sub);

    /*--------------------------------------------------*/
    // initializers
    Game_mode(const string& mode_name);

    /*--------------------------------------------------*/
    // post-game calculations and properties
    double time_elapsed{};

    double calculate_gross_WPM(const double& t, const unsigned long& lc);
    double calculate_net_WPM(const double& t,
                             const unsigned long& lc,
                             const unsigned int& miss_count);

    /*--------------------------------------------------*/
    // game results
    double gross_wpm{};
    double net_wpm{};
    vector<int> speed_indicator;
    const int speed_normal_range = 5;
    const int speed_indicator_step = 5;

    /*--------------------------------------------------*/
    // class properties

    /*--------------------*/
    // strings keep track of progress in game
    long long correct_part_len = 0;
    long long incorrect_part_len = 0;

    long long typed_length = 0;
    /*--------------------*/
    // track number of mistakes
    unsigned int mistake_count{};

    /*--------------------*/
    // color to distinguish between correct and wrong texts and the part left
    string correct_color = "green";
    string nocolor_replay_correct_color = "green";
    string nocolor_replay_incorrect_color = "white-red";
    string nocolor_replay_cursor_color = "black-green";
    string incorrect_color = "white-red";
    string untyped_color = "white";

    string opponent_color = "cyan-blue";
    string cursor_color = "white cursor";

    /*--------------------*/
    // color settings for special cases
    string intro_screen_text_color = "cyan";
    string finish_screen_correct_color = "green";
    string finish_screen_incorrect_color = "magenta";

    /*--------------------------------------------------*/
    // game logics and design

    /*--------------------*/
    // calculate text and display them
    void update_text_display(Graphics& g);
    void update_text_display_classic(Graphics& g);
    // update logic for mock process for replay
    void update_mock_display(Graphics& g);

    /*--------------------*/
    // design of before-game screen
    void intro_screen(Graphics& g);

    /*--------------------*/
    // design of after-game screen
    void finish_screen(Graphics& g);
    void display_total_input(Graphics& g);
    void calculate_performance();
    void display_performance(Graphics& g);

    /*--------------------*/
    // test if game is completed
    bool game_complete();

    /*--------------------*/
    // reset class properties to default
    void reset_game();
    void clear_text_status();

    /*--------------------------------------------------*/
    // default initializer
    Game_mode(){};

    /*--------------------------------------------------*/
    // core game logic
    void process_challenge(Graphics& g, int& ch);
    void process_classic(Graphics& g, int& ch);

    void mock_process(Graphics& g, int& ch, const double& current_wpm);

    void mock_process_nocolor(Graphics& g, int& ch);

    void update_mock_display_nocolor(Graphics& g);
};

#endif
