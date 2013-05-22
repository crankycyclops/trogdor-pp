#ifndef UTILITY_H
#define UTILITY_H


#include <string>
#include <cctype>

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


#endif

