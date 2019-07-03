#include <cstring>
#include <fstream>
#include <iostream>
#include "game_engine.hpp"
#include "tools.hpp"
using namespace std;

int main(int argc, const char* argv[]) {
    try {
        string user_defined_quote_path = realpath(".", NULL);
        int MAX_ID = 3001;
        int user_defined_w = 0;
        int user_defined_h = 0;
        bool execute = false;
        bool shuffle = true;

        if (argc == 1) {  // if no argument passed to typon, start game
            execute = true;
        } else {
            for (auto i = 1; i < argc; ++i) {
                // available flags:
                // -path [address],
                // -resize Integer Integer
                // -noshuffle
                // -new (hidden flag) shouldn't be used by user
                if (!strcmp(argv[i], "-path")) {
                    if (i != argc - 1) {
                        ++i;
                        user_defined_quote_path = argv[i];
                        if (user_defined_quote_path.back() != '/')
                            user_defined_quote_path += "/";
                        execute = true;
                    } else {
                        char* real_path = realpath(".", NULL);
                        printf("Current Quote Path: %s\n", real_path);
                    }
                } else if (!strcmp(argv[i], "-resize")) {
                    bool validInput = true;
                    if (i < argc - 2) {
                        try {
                            user_defined_w = stoi(argv[++i]);
                            user_defined_h = stoi(argv[++i]);
                        } catch (...) {  // stoi failed
                            printf(
                                "Invalid arguments for \"-resize\":\n%s %s\n%s",
                                argv[i + 1], argv[i + 2],
                                "input must be 2 integers\nindicating width "
                                "and height.\n");
                            validInput = false;
                        }
                    } else {
                        // wrong arg size
                        printf(
                            "flag \"-resize\" accepts 2 integer arguments: "
                            "width and "
                            "height\n");
                        validInput = false;
                    }
                    if (validInput) {
                        execute = true;
                    }
                } else if (!strcmp(argv[i], "-noshuffle")) {
                    shuffle = false;
                    execute = true;
                } else if (!strcmp(argv[i], "-new") and (argc == 2)) {
                    // create a script links to the typon binary in the current
                    // directory
                    char* real_path = realpath(".", NULL);
                    string typon_sh_path = string(real_path) + "/typon_run.sh";
                    ofstream newtypon(typon_sh_path);
                    string run_sh_content =
                        ("if [ \"$1\" = \"-path\" ] && [ $# -eq 1 ]; then\n"
                         "\techo \"Typon files location: " +
                         string(real_path) +
                         "\"\n"
                         "elif [ \"$1\" = \"-new\" ]; then\n"
                         "\t:\n"
                         "elif [ \"$1\" != \"\" ]; then\n"
                         "\t" +
                         real_path + "/typon $1 $2 $3 $4 $5 -path " +
                         real_path +
                         "\n"
                         "else\n"
                         "\t" +
                         real_path + "/typon -path " + real_path + "\nfi");
                    newtypon << run_sh_content;
                    newtypon.close();
                    // chmod((typon_sh_path).c_str(), S_IWUSR);
                } else {
                    printf("Typon: undefined flag: %s\n", argv[i]);
                }
            }
        }

        if (execute) {
            game_engine engine(MAX_ID, user_defined_quote_path, user_defined_w,
                               user_defined_h, shuffle);
            engine.run();
        }
    } catch (const Conversion_error& ce) {
        Exit(1, string(ce.what()));
    } catch (const runtime_error& e) {
        Exit(1, string(e.what()));
    } catch (...) {
        Exit(1, "Quit due to: Uncaught unexpected exception.");
    }
}
