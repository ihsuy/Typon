#include "game_mode.hpp"

using namespace std;
char Game_mode::key_replace(
    char in,
    char tar,
    char sub) {  // replace 'in' key with 'sub' key if the 'in' key is 'tar' key
    return (in == tar) ? sub : in;
}

void Game_mode::load_game_info(const string& c_quote,
                               const unsigned long& c_lc,
                               const double& best_wpm,
                               const int& win_w,
                               const bool& do_prettify) {
    char_per_row = win_w;

    current_quote = c_quote;

    current_lc = c_lc;

    best_WPM = best_wpm;

    if (do_prettify)
        quote_prettify();
}

// void quote_prettify_ver1()
//{   // requires information about Graphics (window width)
//    current_quote_for_display = current_quote;
//
//    // look for first '\n'
//    bool contain_newline = false;
//
//    // begin from the frist line
//    size_t loc = 0;
//    // try to find the first newline character
//    auto newlineloc = current_quote.find("\n", loc);
//    if(newlineloc != string::npos)
//    {   // if a newline character is detected
//        // calculate how from this location until the newline character
//        contain_newline = true;
//        auto newlinelen = newlineloc - loc;
//        if(newlinelen >= char_per_row)
//        {   // if the distance aforementioned is longer than or equal to the
//        length(char_per_row)
//            // of a row in the game window, that means the first line is
//            complete
//            // safely increment loc by char_per_row and loc becomes the next
//            target location loc += char_per_row;
//        }
//        else
//        {   // if the newline charactr is shorter than char_per_row
//            // which means the first line isn't complete so there's no
//            // need to break it
//            // simple go ahead and set loc to newlinelen + char_per_row
//            // which is the target location in the next line
//            loc += newlinelen;
//        }
//    }
//    else
//    {
//        loc += char_per_row;
//    }
//
//    while(loc < current_quote.size())
//    {
//        bool inside = false;
//        if((current_quote_for_display[loc] == ' ' and
//        current_quote_for_display[loc-1] != ' ')
//           or (current_quote_for_display[loc] != ' ' and
//           current_quote_for_display[loc-1] != ' '))
//        {
//            loc = current_quote_for_display.rfind(" ",  loc-1);
//            current_quote_for_display.replace(loc, 1, "\n");
//            inside = true;
//        }
//
//        if(contain_newline){
//            newlineloc = current_quote.find("\n", loc);
//            if(newlineloc != string::npos)
//            {
//                auto newlinelen = newlineloc - loc;
//                if(newlinelen >= char_per_row)
//                {
//                    loc += (char_per_row + (inside?1:0));
//                }
//                else
//                {
//                    loc += (char_per_row + newlinelen + (inside?1:0));
//                }
//            }
//            else
//            {
//                loc += (char_per_row + (inside?1:0));
//            }
//        }
//        else
//        {
//            loc += (char_per_row + (inside?1:0));
//        }
//    }
//}

void Game_mode::quote_prettify() {
    current_quote_for_display = current_quote;

    auto ptr = current_quote_for_display.begin();
    while (true) {
        auto ptr_end = ptr + char_per_row;
        if (ptr_end >= current_quote_for_display.end()) {
            break;
        }

        while (*ptr != '\n' and ptr != ptr_end) {
            ptr++;
        }

        if (ptr == ptr_end) {  // didn't find a newline in nextline
            if ((*ptr == ' ' and *(ptr - 1) != ' ') or
                (*ptr != ' ' and *(ptr - 1) != ' ')) {
                auto temp_ptr = ptr - 1;
                while (*temp_ptr != ' ' and *temp_ptr != '\n' and
                       temp_ptr != current_quote_for_display.begin()) {
                    temp_ptr--;
                }
                if (*temp_ptr == '\n' or
                    temp_ptr == current_quote_for_display.begin()) {
                    int lword_len = 0;
                    while (*temp_ptr != ' ' and
                           temp_ptr != current_quote_for_display.end()) {
                        lword_len++;
                        temp_ptr++;
                    }

                    ptr = temp_ptr - (lword_len % char_per_row - 1);
                } else {
                    *temp_ptr = '\n';
                    ptr = temp_ptr + 1;
                }
            } else {
                *(ptr - 1) = '\n';
            }
        } else {
            ptr++;
            continue;
        }
    }
}

vector<pair<int, int>> Game_mode::get_total_input() {
    return total_input;
}

double Game_mode::calculate_gross_WPM(const double& t,
                                      const unsigned long& lc) {
    // gross wpm = (all typed entires/5)/time(min)
    if (t <= 0.0) {
        throw runtime_error(
            "time_elapsed isn't corrected calculated. Invalid value: " +
            to_string(t));
    }
    return (lc / 5.0) * 60 / t;
}

// perhaps not quite interesting?
double Game_mode::calculate_net_WPM(const double& t,
                                    const unsigned long& lc,
                                    const unsigned int& miss_count) {
    if (t <= 0.0) {
        throw runtime_error(
            "time_elapsed isn't corrected calculated. Invalid value: " +
            to_string(t));
    }

    return ((lc / 5.0) - miss_count) * 60 / t;
}

void Game_mode::update_text_display_classic(Graphics& g) {
    g.erase_text();
    vector<pair<string, string>> parts;
    typed_length = correct_part_len + incorrect_part_len;
    if (incorrect_part_len == 0) {
        parts = {
            make_pair(current_quote_for_display.substr(0, correct_part_len),
                      correct_color),  // correct part
            make_pair(current_quote_for_display.substr(correct_part_len, 1),
                      cursor_color),  // cursor
            make_pair(
                current_quote_for_display.substr(
                    (correct_part_len == current_lc) ? correct_part_len
                                                     : correct_part_len + 1),
                untyped_color)  // untyped part
        };
    } else {
        parts = {
            make_pair(current_quote_for_display.substr(0, correct_part_len),
                      correct_color),  // correct part
            make_pair(current_quote_for_display.substr(correct_part_len,
                                                       incorrect_part_len),
                      incorrect_color),  // incorrect part
            make_pair(current_quote_for_display.substr(typed_length),
                      untyped_color)  // untyped part
        };
    }
    for (auto part : parts) {
        g.text_add_str(part.first, part.second);
    }
    g.refresh_text_display();
}

void Game_mode::update_text_display(Graphics& g) {
    locker.lock();

    g.erase_text();

    typed_length = correct_part_len + incorrect_part_len;

    bool no_ic = (incorrect_part_len == 0);

    long long opponent_pos = (opponents_len == 0) ? 0 : opponents_len - 1;

    int wheres_op = 0;  // 0 no op; 1 op in c; 2 op in ic; 3 op in utp

    vector<pair<string, string>> parts;

    if (no_ic) {
        if (opponents_len - 1 == correct_part_len or opponents_len == 0) {
            wheres_op = 0;
        } else {
            if (opponents_len <= correct_part_len) {
                wheres_op = 1;
            } else {
                wheres_op = 3;
            }
        }

        switch (wheres_op) {
            case 0:
                parts = {
                    make_pair(
                        current_quote_for_display.substr(0, correct_part_len),
                        correct_color),  // correct part
                    make_pair(
                        current_quote_for_display.substr(correct_part_len, 1),
                        cursor_color),  // cursor
                    make_pair(current_quote_for_display.substr(
                                  (correct_part_len == current_lc)
                                      ? correct_part_len
                                      : correct_part_len + 1),
                              untyped_color)  // untyped part
                };
                break;
            case 1:
                parts = {
                    make_pair(current_quote_for_display.substr(0, opponent_pos),
                              correct_color),  // correct part 1
                    make_pair(current_quote_for_display.substr(opponent_pos, 1),
                              opponent_color),  // opponent
                    make_pair(
                        current_quote_for_display.substr(
                            opponent_pos + 1, correct_part_len - opponents_len),
                        correct_color),  // correct part 2
                    make_pair(
                        current_quote_for_display.substr(correct_part_len, 1),
                        cursor_color),  // cursor
                    make_pair(current_quote_for_display.substr(
                                  (correct_part_len == current_lc)
                                      ? correct_part_len
                                      : correct_part_len + 1),
                              untyped_color)  // untyped part
                };
                break;
            case 3:
                parts = {
                    make_pair(
                        current_quote_for_display.substr(0, correct_part_len),
                        correct_color),  // correct part
                    make_pair(
                        current_quote_for_display.substr(correct_part_len, 1),
                        cursor_color),  // cursor
                    make_pair(current_quote_for_display.substr(
                                  correct_part_len + 1,
                                  opponent_pos - correct_part_len - 1),
                              untyped_color),  // untyped part 1
                    make_pair(current_quote_for_display.substr(opponent_pos, 1),
                              opponent_color),  // opponent
                    make_pair(
                        current_quote_for_display.substr(opponent_pos + 1),
                        untyped_color)  // untyped part
                };
                break;
            default:
                break;
        }
    } else {  // there's incorrect part which means no cursor
        if (opponents_len == 0) {
            wheres_op = 0;
        } else if (opponents_len <= correct_part_len) {
            wheres_op = 1;
        } else if (opponents_len <= typed_length) {
            wheres_op = 2;
        } else {
            wheres_op = 3;
        }

        switch (wheres_op) {
            case 0:
                parts = {
                    make_pair(
                        current_quote_for_display.substr(0, correct_part_len),
                        correct_color),  // correct part
                    make_pair(current_quote_for_display.substr(
                                  correct_part_len, incorrect_part_len),
                              incorrect_color),  // incorrect part
                    make_pair(current_quote_for_display.substr(typed_length),
                              untyped_color)  // untyped part
                };
                break;
            case 1:
                parts = {
                    make_pair(current_quote_for_display.substr(0, opponent_pos),
                              correct_color),  // correct part 1
                    make_pair(current_quote_for_display.substr(opponent_pos, 1),
                              opponent_color),  // opponent
                    make_pair(
                        current_quote_for_display.substr(
                            opponent_pos + 1, correct_part_len - opponents_len),
                        correct_color),  // correct part 2
                    make_pair(current_quote_for_display.substr(
                                  correct_part_len, incorrect_part_len),
                              incorrect_color),  // incorrect part
                    make_pair(current_quote_for_display.substr(typed_length),
                              untyped_color)  // untyped part
                };
                break;
            case 2:
                parts = {
                    make_pair(
                        current_quote_for_display.substr(0, correct_part_len),
                        correct_color),  // correct part
                    make_pair(current_quote_for_display.substr(
                                  correct_part_len,
                                  opponents_len - correct_part_len - 1),
                              incorrect_color),  // incorrect part 1
                    make_pair(current_quote_for_display.substr(opponent_pos, 1),
                              opponent_color),  // opponent
                    make_pair(current_quote_for_display.substr(
                                  opponents_len, typed_length - opponents_len),
                              incorrect_color),  // incorrect part 2
                    make_pair(current_quote_for_display.substr(typed_length),
                              untyped_color)  // untyped part
                };
                break;
            case 3:
                parts = {
                    make_pair(
                        current_quote_for_display.substr(0, correct_part_len),
                        correct_color),  // correct part
                    make_pair(current_quote_for_display.substr(
                                  correct_part_len, incorrect_part_len),
                              incorrect_color),  // incorrect part
                    make_pair(
                        current_quote_for_display.substr(
                            typed_length, opponents_len - typed_length - 1),
                        untyped_color),  // untyped part 1
                    make_pair(current_quote_for_display.substr(opponent_pos, 1),
                              opponent_color),  // opponent
                    make_pair(
                        current_quote_for_display.substr(
                            (opponent_pos == current_lc) ? opponent_pos
                                                         : opponent_pos + 1),
                        untyped_color)  // untyped part
                };
                break;
            default:
                break;
        }
    }

    for (auto part : parts) {
        g.text_add_str(part.first, part.second);
    }

    g.refresh_text_display();

    locker.unlock();
}

/*--------------------*/
// design of before-game screen
void Game_mode::intro_screen(Graphics& g) {
    // clear previous text
    // set current text color for intro screen
    // embed current quote to inner frame
    g.text_add_str(current_quote_for_display, intro_screen_text_color, true,
                   true);
    g.verify_terminal_size();
}

void Game_mode::finish_screen(Graphics& g) {
    // clear inner frame
    g.erase_text();

    display_total_input(g);

    display_performance(g);

    g.refresh_text_display();
    g.verify_terminal_size();
}

void Game_mode::display_total_input(Graphics& g) {
    for (auto record : total_input) {
        switch (record.second) {
            case 0:
                // 0 means correct
                g.text_add_ch(record.first, finish_screen_correct_color);
                break;
            case 1:
                // means incorrect
                g.text_add_ch(record.first, finish_screen_incorrect_color);
                break;
        }
    }
}

void Game_mode::calculate_performance() {
    gross_wpm = calculate_gross_WPM(time_elapsed, current_lc);

    // disabled
    // net_wpm = calculate_net_WPM(time_elapsed, current_lc, mistake_count);
}

void Game_mode::display_performance(Graphics& g) {
    // performace:
    //     Word count, Time elapsed,
    //     WPM, Quote personal best WPM
    //     All time avg, Recent 10 game avg,
    //     Mistake count, All time avg mistake, Recent 10 game avg mistake

    string wpm_s = "WPM: " + d_to_string(gross_wpm, 2);
    string best_s = "History best WPM: " + d_to_string(best_WPM, 2);
    string time_s = "Time elapsed: " + d_to_string(time_elapsed, 2) + "s";
    string mistakes_s = "Mistakes: " + to_string(mistake_count);

    g.info_add_str(wpm_s, "cyan", true, false);

    if (gross_wpm > best_WPM) {
        best_s = "NEW RECORD!! (" + d_to_string(best_WPM, 2) + "->" +
                 d_to_string(gross_wpm, 2) + ")";
        g.info_add_str(best_s, "yellow", false, false, "\t");
    } else {
        g.info_add_str("History best WPM: " + d_to_string(best_WPM, 2),
                       "yellow", false, false, "\t");
    }

    // disabled
    // g.info_add_str("net WPM: " + to_string(net_wpm), "white", false, false,
    // "\n");
    if (mistake_count == 0) {
        mistakes_s += "\tPERFECT!!";
    }
    g.info_add_str(mistakes_s, "cyan", false, false, "\n");
    g.info_add_str(time_s, "cyan", false, false, "\n");
}

/*--------------------*/
// test if game is completed
bool Game_mode::game_complete() {
    return (correct_part_len == current_lc);
}

// reset class properties to default
void Game_mode::reset_game() {
    total_input = vector<pair<int, int>>{};
    mistake_count = int{};
    opponents_len = 0;

    correct_part_len = 0;
    incorrect_part_len = 0;
    typed_length = 0;
}

void Game_mode::clear_text_status() {
    correct_part_len = 0;
    incorrect_part_len = 0;
    typed_length = 0;

    speed_indicator = vector<int>{};
}

void Game_mode::process_classic(
    Graphics& g,
    int& ch) {  // total_input records all letter input and whether if
    // they were correct

    if (incorrect_part_len == 0) {
        if (ch == KEY_DEL or ch == KEY_BACKSPACE or ch == '\b') {
            correct_part_len--;
            total_input.pop_back();
        } else {
            if (ch == current_quote[correct_part_len]) {
                correct_part_len++;
                total_input.push_back(pair<int, int>(ch, 0));
            } else if (current_quote_for_display[correct_part_len] == '\n') {
                correct_part_len++;
                total_input.push_back(pair<int, int>(' ', 0));
            } else {
                ++mistake_count;
                // do key_replace for clearer display of KEY_SPACE on screen
                incorrect_part_len++;
                total_input.push_back(
                    pair<int, int>(key_replace(ch, KEY_SPACE, '_'), 1));
            }
        }
    } else {
        if (ch == KEY_DEL or ch == KEY_BACKSPACE or ch == '\b') {
            incorrect_part_len--;
        } else {
            incorrect_part_len++;
            total_input.push_back(
                pair<int, int>(key_replace(ch, KEY_SPACE, '_'), 1));
        }
    }
    update_text_display_classic(g);
}

void Game_mode::process_challenge(
    Graphics& g,
    int& ch) {  // total_input records all letter input and whether if
    // they were correct

    if (incorrect_part_len == 0) {
        if (ch == KEY_DEL or ch == KEY_BACKSPACE or ch == '\b') {
            correct_part_len--;
            total_input.pop_back();
        } else {
            if (ch == current_quote[correct_part_len]) {
                correct_part_len++;
                total_input.push_back(pair<int, int>(ch, 0));
            } else if (current_quote_for_display[correct_part_len] == '\n') {
                correct_part_len++;
                total_input.push_back(pair<int, int>(' ', 0));
            } else {
                ++mistake_count;
                // do key_replace for clearer display of KEY_SPACE on screen
                incorrect_part_len++;
                total_input.push_back(
                    pair<int, int>(key_replace(ch, KEY_SPACE, '_'), 1));
            }
        }
    } else {
        if (ch == KEY_DEL or ch == KEY_BACKSPACE or ch == '\b') {
            incorrect_part_len--;
        } else {
            incorrect_part_len++;
            total_input.push_back(
                pair<int, int>(key_replace(ch, KEY_SPACE, '_'), 1));
        }
    }
    update_text_display(g);
}

void Game_mode::mock_process_nocolor(Graphics& g, int& ch) {
    if (incorrect_part_len == 0) {
        if (ch == KEY_DEL or ch == KEY_BACKSPACE or ch == '\b') {
            correct_part_len--;
        } else {
            if (ch == current_quote[correct_part_len]) {
                correct_part_len++;
            } else {
                // do key_replace for clearer display of KEY_SPACE on screen
                incorrect_part_len++;
            }
        }
    } else {
        if (ch == KEY_DEL or ch == KEY_BACKSPACE or ch == '\b') {
            incorrect_part_len--;
        } else {
            incorrect_part_len++;
        }
    }

    update_mock_display_nocolor(g);
}

void Game_mode::update_mock_display_nocolor(Graphics& g) {
    g.erase_text();
    vector<pair<string, string>> parts;
    typed_length = correct_part_len + incorrect_part_len;
    if (incorrect_part_len == 0) {
        parts = {
            make_pair(current_quote_for_display.substr(0, correct_part_len),
                      nocolor_replay_correct_color),  // correct part
            make_pair(current_quote_for_display.substr(correct_part_len, 1),
                      nocolor_replay_cursor_color),  // cursor
            make_pair(
                current_quote_for_display.substr(
                    (correct_part_len == current_lc) ? correct_part_len
                                                     : correct_part_len + 1),
                untyped_color)  // untyped part
        };
    } else {
        parts = {
            make_pair(current_quote_for_display.substr(0, correct_part_len),
                      nocolor_replay_correct_color),  // correct part
            make_pair(current_quote_for_display.substr(correct_part_len,
                                                       incorrect_part_len),
                      nocolor_replay_incorrect_color),  // incorrect part
            make_pair(current_quote_for_display.substr(typed_length),
                      untyped_color)  // untyped part
        };
    }
    for (auto part : parts) {
        g.text_add_str(part.first, part.second);
    }
    g.refresh_text_display();
}
