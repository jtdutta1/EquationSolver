#include "argparse.h"
#include "expression_parser.h"
using std::string;
#include<cstring>
#include<iostream>
using std::cout;

Args parse_args(int argc, char* argv[]) {
    /* Parse user arguments.*/
    Args obj; 
    // Check if there are more than 1 argument
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "--fname") == 0) {
                // check if the argument is a filename checker
                if (i + 1 < argc) {
                    obj.fname = string(argv[++i]);
                } else {
                    throw ArgParseException("--fname requires a filename");
                }
            } else {
                string temp = string(argv[i]);
                if (match_expression(temp)) {
                    obj.eq = temp;
                } else {
                    throw ArgParseException((std::string("Invalid argument/equation ") + argv[i] + ".\nCheck for spaces as solve doesn't support them yet.").c_str());
                }
            }
        }
    } else {
        display_usage();
    }
    return obj;
}

void display_usage() {
    cout << "solve: Pass an mathematical expression to get the result for" << '\n';
    cout << "Usage: solve [equation] [--fname filname]" << '\n';
    cout << "       [equation should not have any space in between the terms]" << '\n';
}