
#include "tools.hpp"

bool isShifted(const int& key) {
    return ((key >= 65 and key <= 90) or (key >= 33 and key <= 38) or
            (key >= 40 and key <= 43) or (key == 58) or (key == 60) or
            (key >= 62 and key <= 64) or (key >= 94 and key <= 95) or
            (key == 123) or (key == 125));
}

string d_to_string(const double& d, const int& decimal) {
    stringstream ss;
    ss << setprecision(decimal) << fixed << d;
    return ss.str();
}

double s_to_double(const string& s) {
    stringstream ss{s};
    double d;
    ss >> d;
    return d;
}

void reformat_spacedTokens(vector<string>& items,
                           const vector<int>& width,
                           ios_base& (*flag)(ios_base&)) {
    stringstream buffer;
    string token;
    const unsigned long n = width.size();
    for (auto& item : items) {
        vector<string> tokens;
        buffer = stringstream(item);
        for (auto i = 0; i < n - 1; ++i) {
            buffer >> token;
            tokens.push_back(token);
        }
        buffer >> token;
        while (!buffer.eof()) {
            string piece;
            buffer >> piece;
            token += " " + piece;
        }
        tokens.push_back(token);

        assert(tokens.size() == n);

        stringstream washer;
        for (auto i = 0; i < n; ++i) {
            washer << flag << setw(width[i]) << tokens[i];
        }
        item = washer.str();
    }
}

void Exit(int n, const string& msg) {
    // quit curses
    endwin();
    // throw an message if given
    if (msg != "") {
        cout << "" << msg << endl;
    }
    // quit program
    exit(n);
}
