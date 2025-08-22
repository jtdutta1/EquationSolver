#include<iostream>
#include "utils/argparse.h"
#include "utils/expression_parser.h"

using std::cout, std::cin, std::cerr;

int main(int argc, char *argv[]) {
    Args args;
    try{
        args = parse_args(argc, argv);
        cout << "Raw equation:-\n";
        cout << args.eq << '\n';

        // parse equation
        cout << "Tree structure" << '\n';
        Expr exp = parse_expression(args.eq);
        exp.show();
    } catch (const ArgParseException& e) {
        cerr << e.what() << '\n';
    } catch (const ValueNotSupportedException& e) {
        cerr << e.what() << '\n';
    }
    return 0;
}