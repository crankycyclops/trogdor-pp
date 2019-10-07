#include <cerrno>
#include <cstring>
#include <cctype>
#include <string>
#include <stack>
#include <fstream>
#include <sstream>

#include "../../include/exception/parseexception.h"
#include "../../include/parser/parsers/inform7lexer.h"

using namespace std;

namespace trogdor {


   void Inform7Lexer::open(string filename) {

      ifstream file;
      stringstream s;

      file.open(filename);

      if (file.fail()) {
         throw ParseException(
            string("failed to open") + filename + ": " + strerror(errno)
         );
      }

      s << file.rdbuf();
      source = s.str();

      // Reset state each time a new file is loaded
      sourceIndex = 0;
      sourceLine = 1;
      while (!tokenBuffer.empty()) {
         tokenBuffer.pop();
      }
   }

   /**************************************************************************/

   Token Inform7Lexer::next() {

      Token t;

      if (!tokenBuffer.empty()) {
         t = tokenBuffer.top();
         tokenBuffer.pop();
         return t;
      }

      else {

         skipWhitespace();

         if (sourceIndex < source.length()) {

            string terminator = getSentenceTerminator();

            if (terminator.length()) {
               t.value = terminator;
               t.type = SENTENCE_TERMINATOR;
            }

            else if (isPunctuation(source.at(sourceIndex))) {
               t.value += source.at(sourceIndex);
               t.type = PUNCTUATION;
            }

            else if ('"' == source.at(sourceIndex)) {
               t.value = getQuotedString();
               t.type = QUOTED_STRING;
            }

            else if (isWordChar(source.at(sourceIndex))) {

               t.value = getWord();

               if (isEquality(t.value)) {
                  t.type = EQUALITY;
               }

               else if (isArticle(t.value)) {
                  t.type = ARTICLE;
               }

               else {
                  t.type = WORD;
               }
            }

            else {
               throw ParseException(string("illegal character '") +
                  source.at(sourceIndex) + "' on line " + to_string(sourceLine));
            }

            sourceIndex++;
         }

         else {
            t.type = SOURCE_EOF;
         }
      }

      return t;
   }

   /**************************************************************************/

   void Inform7Lexer::skipWhitespace() {

     while (
        sourceIndex < source.length() &&
        (isspace(source.at(sourceIndex)) || '[' == source.at(sourceIndex)) &&
        '\n' != source.at(sourceIndex)
     ) {

         // Count comments as whitespace
         if ('[' == source.at(sourceIndex)) {
            for (; sourceIndex < source.length() && ']' != source.at(sourceIndex);
               sourceIndex++);
         }

         sourceIndex++;
      }
   }

   /**************************************************************************/

   string Inform7Lexer::getSentenceTerminator() {

      string terminator = "";

      if ('.' == source.at(sourceIndex)) {
         terminator += source.at(sourceIndex);
         sourceIndex++;
      }

      skipWhitespace();
      int newLineCount = 0;

      // If there's just one newline, then we have the desired side-effect of
      // skipping over it as whitespace
      while (sourceIndex < source.length() && '\n' == source.at(sourceIndex)) {

         newLineCount++;
         sourceLine++;

         if (newLineCount > 1) {
            terminator += source.at(sourceIndex);
         }

         sourceIndex++;
         skipWhitespace();
      }

      if (terminator.length()) {
         sourceIndex--;
      }

      return terminator;
   }

   /**************************************************************************/

   string Inform7Lexer::getQuotedString() {

      string quotedString;

      // Move past first quotation mark
      sourceIndex++;

      for (; sourceIndex < source.length() && '"' != source.at(sourceIndex);
      sourceIndex++) {
         quotedString += source.at(sourceIndex);
      }

      return quotedString;
   }

   /**************************************************************************/

   string Inform7Lexer::getWord() {

      string word;

      for (; sourceIndex < source.length() && isWordChar(source.at(sourceIndex));
      sourceIndex++) {
         word += source.at(sourceIndex);         
      }

      sourceIndex--;
      return word;
   }
}

