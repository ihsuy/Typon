#include "game_engine.hpp"

using namespace std;

game_engine::game_engine(const int& id_max,
                         const string& user_defined_quote_dir,
                         const int& user_defined_w,
                         const int& user_defined_h,
                         const bool& shuffle)
    : ID_MAX(id_max),
      G(user_defined_w, user_defined_h),
      saver(user_defined_quote_dir),
      q_loader(id_max, user_defined_quote_dir) {
    q_loader.load_quoteDatabase_info(quote_order, quoteDatabase_size, isDemo);

    if (not isDemo) {
        saver.prepare_to_save(quote_order);
    } else {
        assert(demo_quote.size() > min_lc);
    }

    if (shuffle) {
        shuffle_quotes(quote_order);
    }
}

void game_engine::run() {
    G.init_graphics();
    load_graphic_properties();
    while (true) {
        next_quote();
        game_intro(game_intro_msg);
        if (not skip_game) {
            game_begin();
            inGame_loop();
            game_end();
            game_outro(game_outro_msg);
        }
        finalize();
    }
}

void game_engine::load_graphic_properties() {
    nOption_menu_inGame = G.inGame_menu_options.size();
    nOption_menu_inHistory = G.inHistory_menu_options.size();
    nOption_menu_preGame = G.preGame_menu_options.size();
    nOption_menu_postGame = G.postGame_menu_options.size();
    nOption_menu_inReplay = G.inReplay_menu_options.size();

    max_hdisplay_nline = G.inner_win_h - 1;

    max_text_nChar = (G.inner_win_w - 2) * G.inner_win_h;

    auto temp_nondate_w = ((G.inner_win_w - fixed_date_w) / 3) - 1;
    int id_w =
        ((temp_nondate_w > max_nondate_w) ? max_nondate_w : temp_nondate_w) - 2;
    int wpm_w = id_w + 4;
    int mis_w = id_w + 3;
    inHistory_header_spacing = {id_w, wpm_w, mis_w, fixed_date_w};

    inHistory_quoteGlancee_len = G.inner_info_w * 2 / 3;
}

void game_engine::next_quote(const int& tar_id) {
    // if tar_id has default val -1 it is loaded in order
    // otherwise next_quote is loaded using tar_id in function args
    if (tar_id < -1) {
        Exit(1, ("game_engine::next_quote: invalid tar_id: " +
                 to_string(tar_id) + " tar_id can't be lesser than -1."));
    }

    // if not restarted, new quote id needs to be generated
    if (not restart_game) {
        // if tar_id is specifically set,
        // search in quote_order, and load it if found
        // otherwise, load quotes following quote_order
        if (tar_id != -1) {
            auto tar = find(quote_order.begin(), quote_order.end(), tar_id);
            if (tar == quote_order.end()) {
                Exit(1, "bad tar_id: Can't find quote" + to_string(tar_id));
            }
            quoteOrder_index = (int)distance(quote_order.begin(), tar);
        } else {
            quoteOrder_index = (quoteOrder_index + 1) % quoteDatabase_size;
        }
    }

    load_current_quote();

    if (not isDemo) {
        best_record = saver.pload(quote_order[quoteOrder_index]);
    } else {
        best_record = demo_best_record;
    }

    // hand quote info to game logic for behavior calculation
    current_game.load_game_info(current_quote, current_lc, best_record.first,
                                G.inner_win_w);
    // restart_game should be set to false everytime a game restarted
    restart_game = false;
}

void game_engine::shuffle_quotes(vector<int>& quote_order) {
    auto seed = (int)chrono::system_clock::now().time_since_epoch().count();
    // shuffle the orders using the seed generated using system cycle count
    shuffle(quote_order.begin(), quote_order.end(),
            (default_random_engine(seed)));
}

void game_engine::load_current_quote() {
    if (not isDemo) {
        current_quote = q_loader.load_quote(quote_order[quoteOrder_index]);
    } else {
        current_quote = demo_quote;
    }

    current_lc = current_quote.length();

    // before proceeding, make sure if quote is not empty,
    // not too short or too long
    if (current_lc == 0) {
        G.info_mvadd_str(0, 0,
                         "Quote" + to_string(quote_order[quoteOrder_index]) +
                             " not found or is empty.\n",
                         intro_warning_color, true, true);
    } else if (current_lc <= min_lc) {
        G.info_mvadd_str(0, 0,
                         "Quote(" + to_string(quote_order[quoteOrder_index]) +
                             ") is too short. (has to be at least " +
                             to_string(min_lc) + " letters)\n",
                         intro_warning_color, true, true);
    } else if (current_lc >= max_text_nChar) {
        G.info_mvadd_str(
            0, 0,
            "Quote_id: " + to_string(quote_order[quoteOrder_index]) +
                " is too long.\n",
            intro_warning_color, true, true);
    } else {
        // good to go
        return;
    }

    invalid_quote = true;
    intro_info_wait = true;
}

void game_engine::game_intro(const string& msg) {
    // skip_game need to be reset before game_intro
    mode = "Classic";
    while (not skip_game) {
        init_mode(msg);

        current_game.intro_screen(G);

        show_bottom_msg(intro_bottom_msg);

        inGame_get_input();

        if (invalid_quote) {
            cSkip();
        }

        // first letter of current quote -> start game
        if (current_text_input == current_quote[0]) {
            inner_stopwatch.start();
            progress();
            return;
        }

        switch (current_text_input) {
            // KEY_SPACE -> start game
            case KEY_SPACE:
                return;
            // KEY_0 -> try to enter/exit challenge mode
            case KEY_0:
                mode = (mode == "Classic") ? "Challenge" : "Classic";
                break;
            // delete keys -> remove performance record for current quote
            case KEY_DEL:
            case KEY_BACKSPACE:
            case '\b':
                remove_best_record(3);
                break;
            // KEY_DASH -> go to
            case KEY_DASH:
                skipToQuote();
                break;
            // KEY_MENU -> open menu
            case KEY_MENU:
                open_menu("preGame", &game_engine::inMenu_controller_introscr);
                break;
            default:
                break;
        }
    }
}

void game_engine::init_mode(const string& msg) {
    // 1. check if record exists for this quote ID
    // 2. if it exists, check if time record and keyinput record have the same
    // size
    //    if not ture, user may have accidentally damaged psave.txt
    // 3. next check if record keyinput's size matches current_quote's letter
    // count.
    //    if not ture, the quote file for this ID may have been modified
    if (mode == "Challenge") {  // check if record exists
        if (best_record.first <= double(0)) {
            G.info_add_str("No record available\n", challenge_warning_color,
                           true, false);
            intro_info_wait = true;
            mode = "Classic";  // do not start challenge mode
        } else {  // check if input size matches the time record size
            if (best_record.second.first.size() !=
                best_record.second.second.size()) {
                remove_best_record(
                    1, "Record for this quote(ID: " +
                           to_string(quote_order[quoteOrder_index]) +
                           ") is modified (psave time record and input "
                           "size don't match)\n" +
                           "Do you want to remove this record?");
                mode = "Classic";  // do not start challenge mode
            } else {               // is this the correct record for this quote?
                long long test_len = 0;
                for (auto item : best_record.second.second) {
                    if (item != KEY_DEL and item != KEY_BACKSPACE and
                        item != '\b') {
                        test_len++;
                    } else {
                        test_len--;
                    }
                }

                if (test_len != current_lc) {
                    remove_best_record(
                        1, "Record for this quote(ID: " +
                               to_string(quote_order[quoteOrder_index]) +
                               ") is doesn't match the quote\n" +
                               "Do you want to remove this record?");
                    mode = "Classic";  // do not start challenge mode
                } else {               // Ok, safe to activated Challenge mode
                    G.info_add_str("Challange Mode Activated",
                                   challenge_warning_color, true, false);
                    G.info_add_str(
                        "Best Gross WPM: " + d_to_string(best_record.first, 2),
                        challenge_warning_color, false, false, "\n");
                    G.info_add_str(msg + " (Cancel: 0)\n", intro_msg_color,
                                   false, true, "\n");
                }
            }
        }
    }

    if (mode == "Classic") {
        bool clear_prev = true, refresh_info = false;
        if (intro_info_wait) {  // don't clear previous since there's extra
                                // message
            //[TODO] very inefficient work around with a boolean, improvement
            //needed
            clear_prev = false;
            intro_info_wait = false;
        }
        G.info_add_str(msg, intro_msg_color, clear_prev, refresh_info);
    }
}

void game_engine::game_begin() {
    G.erase_info();
    current_game.update_text_display(G);
    timing_begin();
}

void game_engine::timing_begin() {
    inner_stopwatch.start();
}

void game_engine::timing_end() {
    inner_stopwatch.stop();
    current_game.time_elapsed = inner_stopwatch.total_duration_in_sec();
}

void game_engine::inGame_loop() {
    // current version only have 2 different modes
    show_bottom_msg(inGame_bottom_msg);

    if (mode == "Classic") {
        Classic_inGame_loop();
    } else if (mode == "Challenge") {
        Challenge_inGame_loop();
    } else {
        Exit(1, "Undefined mode: " + mode);
    }
}

void game_engine::Classic_inGame_loop() {
    while (not current_game.game_complete() and not skip_game) {
        inGame_controller();
        // [deprecated] feature:
        // show current WPM while playing, distracting and slow
        // update_inGame_WPM();
        // G.draw_keyboard(0, G.info_win_w/2-15, current_text_input, "white");
    }
}

void game_engine::Challenge_inGame_loop() {
    // start the opponent thread
    thread opponents_thread(&game_engine::opponents_play, this);
    current_game_terminated = false;
    while (not current_game.game_complete() and not skip_game) {
        Challenge_inGame_controller();
    }
    current_game_terminated = true;
    opponents_thread.join();
}

void game_engine::opponents_play() {
    vector<long long>& best_rec = best_record.second.first;
    vector<int>& best_inp = best_record.second.second;

    for (auto i = 0; i < best_inp.size(); ++i) {
        if (current_game_terminated) {
            // early stop
            timing_end();
            // substract last waiting time from time_elapsed since it was early
            // stopped
            current_game.time_elapsed -= (best_rec[i - 1]) / 1000.0;
            return;
        }
        this_thread::sleep_for(chrono::milliseconds(best_rec[i]) -
                               avg_overslept);
        if (best_inp[i] == KEY_DEL or best_inp[i] == KEY_BACKSPACE or
            best_inp[i] == '\b') {
            current_game.opponents_len--;
        } else {
            current_game.opponents_len++;
        }
        current_game.update_text_display(G);
    }
}

void game_engine::game_end() {
    timing_end();

    if (not skip_game) {
        // calculate and save performance only if game is properly completed
        current_game.calculate_performance();

        if (isDemo) {
            // do not save in demo mode
            if (current_game.gross_wpm > demo_best_record.first) {
                auto demo_time_rec = inner_stopwatch.durations_in_ms();
                demo_time_rec.pop_back();
                demo_best_record =
                    make_pair(current_game.gross_wpm,
                              make_pair(demo_time_rec, all_inputs));
            }
        } else {  // if not in demo mode
            // save to save files
            saver.save(quote_order[quoteOrder_index], current_game.gross_wpm,
                       current_game.mistake_count,
                       inner_stopwatch.durations_in_ms(), all_inputs);
        }
    }
}

void game_engine::game_outro(const string& msg) {
    while (not skip_game) {
        current_game.finish_screen(G);

        auto info_clear_prev = false, refresh_info = false;

        G.info_mvadd_str(G.inner_info_h - 2, 0, msg, outro_msg_color,
                         info_clear_prev, refresh_info);

        // bottom msg be careful don't be too long here
        // since outro msg printed on the same line
        show_bottom_msg(outro_bottom_msg);

        // controller may modify skip_game to get out this loop
        finish_screen_controller();
    }
}

void game_engine::finalize() {
    inner_stopwatch.reset();
    current_game.reset_game();
    all_inputs = vector<int>();
    skip_game = false;
    invalid_quote = false;
    intro_info_wait = false;
    mode = "Classic";
}

/*--------------------------------------------------*/
// class utilites
/*--------------------*/
// get text input
int game_engine::get_input() {
    return wgetch(G.inner_text_frame);
}

void game_engine::inGame_get_input() {  // ignore all characters except ascii
    // SPACE(32)~(127) and KEY_MENU

    // This part should be modified if user's
    // input uses significantly different ascii chars
    current_text_input = get_input();

    if (current_text_input == KEY_RETURN) {
        current_text_input = KEY_SPACE;
    }

    if ((current_text_input < KEY_SPACE or current_text_input > 127) and
        current_text_input != KEY_MENU and
        current_text_input != KEY_BACKSPACE and current_text_input != '\b') {
        current_text_input = KEY_NULL;
    }

    //    do
    //    {
    //        current_text_input = get_input();
    //    }
    //    while((current_text_input < KEY_SPACE
    //           or current_text_input > 127)
    //          and current_text_input != KEY_MENU);
}

void game_engine::inMenu_get_input() {
    current_menu_input = wgetch(G.inner_menu_frame);
}

void game_engine::inInfo_get_input() {
    current_info_input = wgetch(G.inner_info_frame);
}

void game_engine::inGame_controller() {  // control in-game key response
    inGame_get_input();

    switch (current_text_input) {
        case KEY_MENU: {
            open_menu("inGame", &game_engine::inMenu_controller_inGame);
            break;
        }
        case KEY_NULL:
            G.verify_terminal_size();
            break;
        default: {
            progress();
            break;
        }
    }
}

void game_engine::Challenge_inGame_controller() {
    inGame_get_input();

    switch (current_text_input) {
        case KEY_MENU: {
            open_menu("inGame", &game_engine::inMenu_controller_inGame);
            break;
        }
        case KEY_NULL:
            G.verify_terminal_size();
            break;
        default: {
            Challenge_progress();
            break;
        }
    }
}

void game_engine::progress() {  // game advances 1 cycle
    // dont store input if num char exceeds the total length
    if ((current_game.typed_length == current_lc and
         current_game.incorrect_part_len != 0 and
         current_text_input != KEY_DEL and
         current_text_input != KEY_BACKSPACE and current_text_input != '\b') or
        (current_game.typed_length == 0 and
         (current_text_input == KEY_DEL or
          current_text_input == KEY_BACKSPACE or current_text_input == '\b'))) {
        return;
    }

    inner_stopwatch.update();

    all_inputs.push_back(current_text_input);

    current_game.process_classic(G, current_text_input);
}

void game_engine::Challenge_progress() {
    if ((current_game.typed_length == current_lc and
         current_game.incorrect_part_len != 0 and
         current_text_input != KEY_DEL and
         current_text_input != KEY_BACKSPACE and current_text_input != '\b') or
        (current_game.typed_length == 0 and
         (current_text_input == KEY_DEL or
          current_text_input == KEY_BACKSPACE or current_text_input == '\b'))) {
        return;
    }

    inner_stopwatch.update();

    all_inputs.push_back(current_text_input);

    current_game.process_challenge(G, current_text_input);
}

void game_engine::finish_screen_controller() {
    inGame_get_input();
    switch (current_text_input) {
        case KEY_DASH:
            skipToQuote();
            break;
        case KEY_MENU: {
            open_menu("postGame", &game_engine::inMenu_controller_finishscr);
            break;
        }
        case KEY_r:
            replay();
            break;
        case KEY_SPACE: {
            // break out loop in outro
            skip_game = true;
            return;
        }
        default:
            break;
    }
}

void game_engine::replay() {
    replaying = true;
    // load time records
    auto time_record = inner_stopwatch.durations();
    auto time_record_in_sec = inner_stopwatch.durations_in_sec();

    // reset text frame and get ready for displaying 'replay'
    current_game.clear_text_status();
    current_game.update_text_display(G);

    // get overall_gross wpm for reference
    auto overall_gross_wpm = current_game.gross_wpm;

    // non-blocking getch in text frame
    wtimeout(G.inner_text_frame, 0);

    // use the recorded input to virtually run the game again
    for (auto i = 0; i < all_inputs.size(); ++i) {
        if (not skip_replay) {
            replay_controller();
            this_thread::sleep_for(time_record[i] - avg_overslept);
        } else {  // rapidly run to the end
            this_thread::sleep_for(
                chrono::duration<long double>(replay_skipToEnd_speed));
        }

        // Using color to indicate speed wasn't successful
        // users may have different terminal settings and
        // it seems infeasible to implement on terminal
        // nocolor version is used instead
        current_game.mock_process_nocolor(G, all_inputs[i]);

        G.info_add_str(
            "Now replaying...\nWPM: " + d_to_string(overall_gross_wpm, 2),
            now_replaying_color, true, false);

        show_bottom_msg(inReplay_bottom_msg);

        // G.info_add_str("current gross WPM: " + to_string(mock_wpm), "yellow",
        // false, false, "\n");

        G.draw_keyboard(0, G.info_w / 2 - 10, all_inputs[i],
                        replay_keyboard_color);
    }

    // resume back to blocking getch
    wtimeout(G.inner_text_frame, -1);

    // clear keyboard input in buffer
    flushinp();

    replaying = false;
    skip_replay = false;

    erase_bottom_msg();
    G.info_add_str("overall gross WPM: " + d_to_string(overall_gross_wpm, 2),
                   now_replaying_color, true, false);
    G.info_mvadd_str(G.inner_info_h - 1, 0,
                     "replay is over. Press SPACE to continue",
                     replay_over_msg_color, false, true);

    // after-replay controller
    // didn't separate out since it's straight forward
    int current_key = 0;
    do {  // only accept KEY_SPACE
        current_key = get_input();
    } while (current_key != KEY_SPACE);
}

void game_engine::replay_controller() {
    auto current_key = get_input();

    // ignore empty key
    if (current_key == -1)
        return;

    switch (current_key) {
        case KEY_MENU:
            open_menu("inReplay", &game_engine::inMenu_controller_inReplay);
            break;
        default:
            break;
    }
}

void game_engine::inHistory_controller(long long& start_i, long long& end_i) {
    auto current_key = get_input();

    switch (current_key) {
        case KEY_0: {
            show_stats();
            break;
        }
        case KEY_MENU: {
            open_menu("inHistory", &game_engine::inMenu_controller_inHistory);
            break;
        }
        case KEY_w: {
            inHistory_pageUp(start_i, end_i);
            break;
        }
        case KEY_s: {
            inHistory_pageDown(start_i, end_i);
            break;
        }
        case ARROW_UP: {
            inHistory_recordChoiceMove(UP, start_i, end_i);
            break;
        }
        case ARROW_DOWN: {
            inHistory_recordChoiceMove(DOWN, start_i, end_i);
            break;
        }
        case ARROW_RIGHT:
        case KEY_d: {
            verticle_move = false;
            inHistory_headerChoiceMove(RIGHT);
            break;
        }
        case ARROW_LEFT:
        case KEY_a: {
            verticle_move = false;
            inHistory_headerChoiceMove(LEFT);
            break;
        }
        case KEY_RETURN: {
            verticle_move = true;
            switch (header_choice) {
                case 0:
                    sort_history("id");
                    return;
                case 1:
                    sort_history("wpm");
                    break;
                case 2:
                    sort_history("mistake");
                    break;
                case 3:
                    sort_history("date");
                    break;
                default:
                    break;
            }
            break;
        }
        case KEY_SPACE: {
            inHistory_skipToQuote();
            break;
        }
        default:
            break;
    }
}

/*--------------------------------------------------*/
// menu utilities

/*--------------------*/
// menu logic
void game_engine::inMenu_controller_introscr() {
    inMenu_get_input();
    switch (current_menu_input) {
        case KEY_MENU:
            close_menu();
            break;
        // or the arrow keys LEFT RIGHT
        // menu options can be navigated using a(left) or d(right)
        case ARROW_RIGHT:
        case KEY_d:
            menuChoiceMove(RIGHT, nOption_menu_preGame);
            break;
        case ARROW_LEFT:
        case KEY_a:
            menuChoiceMove(LEFT, nOption_menu_preGame);
            break;
        // and also number keys
        case KEY_1:
            menuChoice = 0;
            break;
        case KEY_2:
            menuChoice = 1;
            break;
        case KEY_3:
            menuChoice = 2;
            break;
        case KEY_4:
            menuChoice = 3;
            break;
        case KEY_RETURN: {  // 'return key' chooses the option
            switch (menuChoice) {
                case 0:  // SKIP
                    cSkip();
                    break;
                case 1:
                    show_History();
                    break;
                case 2:  // QUIT
                    cExit();
                    break;
                case 3:
                    help();
                    break;
                default:
                    break;
            }
            break;
        }
        // shortcut key for some choices
        case KEY_q:
            cExit();
            break;
        case KEY_n:
            cSkip();
            break;
        case KEY_h:
            show_History();
            break;
        case KEY_p:
            help();
            break;
        default:
            break;
    }
}

void game_engine::inMenu_controller_inGame() {
    inMenu_get_input();
    switch (current_menu_input) {
        case KEY_MENU:
            close_menu();
            break;
        case ARROW_RIGHT:
        case KEY_d:
            menuChoiceMove(RIGHT, nOption_menu_inGame);
            break;
        case ARROW_LEFT:
        case KEY_a:
            menuChoiceMove(LEFT, nOption_menu_inGame);
            break;
        case KEY_1:
            menuChoice = 0;
            break;
        case KEY_2:
            menuChoice = 1;
            break;
        case KEY_3:
            menuChoice = 2;
            break;
        case KEY_RETURN: {  // 'return key' chooses the option
            switch (menuChoice) {
                case 0:  // RESTART
                    cRestart();
                    break;
                case 1:  // SKIP
                    cSkip();
                    break;
                case 2:  // QUIT
                    cExit();
                    break;
                default:
                    break;
            }
            break;
        }
        case KEY_q:
            cExit();
            break;
        case KEY_n:
            cSkip();
            break;
        case KEY_r:
            cRestart();
            break;
        default:
            break;
    }
}

void game_engine::inMenu_controller_finishscr() {
    inMenu_get_input();
    switch (current_menu_input) {
        case KEY_MENU:
            close_menu();
            break;
        case ARROW_RIGHT:
        case KEY_d:
            menuChoiceMove(RIGHT, nOption_menu_postGame);
            break;
        case ARROW_LEFT:
        case KEY_a:
            menuChoiceMove(LEFT, nOption_menu_postGame);
            break;
        case KEY_1:
            menuChoice = 0;
            break;
        case KEY_2:
            menuChoice = 1;
            break;
        case KEY_3:
            menuChoice = 2;
            break;
        case KEY_4:
            menuChoice = 3;
            break;
        case KEY_RETURN: {  // 'return key' chooses the option
            switch (menuChoice) {
                case 0:  // RESTART
                    cRestart();
                    break;
                case 1:  // SKIP
                    cSkip();
                    break;
                case 2:
                    show_History();
                    break;
                case 3:  // QUIT
                    cExit();
                    break;
                case 4:
                    help();
                    break;
                default:
                    break;
            }
            break;
        }
        case KEY_q:
            cExit();
            break;
        case KEY_n:
            cSkip();
            break;
        case KEY_r:
            cRestart();
            break;
        case KEY_h:
            show_History();
            break;
        case KEY_p:
            help();
            break;
        default:
            break;
    }
}

void game_engine::inMenu_controller_inReplay() {
    inMenu_get_input();
    switch (current_menu_input) {
        case KEY_MENU:
            close_menu();
            break;
        case ARROW_RIGHT:
        case KEY_d:
            menuChoiceMove(RIGHT, nOption_menu_inReplay);
            break;
        case ARROW_LEFT:
        case KEY_a:
            menuChoiceMove(LEFT, nOption_menu_inReplay);
            break;
        case KEY_1:
            menuChoice = 0;
            break;
        case KEY_2:
            menuChoice = 1;
            break;
        case KEY_RETURN: {
            switch (menuChoice) {
                case 0:  // skip to end
                    replaySkip();
                    return;
                case 1:  // QUIT
                    cExit();
                    break;
                default:
                    break;
            }
            break;
        }
        case KEY_q:
            cExit();
            break;
        case KEY_s:
            replaySkip();
            return;
        default:
            break;
    }
}

void game_engine::inMenu_controller_inHistory() {
    inMenu_get_input();
    switch (current_menu_input) {
        case KEY_MENU:
            close_menu();
            break;
        case ARROW_RIGHT:
        case KEY_d:
            menuChoiceMove(RIGHT, nOption_menu_inHistory);
            break;
        case ARROW_LEFT:
        case KEY_a:
            menuChoiceMove(LEFT, nOption_menu_inHistory);
            break;
        case KEY_1:
            menuChoice = 0;
            break;
        case KEY_2:
            menuChoice = 1;
            break;
        case KEY_RETURN: {
            switch (menuChoice) {
                case 0:  // Return
                    quit_history();
                    return;
                case 1:  // QUIT
                    cExit();
                    break;
                default:
                    break;
            }
            break;
        }
        case KEY_q:
            cExit();
            break;
        case KEY_r:
            quit_history();
            return;
        default:
            break;
    }
}

void game_engine::inMenu_controller_inHelp() {
    inMenu_get_input();
    switch (current_menu_input) {
        case KEY_MENU:
            close_menu();
            break;
            break;
        case ARROW_RIGHT:
        case KEY_d:
            menuChoiceMove(RIGHT, nOption_menu_inHistory);
            break;
        case ARROW_LEFT:
        case KEY_a:
            menuChoiceMove(LEFT, nOption_menu_inHistory);
            break;
        case KEY_1:
            menuChoice = 0;
            break;
        case KEY_2:
            menuChoice = 1;
            break;
        case KEY_RETURN: {
            switch (menuChoice) {
                case 0:  // Return
                    quit_help();
                    return;
                case 1:  // QUIT
                    cExit();
                    break;
                default:
                    break;
            }
            break;
        }
        case KEY_q:
            cExit();
            break;
        case KEY_r:
            quit_help();
            return;
        default:
            break;
    }
}

/*--------------------*/
// support params for inMenu logics
void game_engine::menuChoiceMove(int val, const long long& menu_size) {
    int tempChoice = (menuChoice + val);

    if (tempChoice < 0) {
        tempChoice += menu_size;
    }

    tempChoice %= menu_size;
    menuChoice = tempChoice;
}

/*--------------------*/
// show menu and enter menu logic

void game_engine::open_menu(const string& menu_type,
                            void (game_engine::*controller)()) {
    inMenu = true;
    menuChoice = 0;

    G.print_menu(menu_type, menuChoice);
    erase_bottom_msg();
    show_bottom_msg(inMenu_bottom_msg, true, 2);

    while (inMenu) {
        (this->*controller)();
        if (inMenu) {
            G.print_menu(menu_type, menuChoice);
        }
    }
}

/*--------------------*/
// unshow menu and quit from menu loop
void game_engine::close_menu() {
    inMenu = false;

    G.erase_menu();

    erase_bottom_msg(true);
}

/*--------------------------------------------------*/
// Options behaviors

/*--------------------*/
// respond to QUIT
void game_engine::cExit(int n, const string& msg) {
    if (not isDemo)
        saver.update_nchar(all_inputs);

    Exit(n, msg);
}

/*--------------------*/
// respond to SKIP
void game_engine::cSkip() {
    skip_game = true;

    restart_game = false;

    if (not isDemo)
        saver.update_nchar(all_inputs);

    close_menu();
}

void game_engine::replaySkip() {
    skip_replay = true;

    close_menu();
}

/*--------------------*/
// respond to RESTART
void game_engine::cRestart() {
    skip_game = true;

    restart_game = true;

    if (not isDemo)
        saver.update_nchar(all_inputs);

    close_menu();
}

void game_engine::print_header(const int& highlight) {
    vector<int> inHistory_header_position = {0};
    int temp_sum = 0;
    for (auto i = 0; i < inHistory_header_spacing.size() - 1; ++i) {
        temp_sum += inHistory_header_spacing[i];
        inHistory_header_position.push_back(temp_sum);
    }

    for (int i = 0; i < header_choices.size(); ++i) {
        if (i == highlight) {
            G.text_mvadd_str(0, inHistory_header_position[i], header_choices[i],
                             inHistory_header_highlight_color);
        } else {
            G.text_mvadd_str(0, inHistory_header_position[i], header_choices[i],
                             inHistory_header_color);
        }
    }

    G.text_add_ch('\n', "white");
}

void game_engine::print_record(const int& highlight,
                               vector<string>& history_rec) {
    if (history_rec.size() == 0) {  // if no record in gsave
        G.text_add_str("No Record Found", inHistory_record_highlight_color);
    } else {
        // reformat history_rec
        reformat_spacedTokens(history_rec, inHistory_header_spacing, left);

        for (auto i = 0; i < history_rec.size(); ++i) {
            if (i == highlight) {
                G.text_add_str(history_rec[i] + "\n",
                               inHistory_record_highlight_color);
            } else {
                G.text_add_str(history_rec[i] + "\n", inHistory_record_color);
            }
        }

        // also display glance of the highlighted quote
        if (not isDemo and verticle_move) {
            // get highlighted quote id
            stringstream ss(history_rec[highlight]);
            string token, temp_quote;
            ss >> token;

            highlighted_quote_id = stoi(token);

            temp_quote = q_loader.load_quote(highlighted_quote_id,
                                             inHistory_quoteGlancee_len);

            if (temp_quote == "") {
                G.erase_info();
                G.info_mvadd_str(0, 0, "Quote not found.",
                                 inHistory_quoteGlance_color, false, false);
            } else {
                G.erase_info();
                G.info_mvadd_str(0, 0, temp_quote + "...",
                                 inHistory_quoteGlance_color, false, false);
            }
        }
    }
}

void game_engine::inHistory_recordChoiceMove(const int& val,
                                             long long& start_i,
                                             long long& end_i) {
    verticle_move = true;

    auto record_choices_size = end_i - start_i;

    record_choice += val;

    // weird bug : negative number larger than postive number
    // if remove (header_choice>=0), negative header_choice val can also
    // enter the first if statement
    if (record_choice >= 0 and record_choice >= record_choices_size) {
        inHistory_pageDown(start_i, end_i);
    } else if (record_choice < 0) {
        inHistory_pageUp(start_i, end_i);

        record_choices_size = end_i - start_i;
        record_choice = (int)record_choices_size - 1;
    }
}

void game_engine::inHistory_headerChoiceMove(const int& val) {
    auto header_choices_size = header_choices.size();

    header_choice += val;

    // weird bug : negative number larger than postive number
    // if remove (header_choice>=0), negative header_choice val can also
    // enter the first if statement
    if (header_choice >= 0 and header_choice >= header_choices_size) {
        header_choice = 0;
    } else if (header_choice < 0) {
        header_choice = (int)header_choices_size - 1;
    }
}

// respond to History
// most part of this function need to be migrated to Graphic
void game_engine::show_History() {
    close_menu();

    // reserve first line for header
    // also handling when there're fewer records
    hdisplay_nline =
        ((saver.gsave_len < max_hdisplay_nline) ? (int)saver.gsave_len
                                                : max_hdisplay_nline);

    gsave_last_page_len = ((saver.gsave_len % max_hdisplay_nline == 0)
                               ? max_hdisplay_nline
                               : saver.gsave_len % max_hdisplay_nline);

    long long start_i = 0;
    long long end_i = hdisplay_nline;

    gsave_page_maxnum =
        ((saver.gsave_len <= max_hdisplay_nline)
             ? 1
             : ((saver.gsave_len % max_hdisplay_nline == 0)
                    ? saver.gsave_len / max_hdisplay_nline
                    : saver.gsave_len / max_hdisplay_nline + 1));
    gsave_page_num = 1;

    inHistory = true;
    verticle_move = true;

    header_choice = 0;
    record_choice = 0;

    historyHeader_isAscend = {
        {"id", true}, {"wpm", false}, {"mistake", true}, {"date", true}};

    history_order = "date descend";
    string prev_order = "date ascend";

    vector<string> history_record;

    while (inHistory) {
        G.erase_text();
        G.erase_info();

        // go back to top if reordering happened
        if (prev_order != history_order) {
            record_choice = 0;
            start_i = 0;
            end_i = hdisplay_nline;
            gsave_page_num = 1;
        }

        prev_order = history_order;

        if (verticle_move) {
            history_record = saver.gload(start_i, end_i, history_order);
        }

        print_header(header_choice);

        // show record
        print_record(record_choice, history_record);

        // show page number
        string page_info = "Page:" + to_string(gsave_page_num) + "/" +
                           to_string(gsave_page_maxnum);
        G.info_mvadd_str(0, G.inner_info_w - (int)page_info.length(), page_info,
                         inHistory_pageNum_color, false, false);

        show_bottom_msg(inHistory_bottom_msg, false);

        G.refresh_text_display();
        G.verify_terminal_size();

        inHistory_controller(start_i, end_i);
    }
}

void game_engine::inHistory_pageUp(long long& start_i, long long& end_i) {
    verticle_move = true;

    start_i -= hdisplay_nline;
    end_i = start_i + hdisplay_nline;

    if (start_i < 0) {
        start_i = saver.gsave_len - gsave_last_page_len;
        end_i = saver.gsave_len;
    }

    gsave_page_num--;
    if (gsave_page_num <= 0) {
        gsave_page_num = gsave_page_maxnum;
    }

    record_choice = 0;
}

void game_engine::inHistory_pageDown(long long& start_i, long long& end_i) {
    verticle_move = true;

    start_i += hdisplay_nline;
    end_i = start_i + hdisplay_nline;

    if (start_i >= saver.gsave_len) {
        start_i = 0;
        end_i = hdisplay_nline;
    } else if (end_i > saver.gsave_len) {
        end_i = start_i + gsave_last_page_len;
    }

    gsave_page_num++;
    if (gsave_page_num > gsave_page_maxnum) {
        gsave_page_num = 1;
    }

    record_choice = 0;
}

void game_engine::inHistory_skipToQuote() {
    G.info_mvadd_str(1, 0, "Do you want to skip to this quote? (y/n)",
                     inHistory_skipToQuote_msg_color);
    inInfo_get_input();

    if (current_info_input == KEY_y) {
        auto temp_quote = q_loader.load_quote(highlighted_quote_id);
        if (temp_quote == "") {
            G.info_mvadd_str(2, 0,
                             "Quote not found. Press anything to continue.",
                             "black-yellow");
            inInfo_get_input();
        } else {
            next_quote(highlighted_quote_id);
            skip_game = true;
            restart_game = true;
            quit_history();
        }
    }
}

void game_engine::quit_history() {
    inHistory = false;
    inMenu = false;
    close_menu();
}

void game_engine::sort_history(const string& item) {
    reset_history_order_except(item);
    if (historyHeader_isAscend[item]) {
        history_order = item + " ascend";
        historyHeader_isAscend[item] = false;
    } else {
        history_order = item + " descend";
        historyHeader_isAscend[item] = true;
    }
}

void game_engine::reset_history_order_except(const string& item) {
    if (item != "id")
        historyHeader_isAscend["id"] = true;
    if (item != "wpm")
        historyHeader_isAscend["wpm"] = false;
    if (item != "mistake")
        historyHeader_isAscend["mistake"] = true;
    if (item != "date")
        historyHeader_isAscend["date"] = true;
}

void game_engine::show_stats() {
    auto stats = saver.load_stats();
    G.erase_text();
    G.erase_info();

    // print stats
    // current have 8 stats items
    int spacing_multiplier = 1;
    if (G.win_h > 16) {
        spacing_multiplier = 2;
    }
    for (auto i = 0; i < stats.size(); ++i) {
        G.text_mvadd_str(i * spacing_multiplier, 2, stats[i], "yellow", false,
                         false);
    }
    G.info_add_str("press 0 to go back\n", stats_msg_color);

    G.refresh_text_display();

    do {
        if (current_info_input == KEY_RESIZE)
            G.verify_terminal_size();
        inInfo_get_input();
    } while (current_info_input != KEY_0);
}

void game_engine::skipToQuote() {
    // ask if rechallenge
    string input_ID;
    bool input_error = false;
    bool quote_not_found = false;
    while (true) {
        if (input_error) {
            G.info_add_str("Enter Quote ID: " + input_ID, skipToQuote_msg_color,
                           true, false);
            G.info_add_str(
                "Quote ID must be a integer from 0~" + to_string(ID_MAX),
                skipToQuote_warning_color, false, true, "\n");
        } else if (quote_not_found) {
            G.info_add_str("Enter Quote ID: " + input_ID, skipToQuote_msg_color,
                           true, false);
            G.info_add_str("Quote not found.", "black-yellow", false, true,
                           "\n");
        } else {
            G.info_add_str("Enter Quote ID: " + input_ID, skipToQuote_msg_color,
                           true, false);
        }
        show_bottom_msg(skipToQuote_bottom_msg);
        G.verify_terminal_size();

        inInfo_get_input();
        switch (current_info_input) {
            case KEY_DASH:
                G.erase_info();
                return;
            case KEY_SPACE:
            case KEY_RETURN: {
                if (input_ID == "") {
                    input_error = true;
                    continue;
                }
                auto this_id = stoi(input_ID);
                if (this_id >= 0 and this_id <= ID_MAX) {
                    auto temp_quote = q_loader.load_quote(this_id);
                    if (temp_quote == "") {
                        quote_not_found = true;
                        input_ID = string();
                    } else {
                        quote_not_found = false;
                        next_quote(this_id);
                        skip_game = true;
                        restart_game = true;
                        return;
                    }
                } else {
                    input_error = true;
                    input_ID = string();
                }
                break;
            }
            default: {
                if (current_info_input == KEY_DEL or
                    current_info_input == KEY_BACKSPACE or
                    current_info_input == '\b') {
                    if (input_ID.size() != 0) {
                        input_ID.pop_back();
                        input_error = false;
                    }
                } else {
                    if (current_info_input < KEY_0 or
                        current_info_input > KEY_9) {  // not a number
                        input_error = true;
                    } else {
                        input_ID += current_info_input;
                        input_error = false;
                    }
                }
            }
        }
    }
}

void game_engine::remove_best_record(const int& ask_again, const string& msg) {
    if (best_record.first <= 0) {
        G.info_add_str("No record found for this quote\n",
                       remove_record_msg_color, true, false);
        intro_info_wait = true;
        return;
    }

    string really = "REALLY";

    G.info_add_str(msg + " (y/n)", remove_record_msg_color, true, false);

    show_bottom_msg(removeRecord_bottom_msg);

    inInfo_get_input();
    if (current_info_input == KEY_y) {
        for (auto i = 0; i < ask_again; ++i) {
            G.info_add_str(
                "Are you " + really +
                    " sure that you want to remove this record? (y/n)",
                remove_record_msg_color, true, true);
            inInfo_get_input();
            if (current_info_input != KEY_y) {
                return;
            }
            really += " REALLY";
        }

        if (isDemo) {  // remove best record demo
            demo_best_record =
                pair<double, pair<vector<long long>, vector<int>>>();
        } else {
            saver.psave_deleteline(quote_order[quoteOrder_index]);
        }

        best_record = saver.pload(quote_order[quoteOrder_index]);
        current_game.load_game_info(current_quote, current_lc,
                                    best_record.first, G.win_w, false);
        intro_info_wait = false;
    }
}

void game_engine::help() {
    close_menu();
    G.erase_text();
    G.erase_info();
    inHelp = true;

    vector<pair<string, string>> About = {
        {"For more helps please visit:\n", "magenta"},
        {"https://github.com/ihsuy/Typon", "yellow"},
        {"\n\nAbout Challenge Mode", "magenta"},
        {"\nIf you start a ", "white"},
        {"Game", "green"},
        {" under ", "white"},
        {"Challenge mode", "yellow"},
        {", the ", "white"},
        {"Opponent's Text Cursor", "green"},
        {" will appear and start a ", "white"},
        {"Race", "yellow"},
        {" with you.", "white"},
        {" If you manage to finish before the ", "white"},
        {"Opponent", "green"},
        {" does, ", "white"},
        {"Congratulations", "cyan"},
        {", you've ", "white"},
        {"Beaten", "green"},
        {" your previous ", "white"},
        {"Best Record", "green"},
        {".", "white"},
    };

    while (inHelp) {
        G.erase_text();
        for (auto token : About) {
            G.text_add_str(token.first, token.second, false, false);
        }

        show_bottom_msg(inHelp_bottom_msg, false);

        G.refresh_text_display();  // probably don't need
        G.verify_terminal_size();

        auto tempkey = get_input();
        switch (tempkey) {
            case KEY_MENU:
                open_menu("inHelp", &game_engine::inMenu_controller_inHelp);
                break;
                //            case KEY_SPACE:
                //                // next page
                //            {
                //            }
        }
    }
}

void game_engine::quit_help() {
    inMenu = false;
    inHelp = false;
    close_menu();
}

void game_engine::show_bottom_msg(
    const string& msg,
    const bool& show_id,
    const int&
        nspilt_bar) {  // msg not supposed to be longer than 2 x G.inner_info_w
    // (be careful when adding long bottom msg,
    // give at least 2 lines empty in info window from bottom up
    // show tip msg
    string bottom_msg = msg;

    if (show_id) {
        bottom_msg +=
            " | ID: " +
            ((isDemo) ? "DEMO" : to_string(quote_order[quoteOrder_index]));
    }

    if (bottom_msg.length() < G.inner_info_w) {
        G.info_mvadd_str(G.inner_info_h - 1,
                         G.inner_info_w - (int)bottom_msg.length(), bottom_msg,
                         intro_msg_color, false, true);
    } else {  // find the nth-split bar '|' and print in newline after it
        int bar_count = 0, i = 0;
        for (; i < bottom_msg.size(); ++i) {
            if (bottom_msg[i] == '|')
                bar_count++;
            if (bar_count == nspilt_bar)
                break;
        }
        int p1_len = i + 1;
        ;

        auto msg_p1 = bottom_msg.substr(0, p1_len);
        auto msg_p2 = bottom_msg.substr(p1_len);

        G.info_mvadd_str(G.inner_info_h - 2,
                         G.inner_info_w - (int)msg_p1.length(), msg_p1,
                         intro_msg_color, false, false);
        G.info_mvadd_str(G.inner_info_h - 1,
                         G.inner_info_w - (int)msg_p2.length(), msg_p2,
                         intro_msg_color, false, true);
    }
}

void game_engine::erase_bottom_msg(const bool& refresh, const int& nline) {
    G.mvclrtobot("info", G.inner_info_h - nline, 0, refresh);
}

// This function is disabled since it's called every gameloop cycle
// and it's slowing the program down in a noticeable manner
// probably running this on other thread would be good
// void game_engine::update_inGame_WPM()
//{
//    // calculate
//    auto current_wpm =
//    current_game.calculate_gross_WPM(inner_stopwatch.split(),
//    current_game.correct_part_len);
//
//    // temporary save
//    // inGame_WPM.push_back(current_wpm);
//
//    // show
//    G.info_add_str("gross WPM:" + to_string(current_wpm), "yellow", true,
//    true);
//}

// some removed help msg
// string general = ("HELP & TIPS\n"
//                  "On the Start Screen or End Screen, the hyphen(-) key will
//                  launch Search mode and allow " "you to skip to any quote in
//                  the current jumble of quotes by inputting its ID(number)."
//                  "\n\nOn the Start Screen only, you can press the Number key
//                  0 to launch Challenge mode (Next page for details)" "or the
//                  DELETE key to remove the current quote\'s Best Record if it
//                  exists." "A message asking for confirmation will appear. On
//                  the End Screen only, you can press r to replay your most
//                  recent Game play. You can then choose to" "Skip it by
//                  choosing SkipToEnd in the Menu."
//                  "\n\nYou can Open Menu at almost anytime by pressing the tab
//                  key. The Shortcut to every option in Menu can be" "accessed
//                  by pressing the First Letter of each option."
//                  "\n\nNumber keys can be used to move to the n-th option. a
//                  and d are usually synonymous with ArrowNavigate Through
//                  Records by pressing Arrow Up/Down or going Up/Down page by
//                  page" "by pressing w and s. Game Statistics can be accessed
//                  by pressing 0 here."
//                  "\n\nHistory records can be Sorted by using the left and
//                  right keys to move across the column, then pressing"
//                  "Return. When sorting by different columns, if possible, the
//                  previous order will be preserved."
//                  );
//
// vector<pair<string, string>> General =
//{
//    {"HELP & TIPS\nOn the Start Screen or End Screen, the hyphen(-) key will
//    launch ", "white"},
//    {"Search mode", "yellow"},
//    {" and allow\nyou to skip to ", "white"},
//    {"any quote", "green"},
//    {" in the current jumble of quotes by ", "white"},
//    {"inputting its ", "white"},
//    {"ID(number)", "magenta"},
//    {".", "white"},
//    {"\n\nOn the ", "white"},
//    {"Start Screen", "green"},
//    {" only, you can press the ", "white"},
//    {"Number key 0", "cyan"},
//    {" to launch ", "white"},
//    {"Challenge mode", "yellow"},
//    {" (Next page for details)", "magenta"},
//    {"\nor the ", "white"},
//    {"DELETE", "cyan"},
//    {" key to remove the current quote's ", "white"},
//    {"Best Record", "magenta"},
//    {" if it exists.\nA ", "white"},
//    {"message", "magenta"},
//    {" asking for confirmation will appear.", "white"},
//
//    {"\n\nOn the ", "white"},
//    {"End Screen", "green"},
//    {" only, you can press ", "white"},
//    {"r", "cyan"},
//    {" to replay your most recent ", "white"},
//    {"Game play", "magenta"},
//    {". You can then choose to", "white"},
//    {"\nSkip", "yellow"},
//    {" it by choosing ", "white"},
//    {"SkipToEnd", "magenta"},
//    {" in the ", "white"},
//    {"Menu", "green"},
//    {".", "white"},
//
//    {"\n\nYou can ", "white"},
//    {"Open Menu", "yellow"},
//    {" at almost anytime by pressing the ", "white"},
//    {"tab", "cyan"},
//    {" key.", "white"},
//    {" The ", "white"},
//    {"Shortcut", "yellow"},
//    {" to every option in ", "white"},
//    {"Menu", "green"},
//    {" can be\naccessed by pressing the ", "white"},
//    {"First Letter", "magenta"},
//    {" of each option.", "white"},
//
//    {"\n\nNumber keys", "cyan"},
//    {" can be used to move to the ", "white"},
//    {"n-th option", "magenta"},
//    {". ", "white"},
//    {"a", "cyan"},
//    {" and ", "white"},
//    {"d", "cyan"},
//    {" are usually synonymous with", "white"},
//    {" Arrow Left/Right", "cyan"},
//    {".", "white"},
//
//    {"\n\nIn ", "white"},
//    {"History", "green"},
//    {", You can ", "white"},
//    {"Navigate Through Records", "yellow"},
//    {" by pressing ", "white"},
//    {"Arrow Up/Down", "cyan"},
//    {" or going ", "white"},
//    {"Up/Down page by page", "yellow"},
//    {"\nby pressing ", "white"},
//    {"w", "cyan"},
//    {" and ", "white"},
//    {"s", "cyan"},
//    {". ", "white"},
//    {"Game Statistics", "green"},
//    {" can be accessed by pressing ", "white"},
//    {"0", "cyan"},
//    {" here.", "white"},
//    {"\n\nHistory records", "magenta"},
//    {" can be ", "white"},
//    {"Sorted", "yellow"},
//    {" by using the ", "white"},
//    {"left", "cyan"},
//    {" and ", "white"},
//    {"right keys", "cyan"},
//    {" to move across the column, then pressing\n", "white"},
//    {"Return", "cyan"},
//    {". When sorting by ", "white"},
//    {"different columns", "magenta"},
//    {", if possible, the previous order will be ", "white"},
//    {"preserved", "yellow"},
//    {".", "white"},
//};
