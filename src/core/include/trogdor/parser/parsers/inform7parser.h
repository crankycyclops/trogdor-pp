#ifndef INFORM7PARSER_H
#define INFORM7PARSER_H


#include <memory>
#include <string>
#include <vector>
#include <tuple>
#include <unordered_set>
#include <unordered_map>

#include <trogdor/utility.h>
#include <trogdor/vocabulary.h>
#include <trogdor/languages/english.h>
#include <trogdor/exception/parseexception.h>

#include <trogdor/parser/parser.h>
#include <trogdor/parser/parsers/inform7/kind.h>
#include <trogdor/parser/parsers/inform7/lexer.h>

namespace trogdor {


   /*
      Parses an Inform 7 source file and uses it to instantiate the game. This
      is an experimental work in progress, and since Trogdor-pp and Inform are
      different projects with different goals, this will likely never support
      the entire language.

      I'm writing this to learn more about grammars and parsing and because I
      think it will be a fun project :) Don't expect this to be particularly
      useful.

      Here's the current subset of the language I'm parsing in EBNF format (I'll
      keep this updated as support for additional language features grows):

      <program>              ::= [<bibliographic>] <phrase> [{<phrase terminator>
                                 <phrase>}]
      <bibliographic>        ::= <quoted string> ["by" <author name>]
                                 <phrase terminator>
      <author name>          ::= /[A-Za-z ']+/ | <quoted string>
      <phrase>               ::= <equality statement> | <description stmt>
      <description stmt>     ::= {<article>} description of {<article>} <noun>
                                 <equality verb> <description>
      <equality statement>   ::= (<definition> | <property assignment> |
                                 <placement>) [<phrase terminator> <description>]
      <definition>           ::= <identifier list> <equality verb> {<article>} (
                                 [<property list>] <kind> [<in clause> | 
                                 <on clause>] | <location clause>)
      <property assignment>  ::= <identifier list> <equality verb> {<article>}
                                 <property list>
      <placement>            ::= <identifier list> <equality verb> ("in" | "on")
                                 {<article>} <noun>
      <identifier list>      ::= {<article>} <noun> {("," | [","] "and")
                                 {<article>} <noun>}
      <in clause>            ::= "in" {<article>} <noun>
      <on clause>            ::= "on" {<article>} <noun>
      <location clause>      ::= <direction> ("of" | "from") {<article>} <noun>
      <description>          ::= "\"" "/^[\"]+/" ".\"" | "\"" "/^[\"]+/\""
      <equality verb>        ::= "is" | "are"
      <assertion verb>       ::= "has" | "carries" | "is carrying" | "wears" |
                                 "is wearing" | "contains" | "supports" |
                                 "is supporting"
      <article>              ::= "a" | "an" | "the"
      <direction>            ::= "north" | "northeast" | "east" | "southeast" |
                                 "south" | "southwest" | "west" | "northwest" |
                                 "up" | "down" | "inside" | "outside"
      <kind>                 ::= "object" | "direction" | "room" | "region" |
                                 "thing" | "door" | "container" | "vehicle" |
                                 "player's holdall" | "supporter" | "backdrop" |
                                 "device" | "person" | "man" | "woman" | "animal"
      <plural kind>          ::= "objects" | "directions" | "rooms" |
                                 "regions" | "things" | "doors" | "containers" |
                                 "vehicles" | "player's holdalls" |
                                 "supporters" | "backdrops" | "devices" |
                                 "people" | "men" | "women" | "animals"
      <adjective list>       ::= <adjective> {[","] ["and"] <adjective>}
      <adjective>            ::= ["not"] (<property> | ("adjacent" | "visible" |
                                 "invisible" | "visited" | "empty" |
                                 "non-empty" | "carried" | "positive" |
                                 "negative" | "even" | "odd"))
      <property list>        ::= <property> {[","] ["and"] <property>}
      <property>             ::= ["not"] ("touchable" | "untouchable" |
                                 "lighted" | "dark" | "open" | "closed" |
                                 "lit" | "unlit" | "transparent" | "opaque" |
                                 "fixed in place" | "portable" | "openable" |
                                 "unopenable" | "enterable" | "wearable" |
                                 "edible" | "inedible" | "described" |
                                 "undescribed" | "male" | "female" | "neuter" |
                                 "pushable between rooms" | "locked" |
                                 "unlocked" | "lockable" | "unlockable" |
                                 "switched on" | "switched off")
      <noun>                 ::= /[A-Za-z ']+/
      <quoted string>        ::= "\" "/^[\"]+/" \""
      <phrase terminator>    ::= ("." | "\n\n") {"\n"}

      * Directions, kinds, properties, and adjectives listed in the above EBNF
      are those that are built into Inform 7. I'll eventually add support for
      parsing custom kinds and adjectives, in which case the grammar should
      also consider those, once inserted, as their respective types.

      Note that I had to cobble together the EBNF above myself using examples
      from the official documentation (http://inform7.com/book/WI_1_1.html)
      along with other supplemental sources.

      This one in particular helped a lot:
      http://www.ifwiki.org/index.php/Inform_7_for_Programmers

      And this one:
      http://mirrors.ibiblio.org/interactive-fiction/programming/inform7/manuals/Inform7_CheatSheet.pdf

      Where possible, I try to throw the same (or similar) error messages as
      those returned by the original Inform 7 compiler, but I deviate from
      these in many places, both because I can't know how the original parser
      walks through the source code and because I'm able to provide, in some
      cases, more detailed messages about what went wrong.
   */
   class Inform7Parser: public Parser {

      private:

         // Data type that stores a parsed property and whether or not it's
         // negated (prefixed by "not")
         struct ParsedProperty {
            std::string value;
            bool negated;
         };

         // Breaks Inform 7 source down into a token stream
         Lexer lexer;

         // All language rules are based on English
         English language;

         // Set of directions recognized by Inform 7 (list can be extended.)
         // Keys are the direction and values are the opposite. For example,
         // directions["north"] = "south".
         std::unordered_map<std::string, std::string> directions;

         // Set of non-property adjectives recognized by Inform 7
         std::unordered_set<std::string> adjectives;

         // Special entity names that shouldn't be instantiated, like "nowhere"
         std::unordered_set<std::string> specialIdentifiers;

         // Represents the Inform 7 kinds hierarchy
         // (See: http://www.ifwiki.org/index.php/Inform_7_for_Programmers/Part_1#Class_And_Prejudice)
         std::unique_ptr<Kind> kinds;

         // Maps kind names to their entry in the kinds hierarchy
         std::unordered_map<

            // The Kind's name
            std::string,

            std::tuple<

               // The Kind we're referencing
               Kind *,

               // A callback that will insert an AST node defining the internal
               // entity class
               std::function<void(size_t)>,

               // Whether or not the AST node has been inserted already (prevents
               // multiple insertions)
               bool,

               // The name of the internal entity class corresponding to the
               // Inform 7 kind
               std::string
            >
         > kindsMap;

         // Plural lookup mapping plural kinds to their singular equivalents
         std::unordered_map<std::string, std::string> kindPlurals;

         // Maps abstract Kinds to their default intantiable kind (for example,
         // "object" -> "thing")
         std::unordered_map<std::string, Kind *> abstractKinds;

         // Set of properties (both either/or and value) recognized by Inform 7.
         std::unordered_map<

            // Property name
            std::string,

            std::tuple<

               // The highest and most basic kinds in the hierarchy for which
               // the properties can be set
               std::vector<Kind *>,

               // Properties that are contradictory
               std::vector<std::string>,

               // A callback that inserts the appropriate AST nodes. Takes as
               // input an entity name, whether or not the property was negated,
               // and the line number where the property was set.
               std::function<void(std::string, size_t, bool)>
            >

         > properties;

         // Symbol table for all declared entities in the game, mapping the
         // entity's name to a set of possible kinds and whatever properties
         // have been set on the entity in the source.
         std::unordered_map<std::string, std::tuple<

            // If the kind is explicitly set in the source, then this will
            // contain just one value. Otherwise, we'll have to resort to some
            // tie-breaking logic to decide which kind we should pick when it
            // comes time to construct the AST.
            std::unordered_set<Kind *>,

            // Each property that has been set on the entity, along with whether
            // or not the property has been negated and the line number where the
            // property was set.
            std::unordered_map<std::string, std::pair<bool, size_t>>,

            // The entity's description
            std::string,

            // Line number where the entity was defined
            size_t

         >> entities;

         // Tracks connections between rooms and doors by mapping an entity
         // name to another unordered_map that maps a direction to another
         // entity name, as well as to a boolean that determines whether the
         // connection was explicitly or implicitly defined (if the former, we
         // check for contradictions; if the latter, we try to find conditions
         // under which both statements could be true). If an entity name and
         // direction map to an empty string as opposed to the pair just not
         // mapping to anything, we know that the connection has been explicitly
         // removed.
         std::unordered_map<
            std::string,
            std::unordered_map<std::string, std::tuple<std::string, bool, size_t>>
         > entityConnections;

         // Maintains a list of entities in the order that they were declared,
         // either implicitly or explicitly
         std::vector<std::string> entitiesOrdered;

         // Optional Bibliographic Data
         std::optional<std::string> parsedTitle;
         std::optional<std::string> parsedAuthor;

         /*
            Reduces an entity's set of possible kinds down to only those that
            inherit from the passed in kinds. Can also take an optional third
            parameter which will look for an exact kind match and return true if
            that match is found.

            The way I described the use of lookForExactMatch isn't very clear,
            but I can't think of a better way to do so. Instead, to understand
            what this is for, just look for any call to filterKinds() that
            passes in a third parameter and checks the return value. You should
            be able to figure it out that way.

            Input:
               Entity's name (std::string)
               All possible kinds (std::vector<Kind *>)
               An optional kind to look for an exact match with (std::vector<Kind *>)

            Output:
               True if we find one or more exact matches (listed in
               lookForExactMatch) and false if not.
         */
         bool filterKinds(
            std::string entity,
            std::vector<Kind *> compatibleKinds,
            Kind *exactMatch = nullptr
         );

         /*
            Initializes standard rules, including built-in kinds, directions,
            and adjectives.

            Input:
               (none)

            Output:
               (none)
         */
         void initStandardRules();

         /*
            Initializes built-in directions.

            Input:
               (none)

            Output:
               (none)
         */
         void initBuiltinDirections();

         /*
            Initializes built-in kinds and their settable properties.

            Input:
               (none)

            Output:
               (none)
         */
         void initBuiltinKindsAndProperties();

         /*
            Initializes built-in non-property adjectives.

            Input:
               (none)

            Output:
               (none)
         */
         void initBuiltinNonPropertyAdjectives();

         /*
            Parses one or more identifiers on the left hand side of an equality.
            Matches the <identifiers list> production in the EBNF above. This
            method is kind of a cheat and deviates from the LL parsing pattern.
            If I don't do this, I'll have far too much lookahead.

            Input:
               Whether or not the parsed identifiers should be recorded in the
                  entity symbol table (bool)

            Output:
               Vector containing one or more identifiers.
         */
         std::vector<std::string> parseIdentifiersList(bool declareEntities = true);

         /*
            Parses one or more properties on the left hand side of an equality.
            Matches the <property list> production in the EBNF above. Like
            parseIdentifiersList(), this method is kind of a cheat. It deviates
            from the LL parsing pattern so that I won't have too much
            lookahead.

            Input:
               Identifiers for entities the properties apply to (std::vector<std::string>)

            Output:
               Vector containing one or more properties
         */
         std::vector<ParsedProperty> parsePropertyList(std::vector<std::string> identifiers);

         /*
            Parses the in clause of a definition. Matches the <in clause>
            production in the EBNF above.

            Input:
               One or more things that are in a container or place (std::vector<std::string>)

            Output:
               (none)
         */
         void parseInClause(std::vector<std::string> subjects);

         /*
            Parses the on clause of a definition. Matches the <on clause>
            production in the EBNF above.

            Input:
               One or more things that are on a supporter (std::vector<std::string>)

            Output:
               (none)
         */
         void parseOnClause(std::vector<std::string> subjects);

         /*
            Parses the location clause of a definition. Matches the <location
            clause> production in the EBNF above.

            Input:
               One or more rooms (if more than 1, an error is thrown - std::vector<std::string>)
               Direction (std::string)

            Output:
               (none)
         */
         void parseLocationClause(std::vector<std::string> identifiers, std::string direction);

         /*
            Parses a thing's description.

            Input:
               All identifiers being described (if more than 1, we output an error)

            Output:
               (none)
         */
         void parseDescription(std::vector<std::string> identifiers);

         /*
            Parses the definition of one or more things. Matches the
            <definition> production in the EBNF above.

            Input:
               One or more identifiers (std::vector<std::string>)

            Output:
               (none)
         */
         void parseDefinition(std::vector<std::string> identifiers);

         /*
            Parses the placement of one or more things into or onto a room,
            container, or supporter. Matches the <placement> production in the
            EBNF above.

            Input:
               One or more identifiers (std::vector<std::string>)

            Output:
               (none)
         */
         void parsePlacement(std::vector<std::string> subjects);

         /*
            Parses an equality phrase, or any phrase that uses the present tense
            of the verb "to be." Matches the <equality statement> production in
            the EBNF above.

            Input:
               One or more identifiers as the subject (std::vector<std::string>)

            Output:
               (none)
         */
         void parseEquality(std::vector<std::string> identifiers);

         /*
            Parses a description statement of the form "the description of
            <identifier> is "<description>". Matches the <description stmt>
            production in the EBNF above.

            Input:
               Name of the thing being described (std::string)

            Output:
               (none)
         */
         void parseDescriptionStatement(std::string identifier);

         /*
            Parses the optional bibliographic sentence at the beginning of the
            source code. Corresponds to the <bibliographic> production in the
            EBNF above.

            Input:
               (none)

            Output:
               (none)
         */
         void parseBibliographic();

         /*
            Parses a phrase. Corresponds to the <phrase> production in the EBNF
            above.

            Input:
               (none)

            Output:
               (none)
         */
         void parsePhrase();

         /*
            Parses an Inform 7 program. Corresponds to the <program> production
            in the EBNF above.

            Input:
               (none)

            Output:
               (none)
         */
         void parseProgram();

         /*
            Takes as input an entity with a set of possible kinds assigned and
            applies rules to determine which will be the entity's type. In the
            simplest case where there is only one, that one kind will simply be
            returned. If a group of kinds is passed in that we have no rule for,
            an instance of UndefinedException will be thrown.

            Input:
               The name of the entity whose kinds we're examining (std::string)

            Output:
               The chosen kind (Kind *)
         */
         Kind *resolveKind(std::string entityName) const;

         /*
            Builds the AST that will then be fed to the instantiator. Does some
            additional constraint checking in the process that couldn't be
            completed during the parsing step.

            Input:
               (none)

            Output:
               (none)
         */
         void buildAST();

      protected:

         /*
            Inserts a direction recognized by Inform 7. Takes as input both the
            new direction and its opposite. If the opposite direction doesn't
            exist, it will be inserted, too, and the new direction will be set
            as its opposite.

            Example call: insertDirection("north", "south");

            Input:
               New direction (std::string)
               New direction's opposite (std::string)

            Output:
               (none)
         */
         inline void insertDirection(std::string direction, std::string opposite) {

            if (directions.end() != directions.find(direction)) {
               throw ParseException(std::string("Direction '") + direction +
                  "' has already been defined.");
            }

            directions[direction] = opposite;

            if (directions.end() == directions.find(opposite)) {
               directions[opposite] = direction;
            }
         }

         /*
            Inserts an Inform 7 kind.

            Input:
               Parent node in the kind hierarchy (Kind *)
               Kind name (std::string)
               Entity type (entity::EntityType)
               Callback to define AST node (std::function<void()>)

            Output:
               (none)
         */
         inline void insertKind(
            Kind *parent,
            std::string kindName,
            std::string entityClassName,
            entity::EntityType type,
            std::function<void(size_t)> ASTClassDefinitionCallback,
            std::function<void(size_t)> ASTNodeCallback = {}
         ) {

            Kind *node = parent->appendChild(kindName, type, ASTNodeCallback);

            kindsMap[kindName] = {node, ASTClassDefinitionCallback, false, entityClassName};
            kindPlurals[language.pluralizeNoun(kindName)] = kindName;
         }

         /*
            Inserts an Inform 7 property that can be assigned to an instance of
            a certain kind.

            Input:
               Property (std::string)
               The most basic kinds on which the property can be set (std::vector<Kind *>)
               Properties that are contradictory (std::vector<std::string>)

            Output:
               (none)
         */
         inline void insertProperty(
            std::string property,
            std::vector<Kind *> kinds,
            std::function<void(std::string, size_t, bool)> ASTNodeCallback = {},
            std::vector<std::string> contradictions = {}
         ) {

            if (properties.end() == properties.find(property)) {
               properties[property] = {kinds, contradictions, ASTNodeCallback};
            }
         }

         /*
            Inserts an Inform 7 adjective.

            Input:
               Adjective (std::string)

            Output:
               (none)
         */
         inline void insertAdjective(std::string adjective) {

            if (adjectives.end() == adjectives.find(adjective)) {
               adjectives.insert(adjective);
            }
         }

         /*
            Debug method that dumps all parsed entities to stdout.

            Input:
               (none)

            Output:
               (none)
         */
         inline void dumpEntities() const {

            for (const auto &entity: entities) {

               std::string kindsStr;

               for (const auto &kind: std::get<0>(entity.second)) {
                  kindsStr += kind->getName() += ", ";
               }

               kindsStr = kindsStr.substr(0, kindsStr.length() - 2);

               std::string propertiesStr;

               for (const auto &property: std::get<1>(entity.second)) {
                  propertiesStr += (property.second.first ? "not " : "");
                  propertiesStr += property.first + ", ";
               }

               propertiesStr = propertiesStr.substr(0, propertiesStr.length() - 2);

               std::cout << "Entity name: " << entity.first << std::endl;
               std::cout << "Possible Kinds: " << kindsStr << std::endl;
               std::cout << "Resolved Kind: " << resolveKind(entity.first)->getName() << std::endl;
               std::cout << "Properties: "
                  << (propertiesStr.length() ? propertiesStr : "(none)")
                  << std::endl;
               std::cout << "Description: "
                  << (std::get<2>(entity.second).length() ? std::get<2>(entity.second) : "(none)")
                  << std::endl;

               std::cout << "\n" << std::endl;
            }
         }

         /*
            Debug method that dumps all connections between rooms and doors to
            stdout.

            Input:
               (none)

            Output:
               (none)
         */
         inline void dumpEntityConnections() const {

            for (const auto &entity: entityConnections) {
               for (const auto &connection: entity.second) {
                  std::cout << (std::get<0>(connection.second).length() ? std::get<0>(connection.second) : "nowhere")
                     << " is " << connection.first << " of "
                     << (entity.first.length() ? entity.first : "nowhere")
                     << (std::get<1>(connection.second) ? " (explicit)" : "")
                     << std::endl;
               }
            }
         }

      public:

         /*
            Constructor
         */
         Inform7Parser(std::unique_ptr<Instantiator> i, const Vocabulary &v);

         /*
            Destructor
         */
         ~Inform7Parser();

         /*
            Parses an Inform 7 source file.

            Input:
               Filename where the i7 source file is saved (std::string)

            Output:
               (none)
         */
         virtual void parse(std::string filename);
   };
}


#endif
