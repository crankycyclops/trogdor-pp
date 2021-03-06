#include <cerrno>
#include <cstring>
#include <cctype>
#include <string>
#include <stack>
#include <fstream>
#include <sstream>

#include <trogdor/exception/parseexception.h>
#include <trogdor/parser/parsers/inform7/lexer.h>

namespace trogdor {


   void Lexer::open(std::string filename) {

      std::ifstream file;
      std::stringstream s;

      file.open(filename);

      if (file.fail()) {
         throw ParseException(
            std::string("failed to open") + filename + ": " + strerror(errno)
         );
      }

      s << file.rdbuf();
      source = s.str();

      // Reset state each time a new file is loaded
      sourceIndex = 0;
      sourceLine = 1;
      while (!tokenBuffer.empty()) {
         tokenBuffer.pop_back();
      }

      skipWhitespace(true);
   }

   /**************************************************************************/

   Token Lexer::next() {

      Token t;

      if (!tokenBuffer.empty()) {
         t = tokenBuffer.back();
         tokenBuffer.pop_back();
      }

      else {

         skipWhitespace();

         // This way the token knows what line it came from (necessary for
         // reporting errors)
         t.lineno = sourceLine;

         if (sourceIndex < source.length()) {

            std::string terminator = getSentenceTerminator();

            if (terminator.length()) {
               t.value = terminator;
               t.type = SENTENCE_TERMINATOR;
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
                     kinds, properties, and adjectives. If no match is found,
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
                  std::string potentialWord;
                  std::stack<std::pair<int, int>> tokenSourceIndicesAndLines;

                  for (int compoundWordLength = 0;
                  compoundWordLength < maxCompoundWordLength; compoundWordLength++) {

                     potentialWord += (compoundWordLength > 0 ? " " : "");
                     potentialWord += getWord();
                     tokenSourceIndicesAndLines.push({sourceIndex, sourceLine});

                     sourceIndex++;

                     if (0 != getSentenceTerminator().length()) {
                        break;
                     }
                  }

                  while (tokenSourceIndicesAndLines.size() > 1) {

                     if (
                        // TODO: when I introduce verb synonyms that can be
                        // more than one word, I'll have to include a check for
                        // those, too
                        directions.end() == directions.find(potentialWord) &&
                        kinds.end()    == kinds.find(potentialWord) &&
                        // TODO: also search plural forms of kinds
                        properties.end() == properties.find(potentialWord) &&
                        adjectives.end() == adjectives.find(potentialWord)
                     ) {
                        potentialWord.erase(potentialWord.rfind(' '));
                        tokenSourceIndicesAndLines.pop();
                     }

                     else {
                        break;
                     }
                  }

                  auto prevSourceIndexAndLine = tokenSourceIndicesAndLines.top();

                  sourceIndex = prevSourceIndexAndLine.first;
                  sourceLine = prevSourceIndexAndLine.second;
                  tokenSourceIndicesAndLines.pop();
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
                  throw ParseException(std::string("illegal character '") +
                     source.at(sourceIndex) + "' on line " + std::to_string(sourceLine));
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

   void Lexer::skipWhitespace(bool includeNewlines) {

     while (
        sourceIndex < source.length() &&
        (isspace(source.at(sourceIndex)) || '[' == source.at(sourceIndex)) &&
        ('\n' != source.at(sourceIndex) || includeNewlines)
     ) {

         if ('\n' == source.at(sourceIndex)) {
            sourceLine++;
         }

         // Count comments as whitespace
         if ('[' == source.at(sourceIndex)) {
            for (; sourceIndex < source.length() && ']' != source.at(sourceIndex); sourceIndex++) {
               if ('\n' == source.at(sourceIndex)) {
                  sourceLine++;
               }
            }
         }

         sourceIndex++;
      }
   }

   /**************************************************************************/

   std::string Lexer::getSentenceTerminator() {

      std::string terminator = "";

      // We've already reached EOF
      if (sourceIndex >= source.length()) {
         return terminator;
      }

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

   std::string Lexer::getQuotedString() {

      std::string quotedString;

      // Move past first quotation mark
      sourceIndex++;

      for (; sourceIndex < source.length() && '"' != source.at(sourceIndex);
      sourceIndex++) {
         quotedString += source.at(sourceIndex);
      }

      // If a period appears at the end of the quoted string, it should be
      // treated as a SENTENCE_TERMINATOR and returned when it's time to retrieve
      // the next token.
      if ('.' == quotedString.back()) {

         tokenBuffer.push_front({".", SENTENCE_TERMINATOR, sourceLine});

         size_t prevSourceLine = sourceLine;
         size_t prevSourceIndex = sourceIndex;

         sourceIndex++;

         // Okay, so what's going on here is, I might, after pushing a phrase
         // terminator due to a period at the end of the quoted string, then lex
         // in the next token another phrase terminator due to a new paragraph
         // (two newlines.) I attempt to skip ahead of this possible second
         // phrase terminator so we can ignore it, and if it turns out there's
         // no phrase terminator but another token that needs to be lexed
         // instead, we restore our previous state and continue on our merry way.
         if (!getSentenceTerminator().length()) {
            sourceLine = prevSourceLine;
            sourceIndex = prevSourceIndex;
         }
      }

      return quotedString;
   }

   /**************************************************************************/

   std::string Lexer::getWord() {

      std::string word;

      for (; sourceIndex < source.length() && isWordChar(source.at(sourceIndex));
      sourceIndex++) {
         word += source.at(sourceIndex);         
      }

      sourceIndex--;
      return word;
   }
}
