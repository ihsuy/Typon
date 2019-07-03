
#ifndef quote_loader_hpp
#define quote_loader_hpp

#include "tools.hpp"

#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

struct quote_loader {
    const string quote_relative_path = "/quotes/";
    string quote_dir;
    const int ID_RANGE;

    // default read-in quote file format
    string quote_format = ".txt";

    quote_loader(const int& id_range, const string& specified_quote_dir = "");

    // get total number of quote files in my_path and load them to quote_order
    void load_quoteDatabase_info(vector<int>& quote_order,
                                 int& quoteDatabase_size,
                                 bool& isDemo);

    string load_quote(const int& id, const int& len = 0);

    void clear_trailing_spaces(string& quote);
};
#endif /* quote_loader_hpp */
