
#ifndef save_handler_hpp
#define save_handler_hpp

#include "save_handler.hpp"
#include "tools.hpp"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct save_handler {
    // default parameters

    // load at most 1000 lines from gsave
    // const long long max_gsave_load_line = 1000;

    const int gsave_wpm_declen = 2;
    const int psave_wpm_declen = 2;
    const int psave_rec_declen = 4;

    const int id_w = 15;
    const int wpm_w = 20;
    const int mis_w = 15;

    const string gsave_relative_path = "/gsave";
    const string psave_relative_path = "/psave";
    const string stats_relative_path = "/stats";
    const string save_fmt = ".txt";
    const string temp_suffix = "_temp.txt";

    vector<string> stats_entries{
        "alltimewpm", "alltimemistake", "oldwpm",     "oldmistake",
        "recentwpm",  "recentmistake",  "gameplayed",
        "totalchar"  // always keep at bottom
    };
    vector<string> stats_entries_formated{"All Time Average Gross WPM:",
                                          "All Time Average Mistakes:",
                                          "First 20 Games Average Gross WPM:",
                                          "First 20 Games Average Mistakes:",
                                          "Recent 20 Games Average Gross WPM:",
                                          "Recent 20 Games Average Mistakes:",
                                          "Number of Games Played:",
                                          "All Time In-Game Key Pressed:"};

    string gsave_path;
    string psave_path;
    string stats_path;
    const int quote_size;

    vector<string> gload_full_record;
    vector<string> record_copy;
    vector<string>* record_ptr;
    void record_reordering(const string& order);

    string header_s;

    long long gsave_length();
    long long gsave_len = 0;

    const int nchar_precision = 15;

    save_handler(const string& my_addr);

    // check quote size and performance record size
    void prepare_to_save(const vector<int>& quote_order);

    void lookFor_saveFiles();
    void validate_psave(const vector<int>& quote_order);
    void validate_gsave();

    void save(const int& id,
              const double& wpm,
              const int& mis,
              const vector<long long>& rec,
              const vector<int>& all_inputs);

    vector<string> gload(const long long& start_i,
                         const long long& end_i,
                         const string& order = "date descend");
    pair<double, pair<vector<long long>, vector<int>>> pload(const int& id);
    void psave_deleteline(const long long& id);

    void save_stats(const long long& last_game_nchar);
    vector<string> load_stats();

    void update_nchar(const vector<int>& all_inputs);

    // Deprecated
    //    vector<string> gloader_cache;
    //        long long cache_range = 250;
    //        long long cache_start = 0;
    //        long long cache_end= 0;
    //    vector<string> gloader_windowed(const long long& start_i,
    //                                    const long long& end_i,
    //                                    const bool& include_header=true);
};

#endif /* save_handler_hpp */
