/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 08/14/2020 at 02:38 PM
 * 
 * Float.cc
 * 
 * -----------------------------------------------------------------------------
 */

#include "./Float.h"
#include <iomanip>

//------------------------------------------------------------------------------
// FloatNumber public definitions
//------------------------------------------------------------------------------

/**
 * Specified Constructor
 * @param type
 * @param value
 */
FloatNumber::FloatNumber(std::string type, double value){
    // Check if string is either float or double
    // throw exception otherwise
    string_to_lower(type);
    if (!(type == "float" || type == "double"))
        throw std::runtime_error("Error: FloatNumber() must be called with "
                                 "a type argument that is either \"double\" "
                                 " or \"float\" .");
    if (type == "float") {
        this->value.float_val = static_cast<float>(value);
        this->isDouble = false;
    }
    else {
        this->value.double_val = value;
        this->isDouble = true;
    }
    
    generate_number();
}


/**
 * Change the type of number from float to double and
 * vice versa. This causes the number to be regenerated again.
 * 
 * @param type The type of number to convert to.
 */
void FloatNumber::change_type(std::string &type){
    string_to_lower(type);
    if (!(type == "float" || type == "double")){
        std::cerr << "Cannot call change_type() with a type that is not "
                  << "\"float\" or \"double\". Aborting."
                  << "\n";
        return;     
    }

    double temp;
    if (type == "float" && this->isDouble){
        temp = this->value.double_val;
        // Might be impossible, but I'll check it later.
        this->value.double_val = 0;
        this->isDouble = false;
        this->value.float_val = static_cast<float>(temp);
    }
    else if (type == "double" && !this->isDouble){
        temp = this->value.float_val;
        // Will this clear the union?
        this->value.float_val = 0;
        this->isDouble = true;
        this->value.double_val = temp;
    }
    else
        return;

    // Now regenerate number.
    generate_number();
}


/**
 * Change the value of the number. This causes
 * the number to be regenerated again.
 * 
 * @param new_value
 */
void FloatNumber::change_value(const double new_value){
    if (this->isDouble)
        this->value.double_val = new_value;
    else
        this->value.float_val = static_cast<float>(new_value);

    // Now regenerate number:
    generate_number();
}

//------------------------------------------------------------------------------
// FloatNumber private definitions
//------------------------------------------------------------------------------
/**
 * Store the bit representation in a uint64_t value by reinterpreting
 * a float/double value to a uint64_t reference.
 * 
 */
void FloatNumber::create_bit_representation(){
    // This is an easier way than using an unsigned char to get the bit
    // representation. This way, I don't have to check if for Little/Big Endian.
    
    uint64_t temp = reinterpret_cast<uint64_t &>(this->value);      
    this->byte_rep = temp;
    this->norm_status = NOT_SEPERATED;
}

/**
 * Seperate the bit representation into its seperate sign, exponent, and
 * mantissa values depending on whether single or double precision is used.
 */
void FloatNumber::seperate_number(){
    // Set the sign, exponent, and fractional parts for both doubles and floats.
    
    if (this->isDouble){
        uint64_t bit_rep = this->byte_rep;
    
        this->sign_val = (bit_rep & DOUBLE_SIGN_MASK) >> (DOUBLE_EXP_LEN + DOUBLE_FRAC_LEN);
        this->exponent_val = (bit_rep & DOUBLE_EXP_MASK) >> (DOUBLE_FRAC_LEN);
        this->fractional = (bit_rep & DOUBLE_FRAC_MASK);
    }
    else{
        uint32_t bit_rep = static_cast<uint32_t>(this->byte_rep);

        this->sign_val = (bit_rep & FLOAT_SIGN_MASK) >> (FLOAT_EXP_LEN + FLOAT_FRAC_LEN);
        this->exponent_val = (bit_rep & FLOAT_EXP_MASK) >> (FLOAT_FRAC_LEN);
        this->fractional = (bit_rep & FLOAT_FRAC_MASK);
    }

    uint64_t temp_exponent_val = (this->isDouble) ? DOUBLE_MAX_EXPONENT_VAL
        : FLOAT_MAX_EXPONENT_VAL;

    this->float_status = (this->exponent_val == temp_exponent_val) ? (SPECIAL_CASE)
        : ((!this->exponent_val) ? DENORMALIZED : NORMALIZED);
    
    if (this->float_status != SPECIAL_CASE){
        int32_t temp_bias = (this->isDouble) ? DOUBLE_EXPONENT_BIAS : FLOAT_EXPONENT_BIAS;
        this->weighed_bias =
            (!this->float_status) ? ((int32_t)this->exponent_val - temp_bias)
            : (1 - temp_bias);
    }

    this->norm_status = SEPERATED;
}

/**
 * Generate the significand. This depends on significandl, which is
 * non-standard.
 * TODO: Generate a platform independent version.
 */
void FloatNumber::generate_significand(){
    this->signficand_val = (this->isDouble)? significandl(value.double_val)
                                           : significandl(value.float_val);

}

/**
 * Your typical istream operator. Reads to a FloatNumber object.
 * @param is istream to read from
 * @param fn An Floating Number object
 * @returns the istream parameter.
 */
std::istream& operator>>(std::istream &is, FloatNumber &fn){

    std::string floating_type;

    is >> floating_type;
    string_to_lower(floating_type);

    while (!(floating_type == "double" || floating_type == "float")) {
        std::cerr << "INVALID option. Enter either \"float\" or \"double\".\n";
        is >> floating_type;
    }

    // Now compute the value:
    std::string string_value;
    is >> string_value;

    while (!is_valid_number(string_value)) {
        std::cerr << "INVALID number. Try again." << std::endl;
        is >> string_value;
    }

    // Everything went well, now create an object:
    static std::istringstream iss;
    iss.str(string_value);

    double value;
    iss >> value;
    FloatNumber number{floating_type, value};
    fn = number;

    return is;
}


/**
 * Prints out the contents of a FloatNumber to an ostream.
 *
 * @param os Ostream to print contents on.
 * @param fn A constant FloatNumber object
 */
std::ostream& operator<<(std::ostream &os, const FloatNumber &fn){
    std::ostringstream ostringstream;
	print_dash_line(ostringstream);
    ostringstream << "Floating Point Information: " << std::endl;
	print_dash_line(ostringstream);
	
    ostringstream << "Initial Number: " << std::setprecision(4);
    if (fn.isDouble)
        ostringstream << fn.value.double_val << " (Double Precision)\n";
    else
        ostringstream << fn.value.float_val << " (Single Precision)\n";

    ostringstream << std::hex << std::showbase;
    ostringstream << "Bit Representation (In Hexadecimal): " << fn.byte_rep << "\n";
    ostringstream << std::dec << "Sign Value: " << fn.sign_val;
    ostringstream << ((!fn.sign_val) ? " (Positive)" : " (Negative)") << "\n";
    ostringstream << "Exponent Value (Decimal): " << fn.exponent_val << "\n";
    ostringstream << "Fractional Value (In Hexadecimal): " << std::hex << fn.fractional << "\n";
    
    ostringstream << "Classification: ";
    if (fn.float_status == SPECIAL_CASE)
        ostringstream << "Special Case (Exponent field is all ones)\n";
    else if (fn.float_status == DENORMALIZED)
        ostringstream << "DENORMALIZED (Exponent field is all zeros)\n";
    else
        ostringstream << "NORMALIZED (Exponent fields is neither all ones or all zeros)\n";

    if (fn.float_status != SPECIAL_CASE){
        ostringstream << "Weighed Bias value (Decimal): "
                      << std::dec << fn.weighed_bias << "\n";

		print_dash_line(ostringstream);
        ostringstream << "So in M * 2^E form, the number " << std::setprecision(4);
		ostringstream << ((fn.isDouble) ? fn.value.double_val : fn.value.float_val);

        ostringstream << std::dec
                      << " is represented as "
                      << fn.signficand_val << " * 2^" << fn.weighed_bias
                      << "\n";
    
        ostringstream << "When multiplied, the result is "
                      << std::setprecision(24)
                      << fn.signficand_val * (1 << fn.weighed_bias)
          << "\n";
    }
    else {
		print_dash_line(ostringstream);
        ostringstream << "Special Case value: "
                      << (!(fn.fractional) ? (!(fn.sign_val) ? "Positive Infinity" : "Negative Infinity")
                               : "Not A Number (NaN)")
                      << "\n";
	}

    // Additional Information:
	print_dash_line(ostringstream);
    ostringstream << "Additional Info:\n"
                  << "Object size: " << sizeof(FloatNumber) << " bytes\n";
	

    return os << ostringstream.str();
}

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

/**
 * Print a dashed line the size of the window size.
 * The character is defined by the window_character variable.
 * @param os
 */
void print_dash_line(std::ostream &os) {
	for (uint8_t i = 0; i < window_size; i++)
		os << window_character;
	os << std::endl;
}

void print_dash_line() {
	print_dash_line(std::cout);
}	
