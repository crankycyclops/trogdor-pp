#include <memory>
#include <vector>

// TODO: remove this after testing
#include <iostream>

#include <trogdor/utility.h>
#include <trogdor/vocabulary.h>
#include <trogdor/exception/parseexception.h>
#include <trogdor/entities/object.h>

#include <trogdor/parser/parsers/inform7parser.h>

namespace trogdor {


   Inform7Parser::Inform7Parser(std::unique_ptr<Instantiator> i,
   const Vocabulary &v): Parser(std::move(i), v), lexer(directions, adjectives,
   kindsMap, properties), parsedTitle(std::nullopt), parsedAuthor(std::nullopt) {

      initStandardRules();
   }

   /**************************************************************************/

   Inform7Parser::~Inform7Parser() {

      // TODO
   }

   /**************************************************************************/

   void Inform7Parser::initStandardRules() {

      initBuiltinDirections();
      initBuiltinKindsAndProperties();
      initBuiltinNonPropertyAdjectives();
   }

   /**************************************************************************/

   void Inform7Parser::initBuiltinDirections() {

      // Built-in directions that Inform 7 recognizes by default. List can be
      // extended later. This structure maps directions to their opposites.
      insertDirection("up", "down");
      insertDirection("inside", "outside");
      insertDirection("north", "south");
      insertDirection("east", "west");
      insertDirection("northwest", "southeast");
      insertDirection("northeast", "southwest");
   }

   /**************************************************************************/

   void Inform7Parser::initBuiltinKindsAndProperties() {

      // Insert kinds built into Inform 7 and their internal mappings to Entity
      // types and classes
      kinds = std::make_unique<Kind>("root", entity::ENTITY_UNDEFINED);

      // You'll note that, in the true Inform 7 hierarchy, "direction" is a child
      // of "object." However, I'm going to deviate from that, at least for now,
      // and make it its own root type, especially since it doesn't actually
      // correspond to an Entity type.
      insertKind(kinds.get(), "direction", entity::ENTITY_UNDEFINED,
      [&] (size_t lineno) {

         // This is just temporary for our current subset of Inform 7; we can
         // define custom directions internally already, and when I'm ready to
         // support this in my parser, I can implement this callback.
         throw UndefinedException(
            "Custom directions aren't yet supported (line " + lineno + ')'
         );
      });

      insertKind(kinds.get(), "object", entity::ENTITY_UNDEFINED,
      [&] (size_t lineno) {

         throw UndefinedException(
            "The 'object' kind shouldn't be directly instantiable. Instead, entities should default to things or some other more specific type. This is a bug in the Inform 7 parser and should be fixed. (line " + lineno + ')'
         );
      });

      insertKind(std::get<0>(kindsMap["object"]), "region", entity::ENTITY_UNDEFINED,
      [&] (size_t lineno) {

         throw UndefinedException(
            "Inform 7 regions aren't currently supported (line " + lineno + ')'
         );
      });

      insertKind(std::get<0>(kindsMap["object"]), "room", entity::ENTITY_ROOM,
      [&] (size_t lineno) {

         ast->appendChild(ASTDefineEntityClass(
            "inform7_room",
            entity::Entity::typeToStr(entity::ENTITY_ROOM),
            lineno
         ));
      });

      insertKind(std::get<0>(kindsMap["object"]), "thing", entity::ENTITY_OBJECT,
      [&] (size_t lineno) {

         ast->appendChild(ASTDefineEntityClass(
            "inform7_thing",
            entity::Entity::typeToStr(entity::ENTITY_OBJECT),
            lineno
         ));
      });

      insertKind(std::get<0>(kindsMap["thing"]), "door", entity::ENTITY_OBJECT,
      [&] (size_t lineno) {

         // TODO: issue warning about only partial support
         ast->appendChild(ASTDefineEntityClass(
            "inform7_door",
            entity::Entity::typeToStr(entity::ENTITY_OBJECT),
            lineno
         ));
      }, [&] (size_t lineno) {

         // Doors are never allowed to move, so we set the Trogdor-pp
         // equivalent tag for the internal Entity class
         ast->appendChild(ASTSetTag(
            "class",
            entity::Object::UntakeableTag,
            lineno,
            "inform7_door"
         ));
      });

      insertKind(std::get<0>(kindsMap["thing"]), "container", entity::ENTITY_OBJECT,
      [&] (size_t lineno) {

         // TODO: issue warning about only partial support
         ast->appendChild(ASTDefineEntityClass(
            "inform7_container",
            entity::Entity::typeToStr(entity::ENTITY_OBJECT),
            lineno
         ));
      });

      insertKind(std::get<0>(kindsMap["container"]), "vehicle", entity::ENTITY_OBJECT,
      [&] (size_t lineno) {

         // TODO: issue warning about only partial support
         ast->appendChild(ASTDefineEntityClass(
            "inform7_vehicle",
            entity::Entity::typeToStr(entity::ENTITY_OBJECT),
            lineno
         ));
      }, [&] (size_t lineno) {

         // Vehicles are not portable by default, so we set the Trogdor-pp
         // equivalent tag for the class
         ast->appendChild(ASTSetTag(
            "class",
            entity::Object::UntakeableTag,
            lineno,
            "inform7_vehicle"
         ));
      });

      insertKind(std::get<0>(kindsMap["container"]), "player's holdall",
      entity::ENTITY_OBJECT, [&] (size_t lineno) {

         // TODO: issue warning about only partial support
         ast->appendChild(ASTDefineEntityClass(
            "inform7_players_holdall",
            entity::Entity::typeToStr(entity::ENTITY_OBJECT),
            lineno
         ));
      });

      insertKind(std::get<0>(kindsMap["thing"]), "supporter", entity::ENTITY_OBJECT,
      [&] (size_t lineno) {

         // TODO: issue warning about only partial support
         ast->appendChild(ASTDefineEntityClass(
            "inform7_supporter",
            entity::Entity::typeToStr(entity::ENTITY_OBJECT),
            lineno
         ));
      });

      insertKind(std::get<0>(kindsMap["thing"]), "backdrop", entity::ENTITY_UNDEFINED,
      [&] (size_t lineno) {

         throw UndefinedException(
            "Inform 7 backdrops aren't yet supported (line " + lineno + ')'
         );
      });

      insertKind(std::get<0>(kindsMap["thing"]), "device", entity::ENTITY_OBJECT,
      [&] (size_t lineno) {

         // TODO: issue warning about only partial support
         ast->appendChild(ASTDefineEntityClass(
            "inform7_device",
            entity::Entity::typeToStr(entity::ENTITY_OBJECT),
            lineno
         ));
      });

      insertKind(std::get<0>(kindsMap["thing"]), "person", entity::ENTITY_CREATURE,
      [&] (size_t lineno) {

         ast->appendChild(ASTDefineEntityClass(
            "inform7_person",
            entity::Entity::typeToStr(entity::ENTITY_CREATURE),
            lineno
         ));
      });

      insertKind(std::get<0>(kindsMap["person"]), "man", entity::ENTITY_CREATURE,
      [&] (size_t lineno) {

         ast->appendChild(ASTDefineEntityClass(
            "inform7_man",
            entity::Entity::typeToStr(entity::ENTITY_CREATURE),
            lineno
         ));
      });

      insertKind(std::get<0>(kindsMap["person"]), "woman", entity::ENTITY_CREATURE,
      [&] (size_t lineno) {

         ast->appendChild(ASTDefineEntityClass(
            "inform7_woman",
            entity::Entity::typeToStr(entity::ENTITY_CREATURE),
            lineno
         ));
      });

      insertKind(std::get<0>(kindsMap["person"]), "animal", entity::ENTITY_CREATURE,
      [&] (size_t lineno) {

         ast->appendChild(ASTDefineEntityClass(
            "inform7_animal",
            entity::Entity::typeToStr(entity::ENTITY_CREATURE),
            lineno
         ));
      });

      // Built-in properties that Inform 7 recognizes for things.
      insertProperty(
         "lit",
         {std::get<0>(kindsMap["thing"])},
         {"unlit"}
      );

      insertProperty(
         "unlit",
         {std::get<0>(kindsMap["thing"])},
         {"lit"}
      );

      insertProperty(
         "transparent",
         {std::get<0>(kindsMap["thing"])},
         {"opaque"}
      );

      insertProperty(
         "opaque",
         {std::get<0>(kindsMap["thing"])},
         {"transparent"}
      );

      insertProperty(
         "fixed in place",
         {std::get<0>(kindsMap["thing"])},
         {"portable"}
      );

      insertProperty(
         "portable",
         {std::get<0>(kindsMap["thing"])},
         {"fixed in place"}
      );

      insertProperty(
         "openable",
         {std::get<0>(kindsMap["thing"])},
         {"unopenable"}
      );

      insertProperty(
         "unopenable",
         {std::get<0>(kindsMap["thing"])},
         {"openable"}
      );

      insertProperty(
         "edible",
         {std::get<0>(kindsMap["thing"])},
         {"inedible"}
      );

      insertProperty(
         "inedible",
         {std::get<0>(kindsMap["thing"])},
         {"edible"}
      );

      insertProperty(
         "described",
         {std::get<0>(kindsMap["thing"])},
         {"undescribed"}
      );

      insertProperty(
         "undescribed",
         {std::get<0>(kindsMap["thing"])},
         {"described"}
      );

      insertProperty(
         "enterable",
         {std::get<0>(kindsMap["thing"])}
      );

      insertProperty(
         "wearable",
         {std::get<0>(kindsMap["thing"])}
      );

      insertProperty(
         "pushable between rooms",
         {std::get<0>(kindsMap["thing"])}
      );

      // Built-in properties that Inform 7 recognizes for rooms.
      insertProperty(
         "lighted",
         {std::get<0>(kindsMap["room"])},
         {"dark"}
      );

      insertProperty(
         "dark",
         {std::get<0>(kindsMap["room"])},
         {"lighted"}
      );

      // Built-in properties that Inform 7 recognizes for doors.
      insertProperty(
         "locked",
         {std::get<0>(kindsMap["door"])},
         {"unlocked"}
      );

      insertProperty(
         "unlocked",
         {std::get<0>(kindsMap["door"])},
         {"locked"}
      );

      insertProperty(
         "lockable",
         {std::get<0>(kindsMap["door"])},
         {"unlockable"}
      );

      insertProperty(
         "unlockable",
         {std::get<0>(kindsMap["door"])},
         {"lockable"}
      );

      // Built-in properties that Inform 7 recognizes for devices.
      insertProperty(
         "switched on",
         {std::get<0>(kindsMap["device"])},
         {"switched off"}
      );

      insertProperty(
         "switched off",
         {std::get<0>(kindsMap["device"])},
         {"switched on"}
      );

      // Built-in properties that Inform 7 recognizes for persons.
      insertProperty(
         "male",
         {std::get<0>(kindsMap["person"])},
         {"female", "neuter"}
      );

      insertProperty(
         "female",
         {std::get<0>(kindsMap["person"])},
         {"male", "neuter"}
      );

      insertProperty(
         "neuter",
         {std::get<0>(kindsMap["person"])},
         {"male", "female"}
      );

      // Built-in properties that Inform 7 recognizes for containers and doors.
      insertProperty(
         "open",
         {std::get<0>(kindsMap["door"]), std::get<0>(kindsMap["container"])},
         {"closed"}
      );

      insertProperty(
         "closed",
         {std::get<0>(kindsMap["door"]), std::get<0>(kindsMap["container"])},
         {"open"}
      );

      // According to Inform 7, men are always male and women are always female
      std::get<0>(kindsMap["man"])->insertProperty("male");
      std::get<0>(kindsMap["woman"])->insertProperty("female");
   }

   /**************************************************************************/

   void Inform7Parser::initBuiltinNonPropertyAdjectives() {

      // Built-in non-property adjectives that Inform 7 recognizes by default.
      insertAdjective("touchable");
      insertAdjective("untouchable");
      insertAdjective("adjacent");
      insertAdjective("visible");
      insertAdjective("invisible");
      insertAdjective("visited");
      insertAdjective("empty");
      insertAdjective("non-empty");
      insertAdjective("carried");

      // These special adjectives are for numerical values only
      insertAdjective("positive");
      insertAdjective("negative");
      insertAdjective("even");
      insertAdjective("odd");
   }

   /**************************************************************************/

   void Inform7Parser::parseDescriptionStatement(std::string identifier) {

      Token t = lexer.peek();

      if (EQUALITY == t.type) {

         t = lexer.next();

         if (QUOTED_STRING != t.type) {
            throw ParseException(
               "(line " + std::to_string(t.lineno) + "): "
               + "You said you were going to describe " + identifier
               + " but never did."
            );
         }

         else {
            lexer.push(t);
            parseDescription({identifier});
         }
      }

      else {
         throw ParseException(
            "(line " + std::to_string(t.lineno) + "): "
            + "I can't find a verb that I know how to deal with."
         );
      }
   }

   /**************************************************************************/

   void Inform7Parser::parseBibliographic() {

      Token t = lexer.next();

      std::string author;
      std::string title = t.value;

      t = lexer.next();

      if (0 == strToLower(t.value).compare("by")) {

         for (t = lexer.next(); (
            t.type != SOURCE_EOF &&
            t.type != PHRASE_TERMINATOR &&
            t.type != COLON &&
            t.type != SEMICOLON
         ); t = lexer.next()) {

            if (author.length()) {
               author += " ";
            }

            author += t.value;
         }

         if (!author.length()) {
            throw ParseException(
               "(line " + std::to_string(t.lineno) + "): "
               + "Initial bibliographic sentence has 'by' without author."
            );
         }
      }

      else if (PHRASE_TERMINATOR != t.type) {
         throw ParseException(
            "(line " + std::to_string(t.lineno) + "): "
            + "Initial bibliographic sentence can only be a title in double quotes, possibly followed with 'by' and the name of the author."
         );
      }

      if (title.length()) {
         parsedTitle = title;
      }

      if (author.length()) {
         parsedAuthor = author;
      }
   }

   /**************************************************************************/

   std::vector<std::string> Inform7Parser::parseIdentifiersList(bool declareEntities) {

      Token t;
      std::vector<std::string> identifiers;

      // Use this to make sure we only skip past articles if they really
      // function as articles by appearing before an identifier name
      bool startingNewNoun = true;

      do {

         std::string noun;

         // Skip past articles
         // TODO: need to keep track of whether or not an article was used; if
         // one was, then it's a proper noun. Otherwise, it's not. This effects
         // things like whether or not a room is visited when an object is in it.
         for (t = lexer.next(); startingNewNoun && ARTICLE == t.type; t = lexer.next());
         startingNewNoun = false;

         // Sometimes, we have a comma followed by and. In that case, skip over
         // the extraneous "and."
         if (AND == t.type) {
            t = lexer.next();
         }

         // Grab the next identifier
         for (; WORD == t.type || ARTICLE == t.type; t = lexer.next()) {
            noun += (noun.length() ? " " : "");
            noun += t.value;
         }

         if (noun.length()) {

            identifiers.push_back(noun);
            startingNewNoun = true;

            // Add the identifier to the entities symbol table
            if (declareEntities && entities.end() == entities.find(noun)) {
               entities[noun] = {{}, {}, ""};
            }
         }

         else {
            throw ParseException(
               "(line " + std::to_string(t.lineno) + "): "
               + "Sentence does not follow a valid syntax. Did you end with a dangling comma or \"and\"?"
            );
         }
      } while (COMMA == t.type || AND == t.type);

      lexer.push(t);
      return identifiers;
   }

   /**************************************************************************/

   std::vector<Inform7Parser::ParsedProperty> Inform7Parser::parsePropertyList(
      std::vector<std::string> identifiers
   ) {

      Token t = lexer.next();
      std::vector<Inform7Parser::ParsedProperty> propertyList;

      // Flags whether or not an adjective/property is preceded by "not"
      bool negated = false;

      while (
         properties.end() != properties.find(strToLower(t.value)) ||
         adjectives.end() != adjectives.find(strToLower(t.value)) ||
         0 == strToLower(t.value).compare("not")
      ) {

         std::string property = strToLower(t.value);

         // If a property is preceded by not, flag it so we can record the
         // negation when the property is parsed
         if (0 == property.compare("not")) {
            negated = true;
            t = lexer.next();
            continue;
         }

         else if (adjectives.end() != adjectives.find(property)) {
            throw ParseException(
               "(line " + std::to_string(t.lineno) + "): "
               + "'" + property
               + "' is only an adjective that can be checked during runtime, not a property that can be set (line "
               + std::to_string(t.lineno) + ')'
            );
         }

         propertyList.push_back({property, negated});

         for (const auto &identifier: identifiers) {

            auto &entityProperties = std::get<1>(entities[identifier]);

            // Check to see if the source code is trying to set a property,
            // followed again by its negation
            if (
               entityProperties.end() != entityProperties.find(property) &&
               negated != entityProperties[property].first
            ) {
               if (t.lineno != entityProperties[property].second) {
                  throw ParseException(
                     std::string("You stated on line ") + std::to_string(t.lineno)
                     + " that " + identifier + " is " + (negated ? "not " : "")
                     + property + ", but previously, on line "
                     + std::to_string(entityProperties[property].second)
                     + ", you stated that " + identifier
                     + " is " + (entityProperties[property].first ? "not " : "")
                     + property + ". This seems to be a contradiction."
                  );
               } else {
                  throw ParseException(
                     std::string("You stated on line ") + std::to_string(t.lineno)
                     + " that " + identifier + " is " + (negated ? "not " : "")
                     + property + ", but on the same line, you also stated that "
                     + identifier + " is " + (entityProperties[property].first ? "not " : "")
                     + property + ". This seems to be a contradiction."
                  );
               }
            }

            else {

               // If the boolean value is unset, it means that contrary wasn't
               // set on the entity at all and we can skip over it when checking
               // for contradictions.
               std::unordered_map<std::string, std::optional<bool>> contraries;

               // Initialize list of all possible contraries
               for (const auto &contrary: properties[property].second) {
                  contraries[contrary] = std::nullopt;
               }

               // Record in that list any contraries which have been set (in a
               // positive or negative state)
               for (auto &contrary: contraries) {
                  if (entityProperties.end() != entityProperties.find(contrary.first)) {
                     contrary.second = entityProperties[contrary.first].first;
                  }
               }

               // The property was negated, so if there are any contraries, make
               // sure we don't have the situation where a property and all of its
               // contraries are negated, which would also be a contradiction.
               if (negated) {

                  bool wereGood = false;

                  std::set<size_t> linenos = {t.lineno};
                  std::vector<std::string> propertyStrs = {
                     std::string(negated ? "not " : "") + property
                  };

                  // Careful: contrary.second is true if the property is negated,
                  // not the other way around
                  for (const auto &contrary: contraries) {

                     if (!contrary.second || !*contrary.second) {
                        wereGood = true;
                        break;
                     }

                     if (entityProperties.end() != entityProperties.find(contrary.first)) {

                        propertyStrs.push_back(
                           std::string(entityProperties[contrary.first].first ? "not " : "")
                           + contrary.first
                        );

                        if (linenos.end() == linenos.find(entityProperties[contrary.first].second)) {
                           linenos.insert(entityProperties[contrary.first].second);
                        }
                     }
                  }

                  if (!wereGood) {

                     std::string lineNosStr = linenos.size() > 1 ? "lines " : "line ";
                     std::string propertyStr = "";

                     for (auto [i, it] = std::tuple(static_cast<size_t>(0), linenos.begin()); it != linenos.end(); it++, i++) {
                        lineNosStr += std::to_string(*it);
                        lineNosStr += linenos.size() > 1 && i < linenos.size() - 2 ? ", " : i < linenos.size() - 1 ? " and " : "";
                     }

                     for (size_t i = 0; i < propertyStrs.size(); i++) {
                        propertyStr += propertyStrs[i];
                        propertyStr += (i < propertyStrs.size() - 2 ? ", " : i < propertyStrs.size() - 1 ? " and " : "");
                     }

                     throw ParseException(
                        "On " + lineNosStr + ", you stated that " + identifier
                        + " is " + propertyStr
                        + ", but this seems to be a contradiction."
                     );
                  }
               }

               // If the property was not negated, then make sure we don't have
               // any of its contraries set in a non-negated state.
               else {

                  // Careful: contrary.second is true if the property is negated,
                  // not the other way around
                  for (const auto &contrary: contraries) {
                     if (contrary.second && !*contrary.second) {
                        if (t.lineno != entityProperties[contrary.first].second) {
                           throw ParseException(
                              std::string("You stated on line ") + std::to_string(t.lineno)
                              + " that " + identifier + " is " + (negated ? "not " : "")
                              + property + ", but previously, on line "
                              + std::to_string(entityProperties[contrary.first].second)
                              + ", you stated that " + identifier
                              + " is " + (entityProperties[contrary.first].first ? "not " : "")
                              + contrary.first + ". This seems to be a contradiction."
                           );
                        } else {
                           throw ParseException(
                              std::string("You stated on line ") + std::to_string(t.lineno)
                              + " that " + identifier + " is " + (negated ? "not " : "")
                              + property + ", but on the same line, you also stated that "
                              + identifier + " is "
                              + (entityProperties[contrary.first].first ? "not " : "")
                              + contrary.first + ". This seems to be a contradiction."
                           );
                        }
                     }
                  }
               }
            }

            // There are no contradictions, so set the property on the entity
            std::get<1>(entities[identifier])[property] = {negated, t.lineno};
         }

         t = lexer.next();

         // Properties might be delimited by a comma and/or "and" rather than
         // just a space
         if (COMMA == t.type) {
            t = lexer.next();
         }

         if (AND == t.type) {
            t = lexer.next();
         }

         // Reset the negated flag for the next property
         if (0 != property.compare("not")) {
            negated = false;
         }
      }

      lexer.push(t);
      return propertyList;
   }

   /**************************************************************************/

   void Inform7Parser::parseInClause(std::vector<std::string> subjects) {

      Token t;

      // Grab list of containers or places where the subject(s) should go (there
      // should only be one, and if there are more we'll report it as an error)
      std::vector<std::string> containersOrPlaces = parseIdentifiersList(false);

      if (!containersOrPlaces.size()) {
         throw ParseException(
            "(line " + std::to_string(t.lineno) + "): "
            + "You said " + vectorToStr(subjects) + " "
            + (subjects.size() > 1 ? "are" : "is")
            + " in a container or a place without saying what that container or "
            + "place is."
         );
      }

      else if (containersOrPlaces.size() > 1) {
         throw ParseException(
            "(line " + std::to_string(t.lineno) + "): "
            + "You said " + vectorToStr(subjects)
            + (subjects.size() > 1 ? " are" : " is")
            + " in " + vectorToStr(containersOrPlaces) + ", but "
            + (subjects.size() > 1 ? "they" : "it")
            + " can only be in one container or place at a time."
         );
      }

      else {

         for (const auto &subject: subjects) {

            auto &subjectKinds = std::get<0>(entities[subject]);

            // If we haven't already given the subject entity a kind, default to
            // "thing", which can go inside of a room or container.
            if (!subjectKinds.size()) {
               subjectKinds.insert(std::get<0>(kindsMap["thing"]));
            }

            else {

               for (auto it = subjectKinds.begin(); it != subjectKinds.end(); ) {
                  if (!(*it)->isKindRelated(std::get<0>(kindsMap["thing"]))) {
                     it = subjectKinds.erase(it);
                  } else {
                     it++;
                  }
               }

               // If, after removing all incompatible types from the list of
               // possible kinds, no kinds are left, we obviously have a
               // contradiction.
               if (!subjectKinds.size()) {
                  throw ParseException(
                     subject + " is not a physical thing, yet on line "
                     + std::to_string(t.lineno) + ", you attempt to place it inside "
                     + containersOrPlaces[0] + ". This makes no sense and is not allowed."
                  );
               }
            }
         }

         for (const auto &containerOrPlace: containersOrPlaces) {

            auto &containerOrPlaceKinds = std::get<0>(entities[containerOrPlace]);

            // If we haven't already given the containing thing a kind, default
            // to either "room" or "container", since both can contain things.
            if (!containerOrPlaceKinds.size()) {
               containerOrPlaceKinds.insert(std::get<0>(kindsMap["container"]));
               containerOrPlaceKinds.insert(std::get<0>(kindsMap["room"]));
            }

            else {

               // If this is set to true, it means that there's an entity of
               // type "thing" that needs to be automatically promoted to
               // "container."
               bool promoteThingToContainer = false;

               for (auto it = containerOrPlaceKinds.begin(); it != containerOrPlaceKinds.end(); ) {

                  if (
                     !(*it)->isKindRelated(std::get<0>(kindsMap["container"])) &&
                     !(*it)->isKindRelated(std::get<0>(kindsMap["room"]))
                  ) {

                     // We have a generic "thing" that we're going to promote to
                     // "container"; we'll remove the generic "thing" kind now,
                     // then add the more specific type at the end of the loop.
                     if (*it == std::get<0>(kindsMap["thing"])) {
                        promoteThingToContainer = true;
                     }

                     it = containerOrPlaceKinds.erase(it);
                  }

                  else {
                     it++;
                  }
               }

               if (promoteThingToContainer) {
                  containerOrPlaceKinds.insert(std::get<0>(kindsMap["container"]));
               }

               if (!containerOrPlaceKinds.size()) {

                  std::string subjectsStr = vectorToStr(subjects);
                  std::string toBe = subjects.size() > 1 ? "are" : "is";

                  throw ParseException(
                     std::string("You stated that '")
                     + subjectsStr + ' ' + toBe + " in " + containerOrPlace
                     + "' (line " + std::to_string(t.lineno)
                     + "), but " + containerOrPlace
                     + " is neither a room nor a container and therefore cannot contain things."
                  );
               }
            }
         }
      }
   }

   /**************************************************************************/

   void Inform7Parser::parseOnClause(std::vector<std::string> subjects) {

      Token t;

      // Grab list of supports where the subject(s) should go (there should only
      // be one, and if there are more we'll report it as an error)
      std::vector<std::string> supporters = parseIdentifiersList(false);

      if (!supporters.size()) {
         throw ParseException(
            "(line " + std::to_string(t.lineno) + "): "
            + "You said " + vectorToStr(subjects) + " "
            + (subjects.size() > 1 ? "are" : "is")
            + " on a supporter without saying what that supporter is."
         );
      }

      else if (supporters.size() > 1) {
         throw ParseException(
            "(line " + std::to_string(t.lineno) + "): "
            + "You said " + vectorToStr(subjects)
            + (subjects.size() > 1 ? " are" : " is") +
            + " on " + vectorToStr(supporters) + ", but "
            + (subjects.size() > 1 ? "they" : "it")
            + " can only be on one supporter at a time."
         );
      }

      else {

         for (const auto &subject: subjects) {

            auto &subjectKinds = std::get<0>(entities[subject]);

            // If we haven't already given the subject entity a kind, default to
            // "thing", which can go inside of a room or container.
            if (!subjectKinds.size()) {
               subjectKinds.insert(std::get<0>(kindsMap["thing"]));
            }

            else {

               for (auto it = subjectKinds.begin(); it != subjectKinds.end(); ) {
                  if (!(*it)->isKindRelated(std::get<0>(kindsMap["thing"]))) {
                     it = subjectKinds.erase(it);
                  } else {
                     it++;
                  }
               }

               // If, after removing all incompatible types from the list of
               // possible kinds, no kinds are left, we obviously have a
               // contradiction.
               if (!subjectKinds.size()) {
                  throw ParseException(
                     subject + " is not a physical thing, yet on line "
                     + std::to_string(t.lineno) + ", you attempt to place it on "
                     + supporters[0] + ". This makes no sense and is not allowed."
                  );
               }
            }
         }

         for (const auto &supporter: supporters) {

            auto &supporterKinds = std::get<0>(entities[supporter]);

            if (!supporterKinds.size()) {
               supporterKinds.insert(std::get<0>(kindsMap["supporter"]));
            }

            else {

               // If this is set to true, it means that there's an entity of
               // type "thing" that needs to be automatically promoted to
               // "supporter."
               bool promoteThingToSupporter = false;

               for (auto it = supporterKinds.begin(); it != supporterKinds.end(); ) {

                  if (!(*it)->isKindRelated(std::get<0>(kindsMap["supporter"]))) {

                     // We have a generic "thing" that we're going to promote to
                     // "container"; we'll remove the generic "thing" kind now,
                     // then add the more specific type at the end of the loop.
                     if (*it == std::get<0>(kindsMap["thing"])) {
                        promoteThingToSupporter = true;
                     }

                     it = supporterKinds.erase(it);
                  }

                  else {
                     it++;
                  }
               }

               if (promoteThingToSupporter) {
                  supporterKinds.insert(std::get<0>(kindsMap["supporter"]));
               }

               if (!supporterKinds.size()) {

                  std::string subjectsStr = vectorToStr(subjects);
                  std::string toBe = subjects.size() > 1 ? "are" : "is";

                  throw ParseException(
                     std::string("You stated that '")
                     + subjectsStr + ' ' + toBe + " on " + supporter
                     + "' (line " + std::to_string(t.lineno)
                     + "), but " + supporter
                     + " is not a supporter and therefore cannot support things."
                  );
               }
            }
         }
      }
   }

   /**************************************************************************/

   void Inform7Parser::parseLocationClause(
      std::vector<std::string> identifiers,
      std::string direction
   ) {

      Token t = lexer.next();

      if (0 != strToLower(t.value).compare("of") && 0 != strToLower(t.value).compare("from")) {

         std::string rightHandSide;

         for (t = lexer.peek(); PHRASE_TERMINATOR != t.type; t = lexer.next()) {

            if (rightHandSide.length()) {
               rightHandSide += ' ';
            }

            rightHandSide += t.value;
         }

         throw ParseException(
            "(line " + std::to_string(t.lineno) + "): "
            + "The sentence appears to say that two different things, '"
            + identifiers[0] + "' and '" + rightHandSide
            + "', are the same, which doesn't make sense."
         );
      }

      else if (identifiers.size() > 1) {
         throw ParseException(
            "(line " + std::to_string(t.lineno) + "): "
            + vectorToStr(identifiers) + " cannot "
            + (identifiers.size() > 2 ? "all" : "both") + " be " + direction
            + " of the same place."
         );
      }

      std::vector<std::string> connections = parseIdentifiersList();

      if (connections.size() > 1) {
         throw ParseException(
            "(line " + std::to_string(t.lineno) + "): "
            + identifiers[0] + " cannot be " + direction
            + " of more than one place."
         );
      }

      auto &subjectKinds = std::get<0>(entities[identifiers[0]]);

      // If we haven't already given the subject entity a kind, default to
      // "room."
      if (!subjectKinds.size()) {
         subjectKinds.insert(std::get<0>(kindsMap["room"]));
      }

      else {

         for (auto it = subjectKinds.begin(); it != subjectKinds.end(); ) {
            if (
               !(*it)->isKindRelated(std::get<0>(kindsMap["room"])) &&
               !(*it)->isKindRelated(std::get<0>(kindsMap["door"]))
            ) {
               it = subjectKinds.erase(it);
            } else {
               it++;
            }
         }

         // If, after removing all incompatible types from the list of
         // possible kinds, no kinds are left, we obviously have a
         // contradiction.
         if (!subjectKinds.size()) {
            throw ParseException(
               std::string("You stated that ") + identifiers[0]
               + " is not a door or a room, but in another sentence, you stated that '"
               + identifiers[0] + " is " + direction + " of " + connections[0]
               + "' (line " + std::to_string(t.lineno) + "). " + identifiers[0]
               + " must be a room or a door in order to be reached via a map connection."
            );
         }
      }

      auto &connectionKinds = std::get<0>(entities[connections[0]]);

      if (!connectionKinds.size()) {
         connectionKinds.insert(std::get<0>(kindsMap["room"]));
      }

      else {

         for (auto it = connectionKinds.begin(); it != connectionKinds.end(); ) {
            if (
               !(*it)->isKindRelated(std::get<0>(kindsMap["room"])) &&
               !(*it)->isKindRelated(std::get<0>(kindsMap["door"]))
            ) {
               it = connectionKinds.erase(it);
            } else {
               it++;
            }
         }

         if (!connectionKinds.size()) {
            throw ParseException(
               std::string("You stated that ") + connections[0]
               + " is not a door or a room, but in another sentence, you stated that '"
               + identifiers[0] + " is " + direction + " of " + connections[0]
               + "' (line " + std::to_string(t.lineno) + "). " + connections[0]
               + " must be a room or a door in order to be reached via a map connection."
            );
         }
      }

      // Both the subject and the direct object can't be doors
      if (
         1 == subjectKinds.size() && 
         1 == connectionKinds.size() &&
         *subjectKinds.begin() == std::get<0>(kindsMap["door"]) &&
         *connectionKinds.begin() == std::get<0>(kindsMap["door"])
      ) {
         throw ParseException(
            identifiers[0]
            + " seems to be a door opening onto something not a room, but a door must connect one or two rooms (and in particular is not allowed to connect to another door.)"
         );
      }

      // TODO: record connections in new data structure
      std::cout << "Stub: parseLocationClause: add connections to data structure" << std::endl;
   }

   /**************************************************************************/

   void Inform7Parser::parseDescription(std::vector<std::string> identifiers) {

      Token t = lexer.next();

      // A description of the thing being described was included
      if (QUOTED_STRING == t.type) {

         std::string description = t.value;

         if (identifiers.size() > 1) {
            throw ParseException(
               std::string("You wrote '") + description
               + "(line " + std::to_string(t.lineno) + "): but I don't know if "
               + "you're trying to describe " + vectorToStr(identifiers, "or") + "."
            );
         }

         t = lexer.next();

         if (PHRASE_TERMINATOR != t.type && SOURCE_EOF != t.type) {

            std::string combined = "\"" + description + "\"";

            for (; PHRASE_TERMINATOR != t.type && SOURCE_EOF != t.type; t = lexer.next()) {

               if (COMMA != t.type && COLON != t.type && SEMICOLON != t.type) {
                  combined += " ";
               }

               combined += t.value;
            }

            throw ParseException(
               std::string("You wrote '") + combined +
               "(line " + std::to_string(t.lineno) + "): but it looks as if " +
               "perhaps you did not intend that to read as a single " +
               "sentence, and possibly the text in quotes was supposed to " +
               "stand as as a sentence on its own? (The convention is " +
               "that if text ends in a full stop, exclamation or question " +
               "mark, perhaps with a close bracket or quotation mark " +
               "involved as well, then that punctuation mark also closes " +
               "the sentence to which the text belongs: but otherwise the " +
               "words following the quoted text are considered part of " +
               "the same sentence."
            );
         }

         // Set the entity's description
         else {
            std::get<2>(entities[identifiers[0]]) = description;
         }
      }

      else {
         lexer.push(t);
      }
   }

   /**************************************************************************/

   void Inform7Parser::parseDefinition(std::vector<std::string> identifiers) {

      Token t = lexer.peek();

      if (
         kindsMap.end() != kindsMap.find(strToLower(t.value)) ||
         kindPlurals.end() != kindPlurals.find(strToLower(t.value))
      ) {      

         std::string kindName = kindPlurals.end() != kindPlurals.find(strToLower(t.value)) ?
            kindPlurals[strToLower(t.value)] : strToLower(t.value);
         Kind *kind = std::get<0>(kindsMap[kindName]);

         for (const auto &identifier: identifiers) {

            auto &entityKinds = std::get<0>(entities[identifier]);

            // The entity's kind hasn't been declared yet, so we'll do that now
            if (!entityKinds.size()) {
               entityKinds.insert(std::get<0>(kindsMap[kindName]));
            }

            // The entity has already been constrained to one or more possible
            // kinds, so we have to do some detective work to determine whether
            // or not the definition is a contradiction.
            else {

               bool relatedKindFound = false;
               Kind *replacement = nullptr;

               for (const auto &possibleKind: entityKinds) {

                  // The specified kind is either the same or less specific than
                  // one of the entity's already declared possible kinds.
                  if (possibleKind->isKindRelated(kind)) {
                     relatedKindFound = true;
                     replacement = possibleKind;
                     break;
                  }

                  // The specified kind is more specific than one of the
                  // entity's possible kinds, so replace the old with the new.
                  else if (kind->isKindRelated(possibleKind)) {
                     relatedKindFound = true;
                     replacement = kind;
                     break;
                  }
               }

               // Oh no! The program tried to say that the entity is a kind
               // that's unrelated to any of the possible kinds already declared
               // and must now smite the evil developer with the compiler error
               // of death! :-O
               if (!relatedKindFound) {

                  if (entityKinds.size() > 1) {

                     std::vector<std::string> kindNames;

                     for (const auto &kind: entityKinds) {
                        kindNames.push_back(kind->getName());
                     }

                     throw ParseException(
                        std::string("You stated on line ")
                        + std::to_string(t.lineno) + " that " + identifier
                        + " is a " + kind->getName()
                        + ", but due to previous statements, it must be a "
                        + vectorToStr(kindNames, "or") + "."
                     );
                  }

                  else {
                     throw ParseException(
                        std::string("You stated '") + identifier + " is a "
                        + kind->getName() + "' (line " + std::to_string(t.lineno)
                        + "), but previously, you stated that '" + identifier
                        + " is a " + (*entityKinds.begin())->getName()
                        + "', and a " + kind->getName() + " and a "
                        + (*entityKinds.begin())->getName() + " are incompatible."
                     );
                  }
               }

               // This has two purposes: one is to lock down the entity to a
               // single kind (for example, after parsing an <in clause>, an
               // entity could be a container or a room, and a definition
               // statement would then force it to be one or the other. The
               // other is to replace a less specific kind with a more specific
               // kind.
               else if (replacement) {
                  entityKinds.clear();
                  entityKinds.insert(replacement);
               }
            }
         }

         t = lexer.next();

         if (0 == strToLower(t.value).compare("in")) {
            parseInClause(identifiers);
         }

         else if (0 == strToLower(t.value).compare("on")) {
            parseOnClause(identifiers);
         }

         else {
            lexer.push(t);
         }
      }

      else if (directions.end() != directions.find(strToLower(t.value))) {
         parseLocationClause(identifiers, strToLower(t.value));
      }

      lexer.next();
   }

   /**************************************************************************/

   void Inform7Parser::parsePlacement(std::vector<std::string> subjects) {

      Token t = lexer.peek();

      if (0 == strToLower(t.value).compare("in")) {
         parseInClause(subjects);
      }

      else {
         parseOnClause(subjects);
      }

      lexer.next();
   }

   /**************************************************************************/

   void Inform7Parser::parseEquality(std::vector<std::string> identifiers) {

      Token t;

      // Skip past any articles that might be present
      for (t = lexer.next(); ARTICLE == t.type; t = lexer.next());
      lexer.push(t);

      if (
         properties.end() != properties.find(strToLower(t.value)) ||
         adjectives.end() != adjectives.find(strToLower(t.value)) ||
         0 == strToLower(t.value).compare("not")
      ) {

         std::vector<Inform7Parser::ParsedProperty> propertyList;

         // If we have a list of properties, parse them. We're breaking away
         // from a standard recursive descent in order to avoid too much
         // lookahead.
         parsePropertyList(identifiers);

         t = lexer.next();

         /*
            Previously, the if statement below looked like this:

            if (PHRASE_TERMINATOR == t.type) {
               parsePropertyAssignment(identifiers);
            }

            However, that's no longer necessary because property assignment
            occurs in parsePropertyList(), and if we're matching the
            <property assignment> production in the EBNF, we've already reached
            the phrase terminator and no longer have anything left in the
            sentence to parse.
         */
         if (PHRASE_TERMINATOR == t.type) {
            lexer.next();
         }

         else if (WORD == t.type) {
            parseDefinition(identifiers);
         }

         else if (COMMA == t.type) {
            throw ParseException(
               "(line " + std::to_string(t.lineno) + "): "
               + "Your sentence on line " + std::to_string(t.lineno)
               + " seems to refer to something whose name begins with a comma, which is forbidden."
            );
         }

         else {
            throw ParseException(
               std::string("Illegal character on line ")
               + std::to_string(t.lineno) + "."
            );
         }
      }

      // We can define something in one of two ways. Either we can explicitly
      // say that one or more things are of a certain type, or we can
      // instantiate a room implicitly by saying that the name of a room is in a
      // certain direction from another already defined room.
      else if (
         kindsMap.end() != kindsMap.find(strToLower(t.value)) ||
         kindPlurals.end() != kindPlurals.find(strToLower(t.value)) ||
         directions.end() != directions.find(strToLower(t.value))
      ) {
         lexer.next();
         parseDefinition(identifiers);
      }

      else if (0 == strToLower(t.value).compare("in") ||
      0 == strToLower(t.value).compare("on")) {
         lexer.next();
         parsePlacement(identifiers);
      }

      else if (WORD == t.type || EQUALITY == t.type || AND == t.type) {
         throw ParseException(
            "(line " + std::to_string(t.lineno) + "): "
            + "'" + t.value
            + "' is not a known kind of thing or property of a thing."
         );
      }

      else if (QUOTED_STRING == t.type) {
         throw ParseException(
            "(line " + std::to_string(t.lineno) + "): "
            + "The sentence appears to say that one or more things are equal to a value, but that makes no sense."
         );
      }

      else {
         throw ParseException(
            "(line " + std::to_string(t.lineno) + "): "
            + "I can't find a verb that I know how to deal with."
         );
      }

      t = lexer.peek();

      if (PHRASE_TERMINATOR == t.type) {
         parseDescription(identifiers);
      } else {
         lexer.push(t);
      }
   }

   /**************************************************************************/

   void Inform7Parser::parsePhrase() {

      Token t;

      // We're going to break away from strict LL parsing for a moment, because
      // otherwise there would be too much lookahead
      std::vector<std::string> identifiers = parseIdentifiersList();
      t = lexer.next();

      // This is a bit hacky, but it works and results in minimal changes to my
      // code. If relying on parseIdentifiersList() ever causes me problems in
      // the future, I'll revisit this.
      size_t descriptionOfIndex;
      if (1 == identifiers.size() && 0 == (descriptionOfIndex = identifiers[0].find("description of "))) {
         parseDescriptionStatement(identifiers[0].substr(15));
      }

      else if (!identifiers.size()) {
         throw ParseException(
            "(line " + std::to_string(t.lineno) + "): "
            + "You've used a verb ('" + t.value
            + "') without a subject."
         );
      }

      // We're parsing an expression with an "is" or "are" verb
      else if (EQUALITY == t.type) {
         parseEquality(identifiers);
      }

      else {
         throw ParseException(
            "(line " + std::to_string(t.lineno) + "): "
            + "I can't find a verb that I know how to deal with."
         );
      }
   }

   /**************************************************************************/

   void Inform7Parser::parseProgram() {

      Token t = lexer.next();
      lexer.push(t);

      if (QUOTED_STRING == t.type) {
         parseBibliographic();
      }

      for (t = lexer.next(); SOURCE_EOF != t.type; t = lexer.next()) {
         lexer.push(t);
         parsePhrase();
      }
   }

   /**************************************************************************/

   void Inform7Parser::parse(std::string filename) {

      lexer.open(filename);
      parseProgram();

      // TODO: start using the AST (undefined so far) to populate the game
   }
}
