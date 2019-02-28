//
//  quote_loader.hpp
//  new_ytype
//
//  Created by Yushi Qiu on 2019/02/07.
//  Copyright © 2019 Yushi Qiu. All rights reserved.
//

#ifndef quote_loader_hpp
#define quote_loader_hpp

#include "tools.hpp"

#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

struct quote_loader
{
    const string quote_relative_path = "/quotes/";
    string quote_dir;
    const int ID_RANGE;
    
    // default read-in quote file format
    string quote_format = ".txt";
    
    quote_loader(const int& id_range, const string& specified_quote_dir="");
    
    void load_quoteDatabase_info(vector<int>& quote_order, int& quoteDatabase_size, bool& isDemo);
    
    string load_quote(const int& id, const int& len=0);
    
    void clear_trailing_spaces(string& quote);
};
#endif /* quote_loader_hpp */

