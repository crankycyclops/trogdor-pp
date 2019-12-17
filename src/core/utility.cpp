#include <string>
#include <cstring>
#include <cctype>
#include <cmath>
#include <regex>
#include <algorithm> 
#include <functional>
#include <locale>


/*
   Converts a string to lowercase.

   Input: string
   Output: lowercase version of string
*/
std::string strToLower(std::string str);

/*
   Trims whitespace from the left of a string.  Shamelessly stolen from:
   http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

   Input: reference to string
   Output: reference to the same string, which has been modified
*/
std::string &ltrim(std::string &s);

/*
   Trims whitespace from the right of a string.  Shamelessly stolen from:
   http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

   Input: reference to string
   Output: reference to the same string, which has been modified
*/
std::string &rtrim(std::string &s);

/*
   Trims whitespace from both sides of a string.  Shamelessly stolen from:
   http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

   Input: reference to string
   Output: reference to the same string, which has been modified
*/
std::string &trim(std::string &s);

/*
   Checks if a string represents a valid integer.

   Input: string
   Output: true if the string is a valid integer and false if not
*/
bool isValidInteger(const std::string &s);

/*
   Checks if a string represents a valid double. Shamelessly stolen from:
   https://stackoverflow.com/questions/29169153/how-do-i-verify-a-string-is-valid-double-even-if-it-has-a-point-in-it

   Input: string
   Output: true if the string is a valid double and false if not
*/
bool isValidDouble(const std::string &s);


std::string strToLower(std::string str) {

   for (unsigned int i = 0; i < strlen(str.c_str()); i++) {
      str[i] = tolower(str[i]);
   }

   return str;
}


std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}


std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}


std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}


bool isValidInteger(const std::string &s) {

   return std::regex_match(s, std::regex("^\\-?\\d+$")) ? true : false;
}


bool isValidDouble(const std::string &s) {

   char *end = 0;
   double val = strtod(s.c_str(), &end);
   return end != s.c_str() && *end == '\0' && val != HUGE_VAL;
}

