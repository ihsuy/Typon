
#ifndef tools_hpp
#define tools_hpp

#include <ncurses.h>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

bool isShifted(const int& key);

string d_to_string(const double& d, const int& decimal);

double s_to_double(const string& s);

void reformat_spacedTokens(vector<string>& items,
                           const vector<int>& width,
                           ios_base& (*flag)(ios_base&));

void Exit(int n, const string& msg = "");

class Conversion_error : public exception {
   private:
    const string situation;

   public:
    Conversion_error(const string& s) : situation(s) {}

    const char* what() const throw() override {
        string msg = "String to Numeric type conversion failed when ";
        msg += situation;
        return msg.c_str();
    }
};
#endif /* tools_hpp */
