#include "game_engine.hpp"
#include "tools.hpp"
#include <iostream>
using namespace std;

int main(int argc, const char *argv[])
{
    try{
        string user_defined_quote_path = "./";
        int MAX_ID = 3001;
        int user_defined_w = 0;
        int user_defined_h = 0;
        bool execute = false;

        if(argc == 1)
        {   // if no argument passed to typon, start game
            execute = true;
        }
        else{
            for(auto i = 1; i < argc; ++i)
            {   // available flags: -path and -whereami
                if(!strcmp(argv[i], "-path"))
                {
                    if(i != argc-1)
                    {
                        ++i;
                        user_defined_quote_path = argv[i];
                        if(user_defined_quote_path.back() != '/')
                            user_defined_quote_path += "/";
                        execute = true;
                    }
                    else
                    {
                        char* real_path = realpath(".", NULL);
                        printf("%s/quotes/\n", real_path);
                    }
                }
                else if(!strcmp(argv[i], "-resize"))
                {
                    if(i < argc-2)
                    {
                        try
                        {
                            user_defined_w = stoi(argv[++i]);
                            user_defined_h = stoi(argv[++i]);
                        }
                        catch (...)
                        {
                            printf("Invalid arguments for -resize:\n%s %s\n%s", argv[i+1], argv[i+2],
                                   "input must be 2 integers\nindicating width and height.\n");
                        }
                        execute = true;
                    }
                    else
                    {
                        printf("Too few arguments for -resize.\ninput must be 2 integers\nindicating width and height.\n");
                    }
                }
                else if(!strcmp(argv[i], "-whereami"))
                {
                    char* real_path = realpath(".", NULL);
                    printf("%s/\n", real_path);
                }
                else
                {
                    printf("\nUndefined flag: %s\n", argv[i]);
                }
            }
        }

        if(execute)
        {
            game_engine engine(MAX_ID, user_defined_quote_path, user_defined_w, user_defined_h);
            engine.run();
        }
    }
    catch(const Conversion_error& ce)
    {
        Exit(1, string(ce.what()));
    }
    catch(const runtime_error& e)
    {
        Exit(1, string(e.what()));
    }
    catch(...)
    {
        Exit(1, "Quit due to: Uncaught unexpected exception.");
    }
//    string current_quote = "Thedhaistyiasuida obvios good cafasdasdasse is multiple supercalifraglitcaxpiadous single imple snow.";
//    auto current_quote_for_display = current_quote;
//    int char_per_row = 21;
//    // begin from the frist line
//
//    auto ptr = current_quote_for_display.begin();
//    while(true)
//    {
//        auto ptr_end = ptr+char_per_row;
//        if(ptr_end >= current_quote_for_display.end())
//        {
//            break;
//        }
//
//        while(*ptr !='\n' and ptr != ptr_end)
//        {
//            ptr++;
//        }
//
//        if(ptr == ptr_end)
//        {   // didn't find a newline in nextline
//            if((*ptr == ' ' and *(ptr-1) != ' ')
//               or (*ptr != ' ' and *(ptr-1) != ' '))
//            {
//                auto temp_ptr = ptr-1;
//                while(*temp_ptr != ' '
//                      and *temp_ptr != '\n'
//                      and temp_ptr != current_quote_for_display.begin())
//                {
//                    temp_ptr--;
//                }
//                if(*temp_ptr == '\n' or temp_ptr == current_quote_for_display.begin())
//                {
//                    int lword_len = 0;
//                    while(*temp_ptr != ' ' and temp_ptr != current_quote_for_display.end())
//                    {
//                        lword_len++;
//                        temp_ptr++;
//                    }
//
//                    ptr = temp_ptr - (lword_len-char_per_row-1);
//                }
//                else
//                {
//                    *temp_ptr = '\n';
//                    ptr = temp_ptr+1;
//                }
//            }
//            else
//            {
//                *(ptr-1) = '\n';
//            }
//        }
//        else
//        {
//            ptr++;
//            continue;
//        }
//    }
//    for(auto i = 0; i< char_per_row; ++i)
//        cout << '-';
//    cout << endl;
//    cout << current_quote_for_display << endl;
}



// char tester
//initscr();
//WINDOW *text_frame;
//text_frame = newwin(25, 140, 10, 10);
//box(text_frame, 0, 0);
//while(true)
//{
//    auto ch = wgetch(text_frame);
//    wmove(text_frame, 0, 0);
//    wclrtobot(text_frame);
//    mvwaddstr(text_frame, 3, 3, to_string(ch).c_str());
//}
