//
//  quote_loader.cpp
//  new_ytype
//
//  Created by Yushi Qiu on 2019/02/07.
//  Copyright © 2019 Yushi Qiu. All rights reserved.
//

#include "quote_loader.hpp"

quote_loader::quote_loader(const int& id_range, const string& specified_quote_dir):ID_RANGE(id_range)
{
    quote_dir = specified_quote_dir + quote_relative_path;
}

// count number of quote files in the
// quote_dir and update quoteDatabase_size
void quote_loader::load_quoteDatabase_info(vector<int>& quote_order, int& quoteDatabase_size, bool& isDemo)
{
    // try to open first quote
    string q_path;
    ifstream open_file;
    
    // if first quote exists, try to look
    // for other quotes. Quote names are
    // supposed to be quote[consecutive number].txt
    for(auto i = 0; i < ID_RANGE; ++i)
    {
        q_path = quote_dir + "quote" + to_string(i) + quote_format;
        open_file.open(q_path);
        
        if(open_file)
        {
            // since quoteFile_count is also the quote_id
            // store it to quote_order
            quote_order.push_back(i);
            open_file.close();
        }
        else
        {
            open_file.clear();
        }
    }
    
    // quote_count is 0 at this place if and only if
    // open_file failed to load quote0.txt which means
    // there's no qualified quote file in quote_dir
    if(quote_order.size() == 0)
    {
        // if no quote, use the default quote
        isDemo = true;
        
        quote_order.push_back(0);
        quoteDatabase_size = 1;
        return;
        //Exit(1, "no quote file found at " + quote_dir);
    }
    
    quoteDatabase_size = (int)quote_order.size();
    
    assert(quoteDatabase_size == quote_order.size());
}


string quote_loader::load_quote(const int& id, const int& len)
{
    string quote_path = (quote_dir + "quote"
                         + to_string(id)
                         + quote_format);
    
    ifstream ifs_text{quote_path};
    // handle bad path input
    if(!ifs_text)
    {
        //Exit(1, "Couldn't find text file at: "+quote_path);
        // return empty string indicating not found
        return "";
    }
    
    stringstream buffer;
    buffer << ifs_text.rdbuf();
    
    string the_quote = buffer.str();
    
    clear_trailing_spaces(the_quote);
    
    if(len == 0)
        return the_quote;
    else
        return the_quote.substr(0, len);
}

void quote_loader::clear_trailing_spaces(string& quote)
{
    while(quote.size() != 0 and (quote.back()==' ' or quote.back()=='\n'))
    {
        quote.pop_back();
    }
}
