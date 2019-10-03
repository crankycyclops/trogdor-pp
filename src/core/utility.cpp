#include <string>
#include <cstring>
#include <cctype>
#include <regex>
#include <algorithm> 
#include <functional>
#include <locale>

using namespace std;


/*
   Converts a string to lowercase.

   Input: string
   Output: lowercase version of string
*/
string strToLower(string str);

/*
   Trims whitespace from the left of a string.  Shamelessly stolen from:
   http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

   Input: reference to string
   Output: reference to the same string, which has been modified
*/
string &ltrim(std::string &s);

/*
   Trims whitespace from the right of a string.  Shamelessly stolen from:
   http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

   Input: reference to string
   Output: reference to the same string, which has been modified
*/
string &rtrim(std::string &s);

/*
   Trims whitespace from both sides of a string.  Shamelessly stolen from:
   http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

   Input: reference to string
   Output: reference to the same string, which has been modified
*/
string &trim(std::string &s);

/*
   Replaces all instances of search with replace in str. Returns a new string
   and leaves the original unmodified.

   Input:
      String to modify
      Substring search
      Substring replacement

   Output:
      (none)
*/
string replaceAll(const string &str, const string &search, const string &replace);

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


string strToLower(string str) {

   for (int i = 0; i < strlen(str.c_str()); i++) {
      str[i] = tolower(str[i]);
   }

   return str;
}


string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}


string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}


string &trim(std::string &s) {
        return ltrim(rtrim(s));
}


string replaceAll(const string &str, const string &search, const string &replace) {

   string strCopy = string(str);

   for (int i = strCopy.find(search); i != string::npos; i = strCopy.find(search, i + replace.length())) {
      strCopy.replace(i, search.length(), replace);
   }

   return strCopy;
}


bool isValidInteger(const std::string &s) {

   return regex_match(s, regex("^\\-?\\d+$")) ? true : false;
}


bool isValidDouble(const std::string &s) {

   char *end = 0;
   double val = strtod(s.c_str(), &end);
   return end != s.c_str() && *end == '\0' && val != HUGE_VAL;
}

