#ifndef LEXER_H
#define LEXER_H


#include <string>
#include <deque>
#include <tuple>
#include <unordered_set>
#include <unordered_map>

#include <trogdor/utility.h>
#include <trogdor/entities/entity.h>

#include <trogdor/parser/parsers/inform7/kind.h>

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
      std::string value;
      TokenType type;
      size_t lineno;
   };

   /**************************************************************************/

   class Lexer {

      private:

         // String representations of each token type for debugging
         std::unordered_map<TokenType, std::string, std::hash<int>> tokenTypeToStr;

         // Some words are actually composites of more than one word. For
         // example: "player's carryall" is treated as one word, but is actually
         // composed of two. In the lexer's current implementation, when parsing
         // a word, we start by temporarily lexing as many words as we can
         // before the end of a statement and attempting to match them to
         // entries in directions, kinds, properties, and adjectives. If no
         // match is found, we reduce the compound word by one less word and try
         // matching again until either we found a match or we only have one
         // word left (in which case we return just the one word.) The problem
         // is, the source file might be malformatted, in which case we'll
         // continue reading words until EOF, potentially causing memory issues.
         // To mitigate that, I'm only going to allow a maximum compound word
         // length of this size.
         const int maxCompoundWordLength = 4;

         // Reference to parser's list of defined directions
         const std::unordered_map<std::string, std::string> &directions;

         // Reference to parser's list of defined non-property adjectives
         const std::unordered_set<std::string> &adjectives;

         // Reference to parser's list of defined kinds
         const std::unordered_map<
            std::string,
            std::tuple<Kind *, std::function<void(size_t)>, bool>
         > &kinds;

         // Reference to parser's list of defined properties
         const std::unordered_map<
            std::string,
            std::pair<std::vector<Kind *>, std::vector<std::string>>
         > &properties;

         // Inform 7 source code
         std::string source;

         // Index to next unparsed character in source
         size_t sourceIndex = 0;

         // Current line number in the source code
         size_t sourceLine = 1;

         // Tokens that were read ahead to make a decision and then "pushed back"
         std::deque<Token> tokenBuffer;

         // The most recently lexed token
         Token currentToken;

         // If true, we should skip over the next phrase terminator we encounter
         // (workaround to enable parsing terminator inside a quoted string.)
         bool skipNextPhraseTerminator = false;

         /*
            Returns true if the string is a form of the verb to be and false if
            not.

            Input:
               Word (std::string)

            Output:
               True if word is a form of the verb "to be" and false if not (bool)
         */
         inline bool isEquality(std::string s) const {

            s = strToLower(s);

            return (
               0 == s.compare("is")  ||
               0 == s.compare("are")
            ) ? true : false;
         }

         /*
            Returns true if the string is an article and false if not.

            Input:
               Word (std::string)

            Output:
               True if string is an article and false if not (bool)
         */
         inline bool isArticle(std::string s) const {

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
               Sentence terminator or empty string (std::string)
         */
         std::string getSentenceTerminator();

         /*
            Reads a quoted string from the source.

            Input:
               (none)

            Output:
               Quoted string (std::string)
         */
         std::string getQuotedString();

         /*
            Reads a word from the source.

            Input:
               (none)

            Output:
               Word (std::string)
         */
         std::string getWord();

      public:

         /*
            Constructor
         */
         inline Lexer(
            const std::unordered_map<std::string, std::string> &dirs,
            const std::unordered_set<std::string> &adjs,
            const std::unordered_map<std::string, std::tuple<
               Kind *, std::function<void(size_t)>, bool>
            > &ks,
            const std::unordered_map<std::string, std::pair<
	            std::vector<Kind *>, std::vector<std::string>>
            > &props
         ):
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
         }), directions(dirs), adjectives(adjs), kinds(ks), properties(props),
         currentToken({"", SOURCE_EOF, 0}) {}

         Lexer() = delete;

         /*
            Get a string representation of a token type. Useful for debugging.

            Input:
               Token type (TokenType)

            Output:
               String representation of the type (std::string)
         */
         inline std::string getTokenTypeStr(TokenType type) const {

            return tokenTypeToStr.find(type)->second;
         }

         /*
            Attempts to read the file and populate source with its contents.
            Throws a ParseException if the file cannot be read.

            Input:
               Filename (std::string)

            Output:
               (none)
         */
         void open(std::string filename);

         /*
            Returns the source code currently being tokenized.

            Input:
               (none)

            Output:
               Source code (std::string)
         */
         inline const std::string getSource() const {return source;}

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
         inline void push(Token t) {tokenBuffer.push_back(t);}

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
