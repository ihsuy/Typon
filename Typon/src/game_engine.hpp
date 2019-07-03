#ifndef game_engine_hpp
#define game_engine_hpp

#include "game_mode.hpp"
#include "graphics.hpp"
#include "quote_loader.hpp"
#include "save_handler.hpp"
#include "special_keys.cpp"
#include "timer.hpp"
#include "tools.hpp"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <random>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;

/*
game_engine holds all logic and graphic components
necessary for Typon to run.
The engine uses mostly game_mode(logic), timer, graphics(painter),
save_handler and quote_loader to perform variety of tasks.

Example:
    game_engine engine(id_max, quote_dir, w, h);
    engine.run();
*/
class game_engine {
    // guidiance messages that are meant to be
    // displayed in the Info Window
    const string intro_bottom_msg =
        "Menu: tab | goto: hyphen | Challenge mode: 0 | Remove record: delete";
    const string inGame_bottom_msg = "Menu: tab";
    const string outro_bottom_msg = "Menu: tab | Replay: r";
    const string inReplay_bottom_msg = "Menu: tab";
    const string inMenu_bottom_msg =
        "Close Menu: tab | LEFT/RIGHT: a/d or arrow keys | Choose: return";
    const string inHistory_bottom_msg =
        "Menu: tab | Navigate: 4 arrow keys | Sort: return | goto: space | "
        "Page "
        "Up/Down: w/s | Stats: 0";
    const string skipToQuote_bottom_msg = "Return: hyphen";
    const string removeRecord_bottom_msg = "Confirm: y | Cancel: non-y";
    const string inHelp_bottom_msg = "Menu: tab";

    // Other messages
    const string game_intro_msg = "Press first letter or SPACE to begin";
    const string game_outro_msg = "Press r for replay, press SPACE to continue";

    // history column names and default ranking orders
    const vector<string> header_choices{"ID", "Gross WPM", "Mistakes", "Date"};
    unordered_map<string, bool> historyHeader_isAscend{{"id", true},
                                                       {"wpm", false},
                                                       {"mistake", true},
                                                       {"date", true}};

    // color properties of messages
    const string intro_msg_color = "yellow";
    const string intro_warning_color = "black-yellow";
    const string outro_msg_color = "yellow";
    const string challenge_warning_color = "magenta";
    const string replay_over_msg_color = "yellow";
    const string now_replaying_color = "cyan";
    const string replay_keyboard_color = "white";
    const string inHistory_header_color = "yellow";
    const string inHistory_header_highlight_color = "yellow-blue";
    const string inHistory_record_color = "cyan";
    const string inHistory_record_highlight_color = "black-yellow";
    const string inHistory_quoteGlance_color = "cyan";
    const string inHistory_pageNum_color = "yellow";
    const string inHistory_skipToQuote_msg_color = "black-yellow";
    const string stats_msg_color = "black-yellow";
    const string skipToQuote_msg_color = "yellow";
    const string skipToQuote_warning_color = "yellow";
    const string remove_record_msg_color = "black-yellow";
    const string help_n_tips_color = "white";

    // demo quote is a toy quote to play with in "demo mode"
    // NOTE: demo mode is activated only when there's no quotes is detected
    const string demo_quote =
        "There are no quotes in your \"quotes\" directory. Please add some "
        "quotes or correct the path to your quotes by using the \"-path\" "
        "flag.";

    // Lag term for high resolution sleep
    // this term is crucial for game replay and challenge
    // mode accuracy
    // NOTE: Could be System Dependent
    const chrono::nanoseconds avg_overslept = chrono::nanoseconds(3480000);

    // time lag for skipToEnd option in "replay menu", the smaller
    // the faster
    const double replay_skipToEnd_speed = 0.002;

    // minimum letter count, the minimum number of letters in a quote
    // NOTE: Should never be less than 2
    const int min_lc = 2;

    // column width parameters for nondate and date items in "History"
    // max_nondate_w prevent nondate items' width from exceeding 30
    // even when terminal is large enough.
    // fixed_date_w controls date item's width and its fixed
    const int max_nondate_w = 30;
    const int fixed_date_w = 23;

    // length of quote to show in Info Window when browsing records in History
    // the value is decided base on the terminal size
    int inHistory_quoteGlancee_len;

    // maximum id that Typon will detect, in other words, Typon will only look
    // for quotes with ID in the range [0, ID_MAX] inclusively
    const int ID_MAX;

    // container to allow temporary challenge mode in demo mode
    // no records are saved under demo mode
    pair<double, pair<vector<long long>, vector<int>>> demo_best_record;

    // path to quote files and save files (MUST end with'/')
    int quoteDatabase_size;

    vector<int> quote_order;
    int quoteOrder_index = -1;

    string current_quote;
    unsigned long current_lc{};

    vector<int> all_inputs;

    string mode;
    pair<double, pair<vector<long long>, vector<int>>> best_record;

    // used to interrupt oppoenent thread in challenge mode
    bool current_game_terminated = false;

    bool intro_info_wait = false;
    bool skip_game = false;
    bool restart_game = false;
    bool replaying = false;
    bool skip_replay = false;
    bool inMenu = false;
    bool inHistory = false;
    bool inHelp = false;

    int current_text_input{};
    int current_menu_input{};
    int current_info_input{};

    int menuChoice;
    long long nOption_menu_inGame;
    long long nOption_menu_inHistory;
    long long nOption_menu_preGame;
    long long nOption_menu_postGame;
    long long nOption_menu_inReplay;

    int max_hdisplay_nline;
    int hdisplay_nline;
    int gsave_last_page_len;
    long long gsave_page_maxnum;
    long long gsave_page_num;
    vector<int> inHistory_header_spacing;

    int max_text_nChar;

    int highlighted_quote_id;
    string history_order;
    string highlighted_quote_path;

    bool verticle_move = true;
    int header_choice;
    int record_choice;

    bool invalid_quote = false;
    bool isDemo = false;

    // Essential engine helpers

    // Graphic backend
    // all graphic display tasks delegated to it
    Graphics G;

    // Game logic
    // calculates and controls logical behaviors of Typon
    Game_mode current_game;

    // Timer
    // keep track of time and stores them
    timer inner_stopwatch;

    // Save file handler
    // saves, loads, and overwrite records and more
    save_handler saver;

    // Quote loader
    // loads quotes and does some minor fix on quote formats
    quote_loader q_loader;

    // Engine utilities

    void load_graphic_properties();

    // reorder quote_order randomly
    void shuffle_quotes(vector<int>& quote_order);

    // try to load quote using quoteOrder_index
    void load_current_quote();

    // load quote using target id
    void next_quote(const int& tar_id = -1);

    void game_intro(const string& msg);
    void init_mode(const string& msg);

    void game_outro(const string& msg);

    void game_begin();
    void game_end();

    void timing_begin();
    void timing_end();

    void finalize();

    int get_input();
    void inGame_get_input();
    void inMenu_get_input();
    void inInfo_get_input();

    void inGame_loop();
    void Classic_inGame_loop();
    void progress();
    void Challenge_progress();
    void Challenge_inGame_loop();
    void opponents_play();

    void inGame_controller();
    void Challenge_inGame_controller();
    void finish_screen_controller();
    void inHistory_controller(long long& start_i, long long& end_i);
    void replay_controller();

    void open_menu(const string& menu_type, void (game_engine::*controller)());

    void inMenu_controller_inGame();
    void inMenu_controller_inHistory();
    void inMenu_controller_introscr();
    void inMenu_controller_finishscr();
    void inMenu_controller_inReplay();
    void inMenu_controller_inHelp();

    void menuChoiceMove(int val, const long long& menu_size);

    void cExit(int n = 0, const string& msg = "");
    void cRestart();
    void cSkip();
    void replaySkip();
    void skipToQuote();
    void remove_best_record(
        const int& ask_again = 0,
        const string& msg = "Remove record for this quote?");
    void show_stats();
    void quit_history();
    void quit_help();

    void help();

    void show_bottom_msg(const string& msg,
                         const bool& show_id = true,
                         const int& nspilt_bar = 3);
    void erase_bottom_msg(const bool& refresh = false, const int& nline = 2);

    void replay();

    void close_menu();

    void show_History();

    void print_header(const int& highlight);
    void print_record(const int& highlight, vector<string>& history_rec);

    void inHistory_headerChoiceMove(const int& val);
    void inHistory_recordChoiceMove(const int& val,
                                    long long& start_i,
                                    long long& end_i);

    void sort_history(const string& item);
    void reset_history_order_except(const string& item);
    void inHistory_skipToQuote();

    void inHistory_pageUp(long long& start_i, long long& end_i);
    void inHistory_pageDown(long long& start_i, long long& end_i);

    void update_inGame_WPM();

   public:
    // Default initializer
    game_engine(const int& id_max,
                const string& user_defined_quote_dir = ".",
                const int& user_defined_w = 0,
                const int& user_defined_h = 0,
                const bool& shuffle = true);

    // Start engine
    void run();
};

#endif /* game_engine_hpp */
