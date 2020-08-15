/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 08/14/2020 at 05:05 PM
 * 
 * Main.cc
 * 
 * -----------------------------------------------------------------------------
 */

#include "Float.h"

bool is_valid_number(std::string &str){
    int dot_count = 0;
    if (str.length() < 2 or str.empty())
	throw std::runtime_error("Error: String is too small to be used"
				 " as a argument.");

    // Remove any newline/control characters from the string?
    auto it = str.begin();
 
    if (*it == '-' or *it == '+')
	it++;
    
    while (it != str.end()){
	if (*it == '.'){
	    dot_count++;
	    if (dot_count > 1) return false;
	}
	else if (!isdigit(*it)) return false;

	it++;
    }

    return true;

}

void print_instructions(void){
    print_dash_line();
    std::cout << "This program evaluates a value with Single Precision"
	      << " (\"float\")\n"
	      << "or with Double Precision (\"double\")\n\n";
    
    std::cout << "Enter either \"float\" or \"double\". If unsure, "
	      << "enter \"double\".\n";
}

void compute(void){
    std::string str, value;
    print_instructions();
    std::cin >> str;
    string_to_lower(str);
    
    while (!(str == "float" or str == "double")){
	std::cout << "Invalid option. Enter either \"float\" or \"double\".\n";
	std::cin >> str;
    }

    // Now check number:
    std::cout << "Next, enter a number.\n";
    std::cin >> value;
    while (!(is_valid_number(value))){
	std::cout << "Invalid number. Try again." << std::endl;
	std::cin >> value;
    }
    
    std::stringstream ss;
    ss << value;
    double temp;
    ss >> temp;
    
    Float_Number fn(str, temp);
    std::cout << fn;
}

int main(void){
    compute();

}
