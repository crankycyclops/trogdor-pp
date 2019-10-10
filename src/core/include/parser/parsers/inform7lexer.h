#ifndef INFORM7LEXER_H
#define INFORM7LEXER_H


#include <string>
#include <stack>
#include <unordered_set>
#include <unordered_map>

#include "../../utility.h"


using namespace std;

namespace trogdor {


   enum TokenType {
      SOURCE_EOF          = 0,
      PHRASE_TERMINATOR   = 1,
      COMMA               = 2,
      COLON               = 3,
      SEMICOLON           = 4,
      WORD                = 5,
      ARTICLE             = 6,
      EQUALITY            = 7,
      AND                 = 8,
      QUOTED_STRING       = 9
   };

   struct Token {
      string value;
      TokenType type;
      int lineno;
   };

   /**************************************************************************/

   class Inform7Lexer {

      private:

         // String representations of each token type for debugging
         unordered_map<TokenType, string, std::hash<int>> tokenTypeToStr;

         // Some words are actually composites of more than one word. For
         // example: "player's carryall" is treated as one word, but is actually
         // composed of two. In the lexer's current implementation, when parsing
         // a word, we start by temporarily lexing as many words as we can
         // before the end of a statement and attempting to match them to
         // entries in directions, classes, and adjectives. If no match is found,
         // we reduce the compound word by one less word and try matching again
         // until either we found a match or we only have one word left (in
         // which case we return just the one word.) The problem is, the source
         // file might be malformatted, in which case we'll continue reading
         // words until EOF, potentially causing memory issues. To mitigate
         // that, I'm only going to allow a maximum compound word length of
         // this size.
         const int maxCompoundWordLength = 4;

         // Reference to parser's list of defined directions
         const unordered_map<string, string> &directions;

         // Reference to parser's list of defined classes
         const unordered_set<string> &classes;

         // Reference to parser's list of defined adjectives
         const unordered_set<string> &adjectives;

         // Inform 7 source code
         string source;

         // Index to next unparsed character in source
         int sourceIndex = 0;

         // Current line number in the source code
         int sourceLine = 1;

         // Tokens that were read ahead to make a decision and then "pushed back"
         stack<Token> tokenBuffer;

         // The most recently lexed token
         Token currentToken;

         /*
            Returns true if the string is a form of the verb to be and false if
            not.

            Input:
               Word (string)

            Output:
               True if word is a form of the verb "to be" and false if not (bool)
         */
         inline bool isEquality(string s) const {

            s = strToLower(s);

            return (
               0 == s.compare("is")  ||
               0 == s.compare("are")
            ) ? true : false;
         }

         /*
            Returns true if the string is an article and false if not.

            Input:
               Word (string)

            Output:
               True if string is an article and false if not (bool)
         */
         inline bool isArticle(string s) const {

            s = strToLower(s);

            return (
               0 == s.compare("a")    ||
               0 == s.compare("an")   ||
               0 == s.compare("the")  ||
               0 == s.compare("some")
            ) ? true : false;
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

            return (isalnum(c) || '-' == c || '_' == c || '\'' == c) ? true : false;
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
            Reads a sentence terminator (if one exists.) Returns an empty string
            if there isn't one at the current source index.

            Input:
               (none)

            Output:
               Sentence terminator or empty string (string)
         */
         string getSentenceTerminator();

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
            Constructor
         */
         inline Inform7Lexer(
            const unordered_map<string, string> &dirs,
            const unordered_set<string> &cls,
            const unordered_set<string> &adjs
         ): directions(dirs), classes(cls), adjectives(adjs),
         tokenTypeToStr({
            {SOURCE_EOF, "SOURCE_EOF"},
            {PHRASE_TERMINATOR, "PHRASE_TERMINATOR"},
            {COMMA, "COMMA"},
            {COLON, "COLON"},
            {SEMICOLON, "SEMICOLON"},
            {WORD, "WORD"},
            {ARTICLE, "ARTICLE"},
            {EQUALITY, "EQUALITY"},
            {AND, "AND"},
            {QUOTED_STRING, "QUOTED_STRING"}
         }),
         currentToken({"", SOURCE_EOF, 0}) {}

         Inform7Lexer() = delete;

         /*
            Get a string representation of a token type. Useful for debugging.

            Input:
               Token type (TokenType)

            Output:
               String representation of the type (string)
         */
         inline string getTokenTypeStr(TokenType type) const {

            return tokenTypeToStr.find(type)->second;
         }

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
            Returns the current line in the source code.

            Input:
               (none)

            Output:
               Current line (int)
         */
         inline const int getSourceLine() const {return sourceLine;}

         /*
            Peek at the current token without advancing the token stream. If we
            haven't called next() at least once before calling this method, an
            empty token with type SOURCE_EOF is returned.

            Input:
               (none)

            Output:
               Current token (Token)
         */
         inline Token peek() const {return currentToken;}

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

