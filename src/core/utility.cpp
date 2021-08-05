#include <string>
#include <cstring>
#include <cctype>
#include <cmath>
#include <regex>
#include <algorithm> 
#include <functional>
#include <locale>
#include <vector>


namespace trogdor {

   /*
      If a string starts with an alphabetic character, capitalize it. Leading
      whitespace is ignored when determining if the first character should be
      capitalized.

      Input: string
      Output: capitalized version of string
   */
   std::string capitalize(std::string str);

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
      Replaces all instances of search with replace in str. Returns a new string
      and leaves the original unmodified.

      Input:
         String to modify
         Substring search
         Substring replacement

      Output:
         (none)
   */
   std::string replaceAll(const std::string &str, const std::string &search, const std::string &replace);

   /*
      Utility method that converts a vector of strings into a comma-delimited list.

      Input:
         Vector of strings (vector<string>)
         Conjunction (string)

      Output:
         Comma-delimited list (string)
   */
   std::string vectorToStr(std::vector<std::string> list, std::string conjunction = "and");

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

   /*
      Returns true if a string contains only ASCII characters and false if not.

      Input: string
      Output: true if string contains only ASCII characters and false if not
   */
  bool isAscii(const std::string &s);


   std::string capitalize(std::string str) {

      size_t i;

      for (i = 0; i < str.length() && isspace(str[i]); i++);

      if (isalpha(str[i])) {
         str[i] = toupper(str[i]);
      }

      return str;
   }


   std::string strToLower(std::string str) {

      // With the proper compiler optimizations in place (passing
      // -DCMAKE_BUILD_TYPE=Release to cmake will do the trick), this should be
      // just as fast as a for-loop (I tested this on large random strings
      // using G++ 9.3 just to make sure), and I think this is a little more
      // "C++ish."" With no compiler optimizations (i.e.
      // -DCMAKE_BUILD_TYPE=Debug), the for-loop wins.)
      std::transform(str.begin(), str.end(), str.begin(), [] (unsigned char c) {
         return std::tolower(c);
      });

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


   std::string replaceAll(const std::string &str, const std::string &search, const std::string &replace) {

      std::string strCopy = std::string(str);

      for (size_t i = strCopy.find(search); i != std::string::npos; i = strCopy.find(search, i + replace.length())) {
         strCopy.replace(i, search.length(), replace);
      }

      return strCopy;
   }


   std::string vectorToStr(std::vector<std::string> list, const std::string &conjunction) {

      std::string str;

      for (unsigned int i = 0; i < list.size(); i++) {

         str += list[i];

         if (list.size() > 1 && i < list.size() - 2) {
            str += ", ";
         }

         else if (i < list.size() - 1) {
            str += " " + conjunction + " ";
         }
      }

      return str;
   }


   bool isValidInteger(const std::string &s) {

      const char *cstr = s.c_str();

      // Empty strings are not considered valid integers.
      if (0 == s.length()) {
         return false;
      }

      // Doesn't match leading 0's because those represent octal numbers and this
      // method is used to validate ordinary 10-based numbers.
      else if ('0' == *cstr && s.length() > 1) {
         return false;
      }

      for (const char *c = cstr; *c != '\0'; c++) {

         // It's okay if there's a positive or negative sign at the beginning
         // of a number.
         if (c == cstr && s.length() > 0 && ('-' == *c || '+' == *c)) {
            continue;
         }

         if (!std::isdigit(*c)) {
            return false;
         }
      }

      return true;
   }


   bool isValidDouble(const std::string &s) {

      // Don't allow numbers with leading 0's because we're validating normal
      // base 10 numbers and numbers with leading 0's are treated by conversion
      // functions as octal. This solution feels lame, and I don't like that
      // it requires me to make a copy of the string. I might revisit this later.
      std::string strCopy = s;
      trim(strCopy);

      if (
         (strCopy.length() > 2 && '-' == strCopy[0] && '0' == strCopy[1] && '.' != strCopy[2]) ||
         (strCopy.length() > 1 && '0' == strCopy[0] && '.' != strCopy[1])
      ) {
         return false;
      }

      char *end = 0;
      double val = strtod(s.c_str(), &end);
      return end != s.c_str() && *end == '\0' && val != HUGE_VAL;
   }

   bool isAscii(const std::string &s) {

      for (size_t i = 0; i < s.length(); i++) {
         if (!isascii(s[i])) {
            return false;
         }
      }

      return true;
   }
}
