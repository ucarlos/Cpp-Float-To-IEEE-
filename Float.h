/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 08/14/2020 at 02:19 PM
 * 
 * Float.h
 * C++ version of Float_to_IEEE
 * -----------------------------------------------------------------------------
 */

#ifndef FLOAT_H
#define FLOAT_H

#include <iostream>
#include <string>
#include <cctype>
#include <cinttypes>
#include <unistd.h>
#include <climits>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <stdexcept>
#include <iomanip>
//------------------------------------------------------------------------------
// Size Macros:
//------------------------------------------------------------------------------

#define FLOAT_SIZE (sizeof(float) * 8)
#define FLOAT_LEN (sizeof(float))
#define FLOAT_EXPONENT_BIAS (((1 << 7) - 1))
#define FLOAT_EXP_LEN (8)
#define FLOAT_FRAC_LEN (23)
#define FLOAT_MAX_EXPONENT_VAL ((1 << 8) - 1)

#define DOUBLE_SIZE (sizeof(double) * 8)
#define DOUBLE_LEN (sizeof(double))
#define DOUBLE_EXPONENT_BIAS (((1 << 10) - 1))
#define DOUBLE_EXP_LEN (11)
#define DOUBLE_FRAC_LEN (52)
#define DOUBLE_MAX_EXPONENT_VAL ((1 << 11) - 1)

//------------------------------------------------------------------------------
// Bit Masks:
//------------------------------------------------------------------------------

#define FLOAT_SIGN_MASK (0x80000000)
#define FLOAT_EXP_MASK (0x7f800000)
#define FLOAT_FRAC_MASK (0x007fffff)
#define DOUBLE_SIGN_MASK (0x8000000000000000UL)
#define DOUBLE_EXP_MASK (0x7ff0000000000000UL)
#define DOUBLE_FRAC_MASK (0x000fffffffffffffUL)

const char window_character = '-';
const int window_size = 80;

// Byte pointer:
// using byte_pointer = unsigned char *;
typedef unsigned char *byte_pointer;

//------------------------------------------------------------------------------
// Enums:
//------------------------------------------------------------------------------

/*
 * Enum Status:
 * Invalid:      Constructor Invariant
 * Normalized:   Floating Value (Exponent not 0 or 255(2047 for Double))
 * Denormalized: Floating Value (Exponent is 0)
 * Special Case: Floating Value (Exponent is 255 or 2047))
 *
*/
enum Floating_Number_Status{
    Invalid = -1, Normalized = 0, Denormalized, Special_Case
};

/*
 * Normalization Status:
 * Initialized: Initial value
 * Not_Separated: Byte Representation has been created, but Number has not
 *                been separated.
 * Separated: Number is ready to be classified.
*/

enum Normalization_Status{
    Initialized = 0, Not_Separated, Separated
};

class Float_Number{
public:
    Float_Number(){
	this->value.float_val = 0;
	generate_number();
    }
    
    explicit Float_Number(double value){
	this->value.double_val = value;
	this->isDouble = true;
	generate_number();
    }
    bool is_double() { return isDouble; }
    Float_Number(std::string type, double value);
    
    Float_Number(const Float_Number &fn) =default; // Copy Constructor
    Float_Number& operator=(const Float_Number &fn) =default; // Copy Assignment
    
    void change_type(std::string &type);
    void change_value(double new_value);

private:
    long double signficand_val{0};
    uint64_t sign_val{0};
    uint64_t exponent_val{0};
    uint64_t fractional{0};
    uint64_t byte_rep{0};
    
    union datatype_value{
	double double_val;
	float float_val;
    } value;
    
    int32_t weighed_bias{0};
    enum Floating_Number_Status float_status{Invalid};
    enum Normalization_Status norm_status{Initialized};
    bool isDouble{false};

    // Operator Overloading
    friend std::istream& operator>>(std::istream &is, Float_Number &fn);
    friend std::ostream& operator<<(std::ostream &os, const Float_Number &fn);

    // Private declarations
    void generate_number() {
	create_bit_representation();
	seperate_number();
	generate_significand();
    }
    
    void create_bit_representation();
    void seperate_number();
    void generate_significand();
};

//-------------------------------------------------------------------------------
// Helper functions
//-------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// is_little_endian(): Check if the machine uses little endian byte ordering
// or not. This determines if the string has to be reversed in order to get
// the correct values.
//------------------------------------------------------------------------------
inline bool is_little_endian(){
    int value = 1;
    char *check = (char *) &value;
    return (*check) != 0;

}

void Reverse_Bit_Representation(std::string &str);

//------------------------------------------------------------------------------
// string_to_lower: convert string to lowercase.
//------------------------------------------------------------------------------
inline void string_to_lower(std::string &str){
    for (char &i : str)
	i = tolower(i);
}

//------------------------------------------------------------------------------
// Print declarations
//------------------------------------------------------------------------------
void print_instructions();

inline void print_dash_line(){
    for (int16_t i = 0; i < window_size; i++)
	std::cout << window_character;
    std::cout << std::endl;
}

//------------------------------------------------------------------------------
// Input Declarations
//------------------------------------------------------------------------------
bool is_valid_number(std::string &str);

#endif

