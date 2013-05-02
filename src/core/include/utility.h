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


#endif

