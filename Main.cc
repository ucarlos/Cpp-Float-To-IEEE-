/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 08/14/2020 at 05:05 PM
 * 
 * Main.cc
 * 
 * -----------------------------------------------------------------------------
 */

#include "Float.h"
#include <unistd.h>
using namespace std;

bool is_valid_number(std::string &str) {
    int dot_count = 0;
    if (str.length() < 2 or str.empty())
        throw std::runtime_error("Error: String is too small to be used"
                                 " as a argument.");

    // Remove any newline/control characters from the string?
    auto it = str.begin();
    auto end = str.end();
    if (*it == '-' or *it == '+')
        it++;
    
    while (it != end){
        if (*it == '.'){
            dot_count++;
            if (dot_count > 1) return false;
        }
        else if (!isdigit(*it)) return false;

        it++;
    }

    return true;
}

void print_instructions() {
    print_dash_line();
    std::cout << "This program evaluates a value with Single Precision"
              << " (\"float\")\n"
              << "or with Double Precision (\"double\")\n\n";
    
    std::cout << "Enter either \"float\" or \"double\". If unsure, "
              << "enter \"double\".\n";
}


void compute() {
    print_instructions();
    FloatNumber fn;
    cin >> fn;
    cout << fn;
}

void parse_options(int argc, char *arg[]) {
    int option;
    bool is_double;
    double value;

    while ((option = getopt(argc, arg, "d:f")) != -1) {
        switch (option) {

        case 'd':
            is_double = true;
            value = atof(optarg);
            break;
        case 'f':
            is_double = false;
            value = atof(optarg);
            break;
        default:
            cerr << "Usage: ./FloatToIEEE [-d double] [-f float] [value]"
                 << endl;
            exit(EXIT_FAILURE);
        }
    }

    if (optind > argc) {
        cerr << "Expected a real number argument after options.." << endl;
        exit(EXIT_FAILURE);
    }

    // Now create a FloatNumber:

    FloatNumber float_num = (is_double) ? FloatNumber{"double", value}
                                        : FloatNumber{"float" , value};
    cout << float_num << endl;
}

int main(int argc, char *argv[]) {
    if (argc < 2)
        compute();
    else
        parse_options(argc, argv);
}
