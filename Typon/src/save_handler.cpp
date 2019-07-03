
#include "save_handler.hpp"

#include <iostream>
// tools

bool id_ascend(const string& line1, const string& line2) {
    stringstream ss1(line1), ss2(line2);
    string token1, token2;
    ss1 >> token1;
    ss2 >> token2;

    try {
        return stoi(token1) < stoi(token2);
    } catch (...) {
        throw Conversion_error(
            "Reordering History records. (please check gsave ID format)");
    }
}
bool id_descend(const string& line1, const string& line2) {
    stringstream ss1(line1), ss2(line2);
    string token1, token2;
    ss1 >> token1;
    ss2 >> token2;

    try {
        return stoi(token1) > stoi(token2);
    } catch (...) {
        throw Conversion_error(
            "Reordering History records. (please check gsave ID format)");
    }
}
bool wpm_ascend(const string& line1, const string& line2) {
    stringstream ss1(line1), ss2(line2);
    string token1, token2;
    ss1 >> token1 >> token1;
    ss2 >> token2 >> token2;

    try {
        return stod(token1) < stod(token2);
    } catch (...) {
        throw Conversion_error(
            "Reordering History records. (please check gsave WPM format)");
    }
}
bool wpm_descend(const string& line1, const string& line2) {
    stringstream ss1(line1), ss2(line2);
    string token1, token2;
    ss1 >> token1 >> token1;
    ss2 >> token2 >> token2;

    try {
        return stod(token1) > stod(token2);
    } catch (...) {
        throw Conversion_error(
            "Reordering History records. (please check gsave WPM format)");
    }
}
bool mistake_ascend(const string& line1, const string& line2) {
    stringstream ss1(line1), ss2(line2);
    string token1, token2;
    ss1 >> token1 >> token1 >> token1;
    ss2 >> token2 >> token2 >> token2;

    try {
        return stoi(token1) < stoi(token2);
    } catch (...) {
        throw Conversion_error(
            "Reordering History records. (please check gsave Mistake format)");
    }
}
bool mistake_descend(const string& line1, const string& line2) {
    stringstream ss1(line1), ss2(line2);
    string token1, token2;
    ss1 >> token1 >> token1 >> token1;
    ss2 >> token2 >> token2 >> token2;

    try {
        return stoi(token1) > stoi(token2);
    } catch (...) {
        throw Conversion_error(
            "Reordering History records. (please check gsave Mistake format)");
    }
}

save_handler::save_handler(const string& my_addr) : quote_size(0) {
    // strip off the /quotes/ preffix
    gsave_path = my_addr + gsave_relative_path + save_fmt;
    psave_path = my_addr + psave_relative_path + save_fmt;
    stats_path = my_addr + stats_relative_path + save_fmt;
}

void save_handler::prepare_to_save(const vector<int>& quote_order) {
    lookFor_saveFiles();
    validate_gsave();
    gsave_length();
    validate_psave(quote_order);
}

void save_handler::lookFor_saveFiles() {
    // check if gsave, psave and stats file exist, create them
    // if they don't
    ifstream stats_file_try(stats_path);
    if (!stats_file_try) {
        ofstream stats_new_file(stats_path, ios_base::app);
        for (auto entry : stats_entries) {
            stats_new_file << entry << " 0\n";
        }
    }
    stats_file_try.close();

    ifstream gsave_file_try{gsave_path};
    if (!gsave_file_try) {
        ofstream psave_new_file(gsave_path);
    }
    gsave_file_try.close();

    ifstream psave_file_try{psave_path};
    if (!psave_file_try) {
        // throw runtime_error("Can't read from save file at " + psave_path);
        ofstream psave_new_file(psave_path);
    }
    psave_file_try.close();

    // check quote database size and performance record size
    // to see if new quote files added
}

void save_handler::validate_gsave() {
    // remove extranenous newlines in gsave
    ifstream gsave_file(gsave_path);
    if (!gsave_file) {
        Exit(1, "Can't open save file at " + gsave_path);
    }

    string line;
    vector<string> lines;

    while (getline(gsave_file, line)) {  // check if first letter is a number
        if (line != "" and (line[0] < '9' or line[0] > '0')) {
            lines.push_back(line);
        }
    }

    if (lines.size() == 0) {
        return;
    }

    string temp_gaddr =
        gsave_path.substr(0, psave_path.length() - save_fmt.length()) +
        temp_suffix;
    ofstream temp_gsave{temp_gaddr};

    for (auto i = 0; i < lines.size() - 1; ++i) {
        temp_gsave << lines[i] << endl;
    }
    temp_gsave << lines.back();

    temp_gsave.close();
    remove(gsave_path.c_str());
    rename(temp_gaddr.c_str(), gsave_path.c_str());
}

void save_handler::validate_psave(
    const vector<int>& quote_order) {  // check performance record size
    ifstream psave_file_in{psave_path};

    string line, token;
    vector<string> lines;
    vector<string> new_lines;

    vector<int> removed_id;  // currently not used but maybe helpful

    while (getline(psave_file_in, line)) {
        lines.push_back(line);
    }

    auto new_begin = quote_order.begin(), new_end = quote_order.end();
    auto old_begin = lines.begin(), old_end = lines.end();

    if (old_begin == old_end) {  // psave is empty
        for (auto item : quote_order) {
            new_lines.push_back("Quote_id: " + to_string(item));
        }
    } else {
        while (true) {
            stringstream buffer(*old_begin);

            buffer >> token >> token;

            int old_id;
            try {
                old_id = stoi(token);
            } catch (...) {
                throw Conversion_error(
                    "Validating psave file. (please check psave ID format)");
            }

            int new_id = *new_begin;

            if (old_begin + 1 == old_end and new_begin + 1 == new_end) {
                if (old_id == new_id) {
                    new_lines.push_back(*old_begin);
                } else if (old_id > new_id) {
                    removed_id.push_back(old_id);
                } else {
                    new_lines.push_back("Quote_id: " + to_string(new_id));
                }
                break;
            } else if (old_begin + 1 == old_end) {
                if (old_id == new_id) {
                    new_lines.push_back(*old_begin);
                } else {
                    new_lines.push_back("Quote_id: " + to_string(new_id));
                }
                new_begin++;
            } else if (new_begin + 1 == new_end) {
                if (old_id == new_id) {
                    new_lines.push_back(*old_begin);
                } else {
                    removed_id.push_back(old_id);
                }
                old_begin++;
            } else {
                if (old_id == new_id) {
                    new_lines.push_back(*old_begin);
                    new_begin++;
                    old_begin++;
                } else if (old_id > new_id) {
                    new_lines.push_back("Quote_id: " + to_string(new_id));
                    new_begin++;
                } else {
                    removed_id.push_back(old_id);
                    old_begin++;
                }
            }
        }
    }

    if (removed_id.size() != 0) {  // some quotes are removed,
        // warn user and ask whether if he/she meant to
        cout << "Following quote files aren't accessible:";
        for (auto i = 0; i < removed_id.size(); ++i) {
            if (i % 4 == 0)
                cout << endl << '\t';
            cout << "quote" << removed_id[i] << ".txt ";
        }
        cout << "\nTheir best records will be also removed." << endl;
        cout << "Do you want to continue? (y/n)" << endl;
        string user_inp;
        cin >> user_inp;
        if (user_inp != "y") {
            Exit(0,
                 "NOTE:\nIf you are modifying the path to your \"quotes\" "
                 "folder,\nalso migrate the quote files from your previous "
                 "directory to keep their record.");
        }
    }

    string temp_paddr =
        psave_path.substr(0, psave_path.length() - save_fmt.length()) +
        temp_suffix;
    ofstream temp_psave{temp_paddr};

    for (auto new_line : new_lines) {
        temp_psave << new_line << endl;
    }
    temp_psave.close();

    remove(psave_path.c_str());
    rename(temp_paddr.c_str(), psave_path.c_str());
}

void save_handler::save(const int& id,
                        const double& wpm,
                        const int& mis,
                        const vector<long long>& rec,
                        const vector<int>& all_inputs) {
    // save general record

    // open file
    ofstream gsave_file(gsave_path, ios_base::app);
    if (!gsave_file) {
        Exit(1, "Can't open save file at " + gsave_path);
    }

    // create record
    auto time_now =
        chrono::system_clock::to_time_t(chrono::system_clock::now());
    auto time_string = ctime(&time_now);
    //    string msg = "Quote_id: " + to_string(id) + "  " + // 2 spaces
    //                 "Gross_WPM: " + d_to_string(wpm, gsave_wpm_declen) + "  "
    //                 + "Mistakes: " + to_string(mis) + "  " + "Date: " +
    //                 time_string;
    //
    //    gsave_file << msg;

    if (gsave_len != 0)
        gsave_file << "\n";
    gsave_file << left << setw(id_w) << to_string(id);
    gsave_file << left << setw(wpm_w) << d_to_string(wpm, gsave_wpm_declen);
    gsave_file << left << setw(mis_w) << to_string(mis);

    stringstream time_s(time_string);
    vector<string> time_tokens;
    string time_token;

    time_s >> time_token >> time_token;

    do {
        time_tokens.push_back(time_token);

        time_s >> time_token;

    } while (not time_s.eof());

    for (auto tt : time_tokens) {
        gsave_file << tt << " ";
    }

    gsave_len++;

    gsave_file.close();

    // save performance record

    // check if wpm is the best record
    // if yes replace current wpm and rec and inputs with this wpm and rec
    ifstream psave_file(psave_path);

    unsigned long target = 0;
    vector<string> lines;
    string line, token;
    string current_wpm = "0.0";
    stringstream ss;

    while (getline(psave_file, line)) {
        lines.push_back(line);
        ss = stringstream(line);
        // get the second token in line (which is the quote id)
        // and the fourth one which is current wpm
        ss >> token;
        if (token == "Quote_id:") {
            ss >> token;
            if (token == to_string(id)) {
                // found line
                target = lines.size() - 1;
            }
        }
    }

    ss = stringstream(lines[target]);

    ss >> token >> token >> token;
    if (token == "Gross_WPM:") {
        ss >> current_wpm;
    }

    if (wpm > s_to_double(current_wpm)) {
        // ok replace previous records

        auto temp = "Quote_id: " + to_string(id);

        temp += " Gross_WPM: " + d_to_string(wpm, psave_wpm_declen) + " rec: ";

        for (auto i = 0; i < rec.size() - 1; ++i) {
            temp += (to_string(rec[i]) + " ");
        }

        temp += "input: ";
        for (auto c : all_inputs) {
            temp += (to_string(c) + " ");
        }

        lines[target] = temp;

        string temp_paddr =
            psave_path.substr(0, psave_path.length() - save_fmt.length()) +
            temp_suffix;
        ofstream temp_psave{temp_paddr};

        for (auto line : lines) {
            temp_psave << line << endl;
        }

        temp_psave.close();
        remove(psave_path.c_str());
        rename(temp_paddr.c_str(), psave_path.c_str());
    }

    save_stats(all_inputs.size());
}

void save_handler::psave_deleteline(const long long& id) {
    ifstream psave_file(psave_path);

    unsigned long target = 0;
    vector<string> lines;
    string line, token;
    stringstream ss;

    while (getline(psave_file, line)) {
        lines.push_back(line);
        ss = stringstream(line);
        // get the second token in line (which is the quote id)
        // and the fourth one which is current wpm
        ss >> token;
        if (token == "Quote_id:") {
            ss >> token;
            if (token == to_string(id)) {
                // found line
                target = lines.size() - 1;
            }
        }
    }

    string temp;

    temp = "Quote_id: " + to_string(id);

    lines[target] = temp;

    string temp_paddr =
        psave_path.substr(0, psave_path.length() - save_fmt.length()) +
        temp_suffix;
    ofstream temp_psave{temp_paddr};
    for (auto line : lines) {
        temp_psave << line << endl;
    }
    temp_psave.close();
    remove(psave_path.c_str());
    rename(temp_paddr.c_str(), psave_path.c_str());
}

vector<string> save_handler::gload(const long long& start_i,
                                   const long long& end_i,
                                   const string& order) {
    // load all record onto gload_full_record
    // optimization need to be done if user plays too much

    // gsave is empty or only has few items
    if (gsave_len < end_i - start_i) {
        return gload(0, gsave_len, order);
    }

    if (start_i > end_i) {
        Exit(1, "save_handler::gload: start_i (" + to_string(start_i) +
                    ") can't be larger than end_i(" + to_string(end_i) + ").");
    } else if (start_i == end_i) {
        return vector<string>{};
    }

    auto gfr_size = gload_full_record.size();

    if (gfr_size < gsave_len) {
        // calculate how many new record has been create since last update
        auto new_record_n = gsave_len - gfr_size;

        // load those new records to gload_full_record vector
        ifstream gsave_file(gsave_path);
        string line;

        vector<string> data_lines;

        // move to first line of new records
        for (auto i = 0; i < gfr_size; ++i) {
            getline(gsave_file, line);
        }

        for (auto i = 0; i < new_record_n; ++i) {
            getline(gsave_file, line);
            // get only data
            stringstream old(line), fresh;
            string token;

            old >> token;  // get data
            fresh << left << setw(id_w) << token;

            old >> token;
            fresh << left << setw(wpm_w) << token;

            old >> token;
            fresh << left << setw(mis_w) << token;

            old >> token;
            fresh << token << old.rdbuf();

            data_lines.push_back(fresh.str());
        }

        gload_full_record.insert(gload_full_record.end(), data_lines.begin(),
                                 data_lines.end());

        // make a copy
        // bind pointer to it
        record_copy = gload_full_record;
        record_ptr = &record_copy;
    }

    record_reordering(order);

    // slice
    return vector<string>(record_ptr->begin() + start_i,
                          record_ptr->begin() + end_i);
}

void save_handler::record_reordering(const string& order) {
    // sorting with repect to order
    if (order == "date ascend") {
        record_copy = gload_full_record;
        record_ptr = &record_copy;
    } else if (order == "date descend") {
        record_copy = gload_full_record;
        record_ptr = &record_copy;
        reverse(record_ptr->begin(), record_ptr->end());
    } else if (order == "id ascend") {
        stable_sort(record_ptr->begin(), record_ptr->end(), id_ascend);
    } else if (order == "id descend") {
        stable_sort(record_ptr->begin(), record_ptr->end(), id_descend);
    } else if (order == "wpm ascend") {
        stable_sort(record_ptr->begin(), record_ptr->end(), wpm_ascend);
    } else if (order == "wpm descend") {
        stable_sort(record_ptr->begin(), record_ptr->end(), wpm_descend);
    } else if (order == "mistake ascend") {
        stable_sort(record_ptr->begin(), record_ptr->end(), mistake_ascend);
    } else if (order == "mistake descend") {
        stable_sort(record_ptr->begin(), record_ptr->end(), mistake_descend);
    } else {
        Exit(1, "save_handler::gload: undefined sorting order: " + order);
    }
}

pair<double, pair<vector<long long>, vector<int>>> save_handler::pload(
    const int& id) {
    // if record exists for quote id, return pair of
    // vector of the records & wpm
    // otherwise return an empty pair of vector {} and double .0
    ifstream psave_file(psave_path);
    string line, token;

    double wpm = 0.0;
    vector<long long> rec;
    vector<int> inp;

    stringstream ss;
    while (getline(psave_file, line)) {
        ss = stringstream(line);
        ss >> token;
        if (token == "Quote_id:") {
            ss >> token;
            if (token == to_string(id)) {
                // found line

                if (ss.eof()) {
                    // no record yet
                    return pair<double, pair<vector<long long>, vector<int>>>{};
                }

                // next token is "Gross_WPM:" and the next after that is wpm
                ss >> token >> token;
                wpm = s_to_double(token);

                // next token is "rec:" and the next after that is serie of
                // double records
                ss >> token >> token;
                while (token != "input:") {
                    int time_token;
                    try {
                        time_token = stoi(token);
                    } catch (...) {
                        throw Conversion_error(
                            "Loading psave. (Please check psave rec format)");
                    }
                    rec.push_back(time_token);
                    ss >> token;
                }
                // last token is "input:" and the next is serie of int inputs
                ss >> token;
                while (!ss.eof()) {
                    int input_token;
                    try {
                        input_token = stoi(token);
                    } catch (...) {
                        throw Conversion_error(
                            "Loading psave. (Please check psave rec format)");
                    }
                    inp.push_back(input_token);
                    ss >> token;
                }
            }
        }
    }
    return pair<double, pair<vector<long long>, vector<int>>>{
        wpm, pair<vector<long long>, vector<int>>(rec, inp)};
}

long long save_handler::gsave_length() {
    ifstream gsave_file(gsave_path);
    if (!gsave_file) {
        Exit(1, "Can't open save file at " + gsave_path);
    }
    string line;
    long long count = 0;
    while (getline(gsave_file, line)) {
        count++;
    }
    //    if(count > max_gsave_load_line)
    //    {
    //        count = max_gsave_load_line;
    //    }
    gsave_len = count;
    return count;
}

void save_handler::save_stats(const long long& last_game_nchar) {
    long long old_range = 20;
    long long recent_range = 20;

    if (gsave_len < 20) {
        old_range = gsave_len;
        recent_range = gsave_len;
    }

    bool oldwpm_calculated = false;
    bool oldmistake_calculated = false;

    ifstream stats_file(stats_path);
    string line;
    map<string, double> perf;
    vector<string> stats_str;
    // calculate oldest 20 games avg wpm & mistakes (only once)

    // check if old 20 games avg is already calculated
    // since it wont change once calculated
    for (auto i = 0; i < stats_entries.size(); ++i) {
        getline(stats_file, line);
        stats_str.push_back(line);
    }
    string token;
    // go to third line
    stringstream buffer(stats_str[2]);
    buffer >> token >> token;
    if (!buffer.eof()) {
        oldwpm_calculated = true;
        double temp_oldwpm;
        try {
            temp_oldwpm = stod(token);
        } catch (...) {
            throw Conversion_error(
                "loading First 20 games average gross wpm. "
                "(please check stats oldwpm format)");
        }
        perf["oldwpm"] = temp_oldwpm;

        // go to next line for old mistake
        line = stats_str[3];
        buffer = stringstream(line);
        buffer >> token >> token;
        if (!buffer.eof()) {
            oldmistake_calculated = true;
            double temp_oldmis;
            try {
                temp_oldmis = stod(token);
            } catch (...) {
                throw Conversion_error(
                    "loading First 20 games average mistakes. "
                    "(please check stats oldmistake format)");
            }
            perf["oldmistake"] = temp_oldmis;
        }
    }

    ifstream gsave_file(gsave_path);
    double temp_d1, temp_d2, temp_d3, temp_d4;

    if (not oldwpm_calculated or not oldmistake_calculated) {
        temp_d1 = .0;
        temp_d2 = .0;
        for (auto i = 0; i < old_range; ++i) {
            getline(gsave_file, line);
            string token;
            stringstream buffer(line);

            buffer >> token >> token;
            try {
                temp_d1 += stod(token);
            } catch (...) {
                throw Conversion_error(
                    "Calculating first 20 games average gross wpm. "
                    "(please check gsave WPM format)");
            }

            buffer >> token;
            try {
                temp_d2 += stod(token);
            } catch (...) {
                throw Conversion_error(
                    "Calculating first 20 games average gross mistakes. "
                    "(please check "
                    "gsave mistakes format)");
            }
        }

        perf["oldwpm"] = temp_d1 / (double)old_range;
        perf["oldmistake"] = temp_d2 / (double)old_range;
    }

    gsave_file.seekg(0, ios::beg);
    // calculate last 20 games avg wpm & mistakes
    // calculate all time avg wpm & mistakes
    // move to max-20th line
    temp_d1 = .0;
    temp_d2 = .0;
    temp_d3 = .0;
    temp_d4 = .0;
    for (auto i = 0; i < gsave_len; ++i) {
        getline(gsave_file, line);
        buffer = stringstream(line);
        buffer >> token >> token;

        try {
            temp_d1 += stod(token);  // wpm
        } catch (...) {
            throw Conversion_error(
                "Calculating All time average gross wpm. (please "
                "check gsave WPM format)");
        }

        if (i >= gsave_len - recent_range) {
            try {
                temp_d3 += stod(token);  // recent wpm
            } catch (...) {
                throw Conversion_error(
                    "Calculating Recent 20 games average wpm. "
                    "(please check gsave WPM format)");
            }
        }

        buffer >> token;
        try {
            temp_d2 += stod(token);  // mistake
        } catch (...) {
            throw Conversion_error(
                "Calculating All time average mistakes. (please "
                "check gsave mistakes format)");
        }

        if (i >= gsave_len - recent_range) {
            try {
                temp_d4 += stod(token);  // recent mistake
            } catch (...) {
                throw Conversion_error(
                    "Calculating Recent 20 games average mistakes "
                    "(please check gsave mistakes format)");
            }
        }
    }

    perf["alltimewpm"] = temp_d1 / (double)gsave_len;
    perf["alltimemistake"] = temp_d2 / (double)gsave_len;
    perf["recentwpm"] = temp_d3 / (double)recent_range;
    perf["recentmistake"] = temp_d4 / (double)recent_range;

    // total game played
    perf["gameplayed"] = gsave_len;

    // total letter typed
    buffer = stringstream(stats_str.back());
    buffer >> token >> token;

    try {
        temp_d1 = stod(token) + last_game_nchar;
    } catch (...) {
        throw Conversion_error(
            "Updating All time in game key pressed. (please "
            "check stats totalchar format)");
    }

    perf["totalchar"] = temp_d1;

    string temp_saddr =
        stats_path.substr(0, stats_path.length() - save_fmt.length()) +
        temp_suffix;
    ofstream temp_stats(temp_saddr);
    for (auto i = 0; i < stats_entries.size() - 1; ++i) {
        temp_stats << stats_entries[i] << ' ' << perf[stats_entries[i]] << '\n';
    }
    temp_stats << stats_entries.back() << ' ' << setprecision(nchar_precision)
               << perf[stats_entries.back()] << '\n';

    temp_stats.close();
    remove(stats_path.c_str());
    rename(temp_saddr.c_str(), stats_path.c_str());
}

vector<string> save_handler::load_stats() {
    ifstream stats_file(stats_path);
    string line, token1, token2;
    vector<string> perf;
    // calculate oldest 20 games avg wpm & mistakes (only once)

    // check if old 20 games avg is already calculated
    // since it wont change once calculated
    for (auto i = 0; i < stats_entries.size(); ++i) {
        getline(stats_file, line);
        stringstream buffer(line);
        buffer >> token1 >> token2;
        buffer.clear();
        token1 = stats_entries_formated[i];
        buffer << left << setw(38) << token1 << token2;
        perf.push_back(buffer.str());
    }
    return perf;
}

void save_handler::update_nchar(const vector<int>& all_input) {
    ifstream stats_file(stats_path);
    string token;
    vector<string> stats_str;

    for (auto i = 0; i < stats_entries.size(); ++i) {
        getline(stats_file, token);
        stats_str.push_back(token);
    }

    stringstream buffer(stats_str.back());
    string token2;
    buffer >> token2 >> token;

    auto temp_d = all_input.size();

    try {
        temp_d += stod(token);  // recent wpm
    } catch (...) {
        throw Conversion_error(
            "Updating All time in-game key pressed. (please "
            "check stats totalchar format)");
    }

    string temp_saddr =
        stats_path.substr(0, stats_path.length() - save_fmt.length()) +
        temp_suffix;
    ofstream temp_stats(temp_saddr);
    for (auto i = 0; i < stats_str.size() - 1; ++i) {
        temp_stats << stats_str[i] << '\n';
    }
    temp_stats << token2 << ' ' << setprecision(nchar_precision) << temp_d;

    temp_stats.close();
    remove(stats_path.c_str());
    rename(temp_saddr.c_str(), stats_path.c_str());
}

// [Deprecated] cached gload for better memory performance
// vector<string> save_handler::gload_windowed(const long long& start_i, const
// long long& end_i, const bool& include_header)
//{
//    if(start_i > end_i)
//    {
//        throw runtime_error("save_handler::gload: start_i can't be larger than
//        end_i");
//    }
//    else if(start_i == end_i)
//    {
//        return vector<string>{};
//    }
//
//    //
//    //    cout << "c_start: " << cache_start << " c_end: " << cache_end <<
//    endl;
//    //    cout << "start_1: " << start_i << " end_i: " << end_i << endl;
//    //cout << "cache_start: " << cache_start << " cache_end: " << cache_end <<
//    "save_len" << gsave_len << endl; if(gload_cache.size() == 0
//       or start_i > cache_end
//       or start_i < cache_start
//       or end_i > cache_end)
//    {
//        // figure out cache range
//        cache_start = start_i - cache_range;
//        cache_end = end_i + cache_range;
//        if(cache_start < 0)
//        {
//            cache_start = 0;
//        }
//        if(cache_end > gsave_length())
//        {
//            cache_end = gsave_len;
//        }
//
//        ifstream gsave_file(gsave_path);
//        string line;
//
//        // fast move to cache_start
//        for(auto i = 0; i < cache_start; ++i)
//        {
//            getline(gsave_file, line);
//        }
//
//        vector<string> data_lines;
//
//        for(auto i = 0; i < (cache_end-cache_start); ++i)
//        {
//            getline(gsave_file, line);
//            // get only data
//            stringstream old(line), fresh;
//            string token, qid, gwpm, mis, dat;
//
//            old >> qid; // ignore "Quote_id:"
//            old >> token; // get data
//            fresh << left << setw(id_w) << token;
//            old >> gwpm; // ignore "Gross_WPM:"
//            old >> token;
//            fresh << left << setw(wpm_w) << token;
//            old >> mis; // ignore "Mistakes:"
//            old >> token;
//            fresh << left << setw(mis_w) << token;
//            old >> dat;
//            fresh << old.rdbuf();
//
//            if(include_header and i == 0)
//            {
//                stringstream header;
//                header << left << setw(id_w) << qid;
//                header << left << setw(wpm_w) << gwpm;
//                header << left << setw(mis_w) << mis;
//                header << (" " + dat);
//                header_s = header.str();
//            }
//            data_lines.push_back(fresh.str());
//        }
//        gload_cache = data_lines;
//    }
//
//    //    for(auto item: gload_cache)
//    {
//        //        cout << "gcache: " << item << endl;
//        //    }
//        //    cout << "gcache length: " << gload_cache.size() << endl;
//
//        vector<string> res;
//        if(include_header)
//        {
//            res.push_back(header_s);
//            res.insert(res.begin()+1, gload_cache.begin()+ start_i -
//            cache_start, gload_cache.begin() + end_i - cache_start);
//        }
//        else{
//            res = vector<string>(gload_cache.begin()+(start_i - cache_start),
//            gload_cache.begin() + end_i - cache_start);
//        }
//
//        // slice
//        return res;
//
//    }
