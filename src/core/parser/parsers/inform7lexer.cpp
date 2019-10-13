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
      }

      else {

         skipWhitespace();

         // This way the token knows what line it came from (useful for
         // reporting errors)
         t.lineno = sourceLine;

         if (sourceIndex < source.length()) {

            string terminator = getSentenceTerminator();

            if (terminator.length()) {
               t.value = terminator;
               t.type = PHRASE_TERMINATOR;
            }

            else if (sourceIndex < source.length()) {

               if (';' == source.at(sourceIndex)) {
                  t.value += source.at(sourceIndex);
                  t.type = SEMICOLON;
               }

               else if (':' == source.at(sourceIndex)) {
                  t.value += source.at(sourceIndex);
                  t.type = COLON;
               }

               else if (',' == source.at(sourceIndex)) {
                  t.value += source.at(sourceIndex);
                  t.type = COMMA;
               }

               else if ('"' == source.at(sourceIndex)) {
                  t.value = getQuotedString();
                  t.type = QUOTED_STRING;
               }

               else if (isWordChar(source.at(sourceIndex))) {

                  /*
                     Okay, so let me explain the apparent yuckiness... Some
                     words are actually composites of more than one word. For
                     example: "player's carryall" is treated as one word, but is
                     actually composed of two. This code starts by temporarily
                     lexing as many words as it can before the end of a
                     statement (or before the maximum depth is reached) and
                     attempts to match the result to entries in directions,
                     classes, properties, and adjectives. If no match is found,
                     we truncate the last single word from the compound and try
                     matching again. This continues until either a match is
                     found or we reach a base case length of 1, in which case
                     it's just a single non-compound word.

                     Should I clean this up a little by moving it into a
                     separate function? Probably. Am I just grateful it works
                     and not willing to risk breaking it by refactoring? Hell
                     yeah.

                     Until I decide to refactor this, I'll conveniently offset
                     the awkward block of code using comments.
                  */
                  // BEGIN AWKWARD BLOCK OF CODE
                  string potentialWord;
                  stack<int> tokenSourceIndices;

                  for (int compoundWordLength = 0;
                  compoundWordLength < maxCompoundWordLength; compoundWordLength++) {

                     potentialWord += (compoundWordLength > 0 ? " " : "");
                     potentialWord += getWord();
                     tokenSourceIndices.push(sourceIndex);

                     sourceIndex++;

                     if (0 != getSentenceTerminator().length()) {
                        break;
                     }
                  }

                  while (tokenSourceIndices.size() > 1) {

                     if (
                        // TODO: when I introduce verb synonyms that can be
                        // more than one word, I'll have to include a check for
                        // those, too
                        directions.end() == directions.find(potentialWord) &&
                        classes.end()    == classes.find(potentialWord) &&
                        properties.end() == properties.find(potentialWord) &&
                        adjectives.end() == adjectives.find(potentialWord)
                     ) {
                        potentialWord.erase(potentialWord.rfind(' '));
                        tokenSourceIndices.pop();
                     }

                     else {
                        break;
                     }
                  }

                  sourceIndex = tokenSourceIndices.top();
                  tokenSourceIndices.pop();
                  t.value = potentialWord;
                  // END AWKWARD BLOCK OF CODE

                  if (isEquality(t.value)) {
                     t.type = EQUALITY;
                  }

                  else if (isArticle(t.value)) {
                     t.type = ARTICLE;
                  }

                  else if (0 == strToLower(t.value).compare("and")) {
                     t.type = AND;
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

         else {
            t.type = SOURCE_EOF;
         }
      }

      currentToken = t;
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

