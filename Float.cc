/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 08/14/2020 at 02:38 PM
 * 
 * Float.cc
 * 
 * -----------------------------------------------------------------------------
 */

#include "./Float.h"

//------------------------------------------------------------------------------
// Float_Number public definitions
//------------------------------------------------------------------------------

Float_Number::Float_Number(std::string type, double value){
    // Check if string is either float or double
    // throw exception otherwise
    string_to_lower(type);
    if (!(type == "float" || type == "double"))
	throw std::runtime_error("Error: Float_Number() must be called with "
				 "a type argument that is either \"double\" "
				 " or \"float\" .");
    if (type == "float"){
	this->value.float_val = static_cast<float>(value);
	this->isDouble = false;
    }
    else{
	this->value.double_val = value;
	this->isDouble = true;
    }
    
    generate_number();
}

void Float_Number::change_type(std::string &type){
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

void Float_Number::change_value(double new_value){
    if (this->isDouble)
	this->value.double_val = new_value;
    else
	this->value.float_val = static_cast<float>(new_value);

    // Now regenerate number:
    generate_number();
}

//------------------------------------------------------------------------------
// Float_Number private definitions
//------------------------------------------------------------------------------
void Float_Number::create_bit_representation(){
    std::stringstream ss;
     
    byte_pointer address = (byte_pointer) &this->value;
    
    int32_t loop_end = (this->isDouble) ? DOUBLE_LEN : FLOAT_LEN;
    ss >> std::hex >> std::setw(2);
    
    for (int32_t i = 0; i < loop_end; i++)
	ss << static_cast<unsigned>(address[i]);
	
    std::string str = ss.str();
    bool check_endian = is_little_endian();
    
    if (check_endian)
	Reverse_Bit_Representation(str);
    
    ss.str(str);
    uint64_t temp;
    ss >> std::hex >> temp;

    this->byte_rep = temp;
    this->norm_status = Not_Separated;
}

void Float_Number::seperate_number(){
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

    this->float_status = (this->exponent_val == temp_exponent_val) ? (Special_Case)
	: ((!this->exponent_val) ? Denormalized : Normalized);
    
    if (this->float_status != Special_Case){
	int32_t temp_bias = (this->isDouble) ? DOUBLE_EXPONENT_BIAS : FLOAT_EXPONENT_BIAS;
	this->weighed_bias =
	    (!this->float_status) ? ((int32_t)this->exponent_val - temp_bias)
	    : (1 - temp_bias);
    }

    this->norm_status = Separated;
}

void Float_Number::generate_significand(){
    this->signficand_val = (this->isDouble)? significandl(value.double_val)
	: significandl(value.float_val);

}

std::istream& operator>>(std::istream &is, Float_Number &fn){
    return is;
}

std::ostream& operator<<(std::ostream &os, const Float_Number &fn){
    std::ostringstream o;
    o << "Floating Point Information: " << std::endl;
    o << "Initial Number: " << std::setprecision(4);
    if (fn.isDouble)
	o << fn.value.double_val << " (Double Precision)\n";
    else
	o << fn.value.float_val << " (Single Precision)\n";

    o << std::hex << std::showbase;
    o << "Bit Representation: " << fn.byte_rep << "\n";
    o << std::dec << "Sign Value: " << fn.sign_val;
    o << ((!fn.sign_val)? " (Positive)" : " (Negative)") << "\n";
    o << "Exponent Value: " << fn.exponent_val << "\n";
    o << "Fractional Value: " << std::hex << fn.fractional << "\n";
    
    o << "Floating Point Classification: ";
    if (fn.float_status == Special_Case)
	o << "Special Case (Exponent field is all ones)\n";
    else if (fn.float_status == Denormalized)
	o << "Denormalized (Exponent field is all zeros)\n";
    else
	o << "Normalized\n";

    if (fn.float_status != Special_Case){
	o << "Weighed Bias value: " << fn.weighed_bias << "\n";
	o << "So in M * 2^E form, the number " << std::setprecision(4);
	
	if (fn.isDouble) o << fn.value.double_val;
	else o << fn.value.float_val;

	o << std::dec
	  << " is represented as "
	  << fn.signficand_val << " * 2^" << fn.weighed_bias
	  << "\n";
	
	o << "When multiplied, the result is "
	  << fn.signficand_val * (1 << fn.weighed_bias)
	  << "\n";
    }
    else
	o << "Special Case value: "
	  << (!(fn.fractional) ?
	      (!(fn.sign_val) ? "Positive Infinity" : "Negative Infinity")
	      : "Not A Number (NaN)")
	  << "\n";
    
    return os << o.str();
}


//-------------------------------------------------------------------------------
// Helper functions
//-------------------------------------------------------------------------------
void Reverse_Bit_Representation(std::string &str){
    auto limit = str.length() / 2;
    auto end_point = str.length();
    char temp1, temp2;
    for (std::size_t i = 0; i < limit; i+= 2){
	end_point -= 2;

	temp1 = str[i];
	temp2 = str[i + 1];
	// Move endpoint and endpoint + 1 to i and i + 1
	str[i] = str[end_point];
	str[i + 1] = str[end_point + 1];

	// Now move copies of i and i + 1 to endpoint and endpoint + 1
	str[end_point] = temp1;
	str[end_point + 1] = temp2;
    }
}
