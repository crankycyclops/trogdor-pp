#ifndef INFORM7LEXER_H
#define INFORM7LEXER_H


#include <string>
#include <stack>


using namespace std;

namespace trogdor {


   enum TokenType {
      PUNCTUATION   = 1,
      NEWLINE       = 2,
      WORD          = 3,
      ARTICLE       = 4,
      EQUALITY      = 5,
      QUOTED_STRING = 6,
      SOURCE_EOF    = 7
   };

   struct Token {
      string value;
      TokenType type;
   };

   /**************************************************************************/

   class Inform7Lexer {

      private:

         // Inform 7 source code
         string source;

         // Index to next unparsed character in source
         int sourceIndex = 0;

         // Current line number in the source code
         int sourceLine = 1;

         // Tokens that were read ahead to make a decision and then "pushed back"
         stack<Token> tokenBuffer;

         /*
            Returns true if the string is a form of the verb to be and false if
            not.

            Input:
               Word (string)

            Output:
               True if word is a form of the verb "to be" and false if not (bool)
         */
         inline bool isEquality(string s) const {

            return (0 == s.compare("is") || 0 == s.compare("are")) ? true : false;
         }

         /*
            Returns true if the string is an article and false if not.

            Input:
               Word (string)

            Output:
               True if string is an article and false if not (bool)
         */
         inline bool isArticle(string s) const {

            return (0 == s.compare("a") || 0 == s.compare("an") || 0 == s.compare("the")) ? true : false;
         }

         /*
            Returns true if the given character is valid in a word and false
            otherwise.

            Input:
               Character (char)

            Output:
               True if c is a valid word character and false otherwise (bool)
         */
         inline bool isWordChar(const char c) const {

            return (isalnum(c) || '-' == c || '_' == c) ? true : false;
         }

         /*
            Returns true if the given character is punctuation and false
            otherwise.

            Input:
               Character (char)

            Output:
               True if c is punctuation and false otherwise (bool)
         */
         inline bool isPunctuation(const char c) const {

            return (',' == c || '.' == c || ';' == c || ':' == c) ? true : false;
         }

         /*
            Skips over whitespace (not counting \n) in the source.

            Input:
               (none)

            Output:
               (none)
         */
         void skipWhitespace();

         /*
            Reads a quoted string from the source.

            Input:
               (none)

            Output:
               Quoted string (string)
         */
         string getQuotedString();

         /*
            Reads a word from the source.

            Input:
               (none)

            Output:
               Word (string)
         */
         string getWord();

      public:

         /*
            Attempts to read the file and populate source with its contents.
            Throws a ParseException if the file cannot be read.

            Input:
               Filename (string)

            Output:
               (none)
         */
         void open(string filename);

         /*
            Returns the source code currently being tokenized.

            Input:
               (none)

            Output:
               Source code (string)
         */
         inline const string getSource() const {return source;}

         /*
            Pushes back a token that's already been read so it can be read again
            by next(). This is a stack, so the last token pushed is the first
            token returned by next().

            Input:
               Token

            Output:
               (none)
         */
         inline void push(Token t) {tokenBuffer.push(t);}

         /*
            Returns the next token. If we've reached the end of the file, the
            token will have the SOURCE_EOF type.

            Input:
               (none)

            Output:
               Next token (Token)
         */
         Token next();
   };
}


#endif

