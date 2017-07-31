#ifndef INTERPOLATE_HPP
#define INTERPOLATE_HPP
#include<sstream>
#include<string>
#include <iostream>
#include <typeinfo>
#include <exception>

namespace cs540 {
using namespace std;
struct WrongNumberOfArgs : public std::exception { };

class Helper {
	public:
		// Assign string buffer to Ostream
		Helper() : outputStr ( new std::ostream(&stringBuffer) ) { }
		// Returns string buffer
		std::stringbuf& getBuff() { return stringBuffer; }	
		// Returns STD::OStream
		std::ostream* getStdStream() { return outputStr; }
		// Flush STD::STREAM
		void flushStdStream() { 
			stringBuffer.str(""); 
			stringBuffer.str().clear(); 
			outputStr->flush(); 
			outputStr->flags(f);
		}
		// Free resources
		~Helper() { delete outputStr; }

		template <typename T> Helper& operator<<(const T& t) {	
			*outputStr << t;
			return *this;
		}
	private:
		ios::fmtflags f;
		std::ostream* outputStr = nullptr;
		std::stringbuf stringBuffer;	
};
Helper helper; // Helper Object

/* force the instantiation of manipulators */
std::string ffr(ostream& (*m)(ostream&)) { 	
	std::stringstream out;
	out << m;
	return out.str();
}

// Overload std::stringstream operator and clears buffer and delete std::ostream object
std::stringstream& operator<<(std::stringstream& stringStr, Helper& helper) {
	//stringStr.flags (helper.getStdStream()->flags());
	if(helper.getBuff().str() != "") {
		stringStr << helper.getStdStream()->rdbuf();	
	}	
	helper.flushStdStream();	
	return stringStr;
}

// Overload std::ostream operator and clears buffer and delete std::ostream object
std::ostream& operator<<(std::ostream& outStream, Helper& helper) { 
	outStream << helper.getStdStream()->rdbuf();
	helper.flushStdStream();	
	return outStream;
}

/* Check if for % sign there is argument and Raw string manipulation (replace '\%' with '%')*/
std::size_t isArgumentsCorrect(std::string& r_str) {	
	std::size_t index = 0;
	while((index = r_str.find("%", index)) != -1) {
		if(index == 0 || r_str[index-1] != '\\') { break; }
		r_str.replace(index-1, 2, std::string("%"));
	}
	return index;
}

/* Check for ios Base type (Manipulators) */
template <typename T>
bool isManipulators(T&& t) {
	std::stringstream tempBuffer;
	tempBuffer << t;
	if(typeid(T) == typeid(std::ios_base& (*)(std::ios_base&)) || 
				tempBuffer.str().empty() ) {
		return true;
	}
	return false;
}

/* Update ostream and raw string */
template <typename T>
void print(std::string& r_str, std::size_t index, T&& t) {
	if(index > 0) {	helper << r_str.substr(0, index); }
	helper << t; 
	r_str = r_str.substr(index + 1);
}

Helper& Interpolate(std::string r_str) { 
	std::size_t index = 0;
	if(r_str == "") { return helper; }
	while((index = r_str.find("%", index)) != -1) {
		if(index != 0 && r_str[index-1] == '\\') { 
			r_str.replace(index-1, 2, std::string("%"));
		} else {
			helper.flushStdStream();
			throw WrongNumberOfArgs();
		}
	}
	return (helper << r_str); 
}

template <typename T, typename... Ts>
Helper& Interpolate(std::string r_str, T&& t) {	
	std::size_t index = 0;
	if(isManipulators(t)) {
		helper << t;
		return Interpolate(r_str);
	}
	if((index = isArgumentsCorrect(r_str)) == -1) {
		helper.flushStdStream();
		throw WrongNumberOfArgs();
	}	
	print(r_str, index, t);
	return Interpolate(r_str);
}

template <typename T, typename... Ts>
Helper& Interpolate(std::string r_str, T&& t, Ts&&... rest) {
	std::size_t index = 0;	
	if(isManipulators(t)) {
		helper << t;
		return Interpolate(r_str, rest...);
	}
	if((index = isArgumentsCorrect(r_str)) == -1) {
		helper.flushStdStream();
		throw WrongNumberOfArgs();
	}	
	print(r_str, index, t);
	return Interpolate(r_str, rest...);
}
}
#endif
