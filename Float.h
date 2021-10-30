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

//------------------------------------------------------------------------------
// Enums
//------------------------------------------------------------------------------

/*
 * Floating Number Status:
 * INVALID:      Constructor Invariant
 * NORMALIZED:   Floating Value (Exponent not 0 or 255(2047 for Double))
 * DENORMALIZED: Floating Value (Exponent is 0)
 * Special Case: Floating Value (Exponent is 255 or 2047))
 *
*/
enum Floating_Number_Status {
    INVALID = -1, NORMALIZED = 0, DENORMALIZED, SPECIAL_CASE
};

/*
 * Normalization Status:
 * INITIALIZED:   Initial value
 * NOT_SEPERATED: Byte Representation has been created, but Number has not
 *                been separated.
 * SEPERATED:     Number is ready to be classified.
*/
enum Normalization_Status {
    INITIALIZED = 0, NOT_SEPERATED, SEPERATED
};



//------------------------------------------------------------------------------
// FloatNumber class
//------------------------------------------------------------------------------
class FloatNumber {
public:
    FloatNumber() {
        this->value.double_val = 0;
		this->value.float_val = 0;
		generate_number();
    }
    
    explicit FloatNumber(double value) {
		this->value.double_val = value;
		this->isDouble = true;
		generate_number();
    }
    
    bool is_double() const { return isDouble; }
    FloatNumber(std::string type, double value);

	// Copy Constructor
    FloatNumber(const FloatNumber &fn) =default;

	// Copy Assignment
    FloatNumber& operator=(const FloatNumber &fn) =default;
    
    void change_type(std::string &type);
    void change_value(double new_value);

private:
    long double signficand_val{0};
    uint64_t sign_val{0};
    uint64_t exponent_val{0};
    uint64_t fractional{0};
    uint64_t byte_rep{0};
    
    union datatype_value {
		double double_val;
		float float_val;
    } value;
    
    int32_t weighed_bias{0};
    enum Floating_Number_Status float_status{INVALID};
    enum Normalization_Status norm_status{INITIALIZED};
    bool isDouble{false};

    // Operator Overloading
    friend std::istream& operator>>(std::istream &is, FloatNumber &fn);
    friend std::ostream& operator<<(std::ostream &os, const FloatNumber &fn);

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
// Function declarations
//-------------------------------------------------------------------------------

void Reverse_Bit_Representation(std::string &str);

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------
/**
 * Convert a string to lowercase.
 * @param str
 */
inline void string_to_lower(std::string &str){
    for (char &i : str)
		i = tolower(i);
}

//------------------------------------------------------------------------------
// Print declarations
//------------------------------------------------------------------------------
void print_instructions();

void print_dash_line(std::ostream &os);
void print_dash_line();




//------------------------------------------------------------------------------
// Input Declarations
//------------------------------------------------------------------------------
bool is_valid_number(std::string &str);

#endif

