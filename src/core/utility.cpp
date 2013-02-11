#include <string>
#include <cstring>
#include <cctype>

using namespace std;


/*
   Converts a string to lowercase.

   Input: string
   Output: lowercase version of string
*/
string strToLower(string str);


string strToLower(string str) {

   for (int i = 0; i < strlen(str.c_str()); i++) {
      str[i] = tolower(str[i]);
   }

   return str;
}

