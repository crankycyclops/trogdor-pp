#include <memory>
#include <vector>

#include <trogdor/utility.h>
#include <trogdor/vocabulary.h>
#include <trogdor/exception/parseexception.h>
#include <trogdor/entities/object.h>

#include <trogdor/parser/parsers/inform7parser.h>

namespace trogdor {


   Inform7Parser::Inform7Parser(
      std::unique_ptr<Instantiator> i,
      const Vocabulary &v,
      const Trogerr &e
   ): Parser(std::move(i), v, e), lexer(directions, adjectives,
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

      specialIdentifiers.insert("nowhere");
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
      insertKind(kinds.get(), "direction", "inform7_direction", entity::ENTITY_UNDEFINED,
      [&] (size_t lineno) {

         // This is just temporary for our current subset of Inform 7; we can
         // define custom directions internally already, and when I'm ready to
         // support this in my parser, I can implement this callback.
         throw UndefinedException(
            "Custom directions aren't yet supported (line "
            + std::to_string(lineno) + ')'
         );
      });

      insertKind(kinds.get(), "object", "inform7_object", entity::ENTITY_UNDEFINED,
      [&] (size_t lineno) {

         throw UndefinedException(
            "The 'object' kind shouldn't be directly instantiable. Instead, entities should default to things or some other more specific type. This is a bug in the Inform 7 parser and should be fixed. (line "
            + std::to_string(lineno) + ')'
         );
      });

      insertKind(std::get<0>(kindsMap["object"]), "region", "inform7_region",
      entity::ENTITY_UNDEFINED, [&] (size_t lineno) {

         throw UndefinedException(
            "Inform 7 regions aren't currently supported (line "
            + std::to_string(lineno) + ')'
         );
      });

      insertKind(std::get<0>(kindsMap["object"]), "room", "inform7_room",
      entity::ENTITY_ROOM, [&] (size_t lineno) {

         ast->appendChild(ASTDefineEntityClass(
            "inform7_room",
            entity::Entity::typeToStr(entity::ENTITY_ROOM),
            lineno
         ));
      });

      insertKind(std::get<0>(kindsMap["object"]), "thing", "inform7_thing",
      entity::ENTITY_OBJECT, [&] (size_t lineno) {

         ast->appendChild(ASTDefineEntityClass(
            "inform7_thing",
            entity::Entity::typeToStr(entity::ENTITY_OBJECT),
            lineno
         ));
      });

      insertKind(std::get<0>(kindsMap["thing"]), "door", "inform7_door",
      entity::ENTITY_OBJECT, [&] (size_t lineno) {

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

      insertKind(std::get<0>(kindsMap["thing"]), "container", "inform7_container",
      entity::ENTITY_OBJECT, [&] (size_t lineno) {

         // TODO: issue warning about only partial support
         ast->appendChild(ASTDefineEntityClass(
            "inform7_container",
            entity::Entity::typeToStr(entity::ENTITY_OBJECT),
            lineno
         ));
      });

      insertKind(std::get<0>(kindsMap["container"]), "vehicle", "inform7_vehicle",
      entity::ENTITY_OBJECT, [&] (size_t lineno) {

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
      "inform7_players_holdall", entity::ENTITY_OBJECT, [&] (size_t lineno) {

         // TODO: issue warning about only partial support
         ast->appendChild(ASTDefineEntityClass(
            "inform7_players_holdall",
            entity::Entity::typeToStr(entity::ENTITY_OBJECT),
            lineno
         ));
      });

      insertKind(std::get<0>(kindsMap["thing"]), "supporter", "inform7_supporter",
      entity::ENTITY_OBJECT, [&] (size_t lineno) {

         // TODO: issue warning about only partial support
         ast->appendChild(ASTDefineEntityClass(
            "inform7_supporter",
            entity::Entity::typeToStr(entity::ENTITY_OBJECT),
            lineno
         ));
      });

      insertKind(std::get<0>(kindsMap["thing"]), "backdrop", "inform7_backdrop",
      entity::ENTITY_UNDEFINED, [&] (size_t lineno) {

         throw UndefinedException(
            "Inform 7 backdrops aren't yet supported (line "
            + std::to_string(lineno) + ')'
         );
      });

      insertKind(std::get<0>(kindsMap["thing"]), "device", "inform7_device",
      entity::ENTITY_OBJECT, [&] (size_t lineno) {

         // TODO: issue warning about only partial support
         ast->appendChild(ASTDefineEntityClass(
            "inform7_device",
            entity::Entity::typeToStr(entity::ENTITY_OBJECT),
            lineno
         ));
      });

      insertKind(std::get<0>(kindsMap["thing"]), "person", "inform7_person",
      entity::ENTITY_CREATURE, [&] (size_t lineno) {

         ast->appendChild(ASTDefineEntityClass(
            "inform7_person",
            entity::Entity::typeToStr(entity::ENTITY_CREATURE),
            lineno
         ));
      });

      insertKind(std::get<0>(kindsMap["person"]), "man", "inform7_man",
      entity::ENTITY_CREATURE, [&] (size_t lineno) {

         ast->appendChild(ASTDefineEntityClass(
            "inform7_man",
            entity::Entity::typeToStr(entity::ENTITY_CREATURE),
            lineno
         ));
      });

      insertKind(std::get<0>(kindsMap["person"]), "woman", "inform7_woman",
      entity::ENTITY_CREATURE, [&] (size_t lineno) {

         ast->appendChild(ASTDefineEntityClass(
            "inform7_woman",
            entity::Entity::typeToStr(entity::ENTITY_CREATURE),
            lineno
         ));
      });

      insertKind(std::get<0>(kindsMap["person"]), "animal", "inform7_animal",
      entity::ENTITY_CREATURE, [&] (size_t lineno) {

         ast->appendChild(ASTDefineEntityClass(
            "inform7_animal",
            entity::Entity::typeToStr(entity::ENTITY_CREATURE),
            lineno
         ));
      });

      // Map purely abstract kinds to their default instantiable kinds
      abstractKinds["object"] = std::get<0>(kindsMap["thing"]);

      // Built-in properties that Inform 7 recognizes for things.
      insertProperty(
         "lit",
         {std::get<0>(kindsMap["thing"])},
         [&] (std::string, size_t, bool) {
            // TODO
         }, {"unlit"}
      );

      insertProperty(
         "unlit",
         {std::get<0>(kindsMap["thing"])},
         [&] (std::string, size_t, bool) {
            // TODO
         }, {"lit"}
      );

      insertProperty(
         "transparent",
         {std::get<0>(kindsMap["thing"])},
         [&] (std::string, size_t, bool) {
            // TODO
         }, {"opaque"}
      );

      insertProperty(
         "opaque",
         {std::get<0>(kindsMap["thing"])},
         [&] (std::string, size_t, bool) {
            // TODO
         }, {"transparent"}
      );

      insertProperty(
         "fixed in place",
         {std::get<0>(kindsMap["thing"])},
         [&] (std::string, size_t, bool) {
            // TODO
         }, {"portable"}
      );

      insertProperty(
         "portable",
         {std::get<0>(kindsMap["thing"])},
         [&] (std::string, size_t, bool) {
            // TODO
         }, {"fixed in place"}
      );

      insertProperty(
         "openable",
         {std::get<0>(kindsMap["thing"])},
         [&] (std::string, size_t, bool) {
            // TODO
         }, {"unopenable"}
      );

      insertProperty(
         "unopenable",
         {std::get<0>(kindsMap["thing"])},
         [&] (std::string, size_t, bool) {
            // TODO
         }, {"openable"}
      );

      insertProperty(
         "edible",
         {std::get<0>(kindsMap["thing"])},
         [&] (std::string, size_t, bool) {
            // TODO
         }, {"inedible"}
      );

      insertProperty(
         "inedible",
         {std::get<0>(kindsMap["thing"])},
         [&] (std::string, size_t, bool) {
            // TODO
         }, {"edible"}
      );

      insertProperty(
         "described",
         {std::get<0>(kindsMap["thing"])},
         [&] (std::string, size_t, bool) {
            // TODO
         }, {"undescribed"}
      );

      insertProperty(
         "undescribed",
         {std::get<0>(kindsMap["thing"])},
         [&] (std::string, size_t, bool) {
            // TODO
         }, {"described"}
      );

      insertProperty(
         "enterable",
         {std::get<0>(kindsMap["thing"])},
         [&] (std::string, size_t, bool) {
            // TODO
         }
      );

      insertProperty(
         "wearable",
         {std::get<0>(kindsMap["thing"])},
         [&] (std::string, size_t, bool) {
            // TODO
         }
      );

      insertProperty(
         "pushable between rooms",
         {std::get<0>(kindsMap["thing"])},
         [&] (std::string, size_t, bool) {
            // TODO
         }
      );

      // Built-in properties that Inform 7 recognizes for rooms.
      insertProperty(
         "lighted",
         {std::get<0>(kindsMap["room"])},
         [&] (std::string, size_t, bool) {
            // TODO
         }, {"dark"}
      );

      insertProperty(
         "dark",
         {std::get<0>(kindsMap["room"])},
         [&] (std::string, size_t, bool) {
            // TODO
         }, {"lighted"}
      );

      // Built-in properties that Inform 7 recognizes for doors.
      insertProperty(
         "locked",
         {std::get<0>(kindsMap["door"])},
         [&] (std::string, size_t, bool) {
            // TODO
         }, {"unlocked"}
      );

      insertProperty(
         "unlocked",
         {std::get<0>(kindsMap["door"])},
         [&] (std::string, size_t, bool) {
            // TODO
         }, {"locked"}
      );

      insertProperty(
         "lockable",
         {std::get<0>(kindsMap["door"])},
         [&] (std::string, size_t, bool) {
            // TODO
         }, {"unlockable"}
      );

      insertProperty(
         "unlockable",
         {std::get<0>(kindsMap["door"])},
         [&] (std::string, size_t, bool) {
            // TODO
         }, {"lockable"}
      );

      // Built-in properties that Inform 7 recognizes for devices.
      insertProperty(
         "switched on",
         {std::get<0>(kindsMap["device"])},
         [&] (std::string, size_t, bool) {
            // TODO
         }, {"switched off"}
      );

      insertProperty(
         "switched off",
         {std::get<0>(kindsMap["device"])},
         [&] (std::string, size_t, bool) {
            // TODO
         }, {"switched on"}
      );

      // Built-in properties that Inform 7 recognizes for persons.
      insertProperty(
         "male",
         {std::get<0>(kindsMap["person"])},
         [&] (std::string, size_t, bool) {
            // TODO
         }, {"female", "neuter"}
      );

      insertProperty(
         "female",
         {std::get<0>(kindsMap["person"])},
         [&] (std::string, size_t, bool) {
            // TODO
         }, {"male", "neuter"}
      );

      insertProperty(
         "neuter",
         {std::get<0>(kindsMap["person"])},
         [&] (std::string, size_t, bool) {
            // TODO
         }, {"male", "female"}
      );

      // Built-in properties that Inform 7 recognizes for containers and doors.
      insertProperty(
         "open",
         {std::get<0>(kindsMap["door"]), std::get<0>(kindsMap["container"])},
         [&] (std::string, size_t, bool) {
            // TODO
         }, {"closed"}
      );

      insertProperty(
         "closed",
         {std::get<0>(kindsMap["door"]), std::get<0>(kindsMap["container"])},
         [&] (std::string, size_t, bool) {
            // TODO
         }, {"open"}
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

   bool Inform7Parser::filterKinds(
      std::string entity,
      std::vector<Kind *> compatibleKinds,
      Kind *exactMatch
   ) {

      bool foundExactMatch = false;
      auto &entityKinds = std::get<0>(entities[entity]);

      for (auto it = entityKinds.begin(); it != entityKinds.end(); ) {

         bool kindMatches = false;

         for (const auto &kind: compatibleKinds) {
            if ((*it)->isKindRelated(kind)) {
               kindMatches = true;
               break;
            }
         }

         if (!kindMatches) {

            if (exactMatch == *it) {
               foundExactMatch = true;
            }

            it = entityKinds.erase(it);
         }

         else {
            it++;
         }
      }

      return foundExactMatch;
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

         else if (specialIdentifiers.end() != specialIdentifiers.find(identifier)) {
            throw ParseException(
               "You referred to '" + identifier
               + "' as if it were a thing (line " + std::to_string(t.lineno)
               + "), but " + identifier
               + " cannot be made specific, and so cannot have specific properties or be of any given kind."
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
            noun += strToLower(t.value);
         }

         if (noun.length()) {

            identifiers.push_back(noun);
            startingNewNoun = true;

            // Add the identifier to the entities symbol table
            if (
               specialIdentifiers.end() == specialIdentifiers.find(noun) &&
               declareEntities && entities.end() == entities.find(noun)
            ) {
               entities[noun] = {{}, {}, "", t.lineno};
               entitiesOrdered.push_back(noun);
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

            if (specialIdentifiers.end() != specialIdentifiers.find(identifier)) {
               throw ParseException(
                  "You referred to '" + identifier
                  + "' as if it were a thing (line " + std::to_string(t.lineno)
                  + "), but " + identifier
                  + " cannot be made specific, and so cannot have specific properties or be of any given kind."
               );
            }

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
               for (const auto &contrary: std::get<1>(properties[property])) {
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

      Token t = lexer.peek();

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

            if (specialIdentifiers.end() != specialIdentifiers.find(subject)) {
               throw ParseException(
                  "You referred to '" + subject
                  + "' as if it were a thing (line " + std::to_string(t.lineno)
                  + "), but " + subject
                  + " cannot be made specific, and so cannot have specific properties or be of any given kind."
               );
            }

            auto &subjectKinds = std::get<0>(entities[subject]);

            // If we haven't already given the subject entity a kind, default to
            // "thing", which can go inside of a room or container.
            if (!subjectKinds.size()) {
               subjectKinds.insert(std::get<0>(kindsMap["thing"]));
            }

            else {

               filterKinds(subject, {std::get<0>(kindsMap["thing"])});

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

            if (specialIdentifiers.end() != specialIdentifiers.find(containerOrPlace)) {
               throw ParseException(
                  "You referred to '" + containerOrPlace
                  + "' as if it were a thing (line " + std::to_string(t.lineno)
                  + "), but " + containerOrPlace
                  + " cannot be made specific, and so cannot have specific properties or be of any given kind."
               );
            }

            auto &containerOrPlaceKinds = std::get<0>(entities[containerOrPlace]);

            // If we haven't already given the containing thing a kind, default
            // to either "room" or "container", since both can contain things.
            if (!containerOrPlaceKinds.size()) {
               containerOrPlaceKinds.insert(std::get<0>(kindsMap["container"]));
               containerOrPlaceKinds.insert(std::get<0>(kindsMap["room"]));
            }

            else {

               if (filterKinds(
                  containerOrPlace,
                  {std::get<0>(kindsMap["container"]), std::get<0>(kindsMap["room"])},
                  std::get<0>(kindsMap["thing"])
               )) {

                  // If one of the possible types is "thing", it needs to be
                  // automatically promoted to "container."
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

      Token t = lexer.peek();

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

            if (specialIdentifiers.end() != specialIdentifiers.find(subject)) {
               throw ParseException(
                  "You referred to '" + subject
                  + "' as if it were a thing (line " + std::to_string(t.lineno)
                  + "), but " + subject
                  + " cannot be made specific, and so cannot have specific properties or be of any given kind."
               );
            }

            auto &subjectKinds = std::get<0>(entities[subject]);

            // If we haven't already given the subject entity a kind, default to
            // "thing", which can go inside of a room or container.
            if (!subjectKinds.size()) {
               subjectKinds.insert(std::get<0>(kindsMap["thing"]));
            }

            else {

               filterKinds(subject, {std::get<0>(kindsMap["thing"])});

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

            if (specialIdentifiers.end() != specialIdentifiers.find(supporter)) {
               throw ParseException(
                  "You referred to '" + supporter
                  + "' as if it were a thing (line " + std::to_string(t.lineno)
                  + "), but " + supporter
                  + " cannot be made specific, and so cannot have specific properties or be of any given kind."
               );
            }

            auto &supporterKinds = std::get<0>(entities[supporter]);

            if (!supporterKinds.size()) {
               supporterKinds.insert(std::get<0>(kindsMap["supporter"]));
            }

            else {

               if (filterKinds(
                  supporter,
                  {std::get<0>(kindsMap["supporter"])},
                  std::get<0>(kindsMap["thing"])
               )) {

                  // If one of the possible types is "thing", it needs to be
                  // automatically promoted to "supporter."
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

      else if (specialIdentifiers.end() != specialIdentifiers.find(connections[0])) {
         throw ParseException(
            std::string("You stated '") + identifiers[0] + " is " + direction
            + " of " + connections[0] + "' (line " + std::to_string(t.lineno)
            + "), but the source of a map connection can't be " + connections[0]
            + ", so sentences like 'The oak door is north of " + connections[0]
            + ".' are not allowed."
         );
      }

      auto &connectionKinds = std::get<0>(entities[connections[0]]);

      if (!connectionKinds.size()) {
         connectionKinds.insert(std::get<0>(kindsMap["room"]));
         connectionKinds.insert(std::get<0>(kindsMap["door"]));
      }

      else {

         filterKinds(connections[0], {
            std::get<0>(kindsMap["room"]),
            std::get<0>(kindsMap["door"])
         });

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

      // We're removing a connection
      if (0 == identifiers[0].compare("nowhere")) {

         // A door can't connect to nowhere, so remove it from the list of
         // possible kinds
         filterKinds(connections[0], {std::get<0>(kindsMap["room"])});

         // If there are no possible kinds left, we know that we attempted to
         // connect a door to nowhere, which is also a no-no
         if (!connectionKinds.size()) {
            throw ParseException(
               std::string("A door cannot be connected to nowhere (line ")
               + std::to_string(t.lineno) + ".)"
            );
         }

         // If the connection exists, it was explicitly defined, and it was not
         // set to nowhere (empty string), we have a contradiction
         else if (
            entityConnections[connections[0]].end() != entityConnections[connections[0]].find(direction) &&
            std::get<1>(entityConnections[connections[0]][direction]) &&
            connections[0].length() > 0
         ) {
            throw ParseException(
               "You stated that '" + identifiers[0] + " is " + direction + " of "
               + connections[0] + "' (line " + std::to_string(t.lineno)
               + "), but in another sentence you also stated that '"
               + std::get<0>(entityConnections[connections[0]][direction]) + " is " +
               direction + " of " + connections[0] + "', which is a contradiction."
            );
         }

         // Setting the connecting entity to an empty string is how we signal
         // that the connection is to nowhere
         else {
            entityConnections[connections[0]][direction] = {"", true, t.lineno};
         }
      }

      else if (specialIdentifiers.end() != specialIdentifiers.find(identifiers[0])) {
         throw UndefinedException(
            std::string("parseLocationClause: Unsupported special identifier '")
            + connections[0] + "' encountered on line " + std::to_string(t.lineno)
            + ". This is a bug."
         );
      }

      // We're creating a connection
      else {

         auto &subjectKinds = std::get<0>(entities[identifiers[0]]);

         if (!subjectKinds.size()) {
            subjectKinds.insert(std::get<0>(kindsMap["room"]));
            subjectKinds.insert(std::get<0>(kindsMap["door"]));
         }

         else {

            filterKinds(identifiers[0], {
               std::get<0>(kindsMap["room"]),
               std::get<0>(kindsMap["door"])
            });

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

         // We've already explicitly set another connection in the same
         // direction, so we have a contradiction
         if (std::get<1>(entityConnections[connections[0]][direction])) {

            std::string previousSubject = std::get<0>(entityConnections[connections[0]][direction]).length() ?
               std::get<0>(entityConnections[connections[0]][direction]) : "nowhere";

            throw ParseException(
               std::string("You stated that '") + identifiers[0] + " is "
               + direction + " of " + connections[0] + "' (line "
               + std::to_string(t.lineno)
               + "), but in another sentence, you stated that '"
               + previousSubject + " is " + direction + " of " + connections[0]
               + ", which is a contradiction."
            );
         }

         entityConnections[connections[0]][direction] = {identifiers[0], true, t.lineno};

         // If an explicit connection hasn't already been made in the opposite
         // direction, create an implicit one now.
         if (
            directions[direction].length() &&
            !std::get<1>(entityConnections[identifiers[0]][directions[direction]])
         ) {
            entityConnections[identifiers[0]][directions[direction]] = {connections[0], false, t.lineno};
         }
      }
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
                        + "), but previously, you stated or implied that '"
                        + identifier + " is a " + (*entityKinds.begin())->getName()
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

   Kind *Inform7Parser::resolveKind(std::string entityName) const {

      const auto &entity = entities.find(entityName);

      if (1 == std::get<0>(entity->second).size()) {

         std::string kindName = (*std::get<0>(entity->second).begin())->getName();

         if (abstractKinds.end() != abstractKinds.find(kindName)) {
            return abstractKinds.find(kindName)->second;
         } else {
            return *std::get<0>(entity->second).begin();
         }
      }

      else if (2 == std::get<0>(entity->second).size()) {

         Kind *room = std::get<0>(kindsMap.find("room")->second);
         Kind *door = std::get<0>(kindsMap.find("door")->second);
         Kind *container = std::get<0>(kindsMap.find("container")->second);

         if (
            std::get<0>(entity->second).count(room) &&
            std::get<0>(entity->second).count(container)
         ) {
            return container;
         }

         else if (
            std::get<0>(entity->second).count(door) &&
            std::get<0>(entity->second).count(room)
         ) {
            return room;
         }

         else {

            std::string kindsStr;

            for (const auto &kind: std::get<0>(entity->second)) {
               kindsStr += kind->getName() + ", ";
            }

            kindsStr = kindsStr.substr(0, kindsStr.length() - 2);

            throw UndefinedException(
               entityName + " was assigned the following incompatible kinds: "
               + kindsStr + ". This is a bug and should be fixed."
            );
         }
      }

      else if (!std::get<0>(entity->second).size()) {
         throw UndefinedException(
            entityName + " has an empty number of possible kinds. This is a bug and should be fixed."
         );
      }

      else {

         std::string kindsStr;

         for (const auto &kind: std::get<0>(entity->second)) {
            kindsStr += kind->getName() + ", ";
         }

         kindsStr = kindsStr.substr(0, kindsStr.length() - 2);

         throw UndefinedException(
            entityName + " has too many possible kinds: " + kindsStr
            + ". This is a bug and should be fixed."
         );
      }
   }

   /**************************************************************************/

   void Inform7Parser::buildAST() {

      bool atLeastOneRoomFound = false;

      // We internally re-name this room to "start" while maintaining its
      // displayed title
      std::string startRoomName;

      // Keeps track of which kinds an entity resolves to just before it's
      // instantiated
      std::unordered_map<std::string, Kind *> entityToResolvedKind;

      // I iterate over entitiesOrdered instead of entities directly because I need
      // to make sure I'm going in the order in which the entities were declared
      for (const auto &entityName: entitiesOrdered) {

         const auto &entity = *entities.find(entityName);

         Kind *entityKind = resolveKind(entityName);

         std::string description = std::get<2>(entity.second);
         std::string entityClassName = std::get<3>(kindsMap[entityKind->getName()]);
         const auto &entityProperties = std::get<1>(entity.second);
         size_t lineno = std::get<3>(entity.second);

         if (entityKind == std::get<0>(kindsMap["room"])) {

            atLeastOneRoomFound = true;

            if (startRoomName.empty()) {
               startRoomName = entityName;
            }
         }

         entityToResolvedKind[entityName] = entityKind;

         // The kind's internal entity class definitions haven't been inserted
         // into the AST yet, so do that now
         if (bool &classInserted = std::get<2>(kindsMap[entityKind->getName()]); !classInserted) {

            // Inserts the entity class definition itself
            std::get<1>(kindsMap[entityKind->getName()])(lineno);

            // Inserts any additional AST nodes necessary to configure that
            // entity class, starting with the parent kind and working its way
            // down to the child
            std::stack<std::function<void(size_t)>> classCallbacks;

            for (Kind *curKind = entityKind; curKind != nullptr; curKind = curKind->getParent()) {

               const auto &callback = curKind->getCallback();

               if (callback) {
                  classCallbacks.push(curKind->getCallback());
               }
            }

            while (classCallbacks.size()) {
               classCallbacks.top()(lineno);
               classCallbacks.pop();
            }

            classInserted = true;
         }

         std::string astName = 0 == startRoomName.compare(entityName) ? "start" : entityName;

         // Finally, insert the entity definition itself into the AST
         ast->appendChild(ASTDefineEntity(
            astName,
            entity::Entity::typeToStr(entityKind->getInternalType()),
            entityClassName,
            std::nullopt,
            lineno
         ));

         // TODO: I think Inform allows some way to customize thie value
         ast->appendChild(ASTSetProperty(
            "entity",
            "title",
            entityName,
            lineno,
            astName
         ));

         if (description.length()) {

            ast->appendChild(ASTSetProperty(
               "entity",
               "longDesc",
               description,
               lineno,
               astName
            ));

            // TODO: what is Inform 7's shortDesc equivalent?
         }

         // Insert AST nodes for entity's implied properties
         for (Kind *curKind = entityKind; curKind != nullptr; curKind = curKind->getParent()) {
            for (const std::string &property: curKind->getProperties()) {
               std::get<2>(properties[property])(entityName, lineno, false);
            }
         }

         // Insert AST nodes for entity's assigned properties
         for (const auto &property: entityProperties) {

            bool kindIsAllowed = false;

            for (auto const &kind: std::get<0>(properties[property.first])) {
               if (entityKind->isKindRelated(kind)) {
                  kindIsAllowed = true;
                  break;
               }
            }

            if (kindIsAllowed) {
               std::get<2>(properties[property.first])(entityName, property.second.second, property.second.first);
            } else {
               throw ParseException(
                  std::string("You stated that '") + entityName + " is "
                  + (property.second.first ? "not " : "") + property.first
                  + "' (line " + std::to_string(property.second.second)
                  + "), but the property " + property.first + "" +
                  + " is not allowed to exist because you haven't said it is. What properties something can have depends on what kind of thing it is."
               );
            }
         }
      }

      if (!atLeastOneRoomFound) {
         throw ParseException("There doesn't seem to be any location in this story, so there's nowhere for the player to begin.");
      }

      // Insert AST nodes representing connections between doors and rooms
      for (const auto &entityToConnect: entityConnections) {

         for (const auto &connection: entityToConnect.second) {

            std::string direction = connection.first;
            std::string connectTo = std::get<0>(connection.second);

            // A door can only connect to a room, not another door or anything else
            if (
               entityToResolvedKind[entityToConnect.first] == std::get<0>(kindsMap["door"]) &&
               entityToResolvedKind[connectTo] != std::get<0>(kindsMap["room"])
            ) {
               throw ParseException(
                  entityToConnect.first
                  + " seems to be a door opening onto something not a room, but a door must connect one or two rooms, and in particular is not allowed to connect to another door (line "
                  + std::to_string(std::get<3>(entities[entityToConnect.first])) + ".)"
               );
            }

            // We can ignore connections to nowhere (represented by an empty
            // string) since it's just a way for Inform7 to remove an otherwise
            // implied connection.
            if (connectTo.length()) {
               ast->appendChild(ASTConnectRooms(
                  "entity",
                  0 == startRoomName.compare(entityToConnect.first) ? "start" : entityToConnect.first,
                  0 == startRoomName.compare(connectTo) ? "start" : connectTo,
                  direction,
                  std::get<2>(connection.second)
               ));
            }
         }
      }

      // TODO: I still have to build out contains and supports relationships in
      // parseInClause and parseOnClause and then insert the appropriate AST
      // nodes. I also need to take care of scenario where someone names a room
      // "start" but it's not actually supposed to be the starting room. Maybe I
      // could make that a reserved word that's not allowed as an identifier
      // name?) Except then that would violate the rules of Inform 7. So, I
      // think that if I find a room by this name, I should just transparently
      // rename it.
   }

   /**************************************************************************/

   void Inform7Parser::parse(std::string filename) {

      lexer.open(filename);
      parseProgram();
      buildAST();

      instantiator->instantiate(ast);
   }
}
