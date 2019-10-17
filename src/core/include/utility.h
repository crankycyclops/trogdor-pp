#ifndef UTILITY_H
#define UTILITY_H


#include <string>
#include <cctype>
#include <vector>

/* macro to clip a value within a given range */
#define CLAMP(V, MIN, MAX)  (((V) > (MAX)) ? (MAX) : (((V) < (MIN)) ? (MIN) : (V)))

using namespace std;


/*
   Converts a string to lowercase.

   Input: string
   Output: lowercase version of string
*/
extern string strToLower(string str);


/*
   Trims whitespace from the left of a string.  Shamelessly stolen from:
   http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

   Input: reference to string
   Output: reference to the same string, which has been modified
*/
extern string &ltrim(std::string &s);

/*
   Trims whitespace from the right of a string.  Shamelessly stolen from:
   http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

   Input: reference to string
   Output: reference to the same string, which has been modified
*/
extern string &rtrim(std::string &s);

/*
   Trims whitespace from both sides of a string.  Shamelessly stolen from:
   http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

   Input: reference to string
   Output: reference to the same string, which has been modified
*/
extern string &trim(std::string &s);

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
   Utility method that converts a vector of strings into a comma-delimited list.

   Input:
      Vector of strings (vector<string>)

   Output:
      Comma-delimited list (string)
*/
string vectorToStr(vector<string> list);

/*
   Checks if a string represents a valid integer.

   Input: string
   Output: true if the string is a valid integer and false if not
*/
extern bool isValidInteger(const std::string &s);

/*
   Checks if a string represents a valid double. Shamelessly stolen from:
   https://stackoverflow.com/questions/29169153/how-do-i-verify-a-string-is-valid-double-even-if-it-has-a-point-in-it

   Input: string
   Output: true if the string is a valid double and false if not
*/
extern bool isValidDouble(const std::string &s);


#endif

