#include <string>
#include <fstream>

#include <trogdor/utility.h>
#include <trogdor/vocabulary.h>
#include <trogdor/entities/entity.h>

#include <trogdor/exception/validationexception.h>
#include <trogdor/exception/undefinedexception.h>

#include <trogdor/instantiator/instantiator.h>


namespace trogdor {


   // See comment above destructor definition in include/instantiator/instantiator.h.
   Instantiator::~Instantiator() {}

   /***************************************************************************/

   Instantiator::Instantiator(const Vocabulary &v): vocabulary(v) {

      mapPreOperations();
      mapEntityPropValidators();
      mapGamePropValidators();

      // Default Entity classes that should always exist
      symbols.entityClasses["resource"] = entity::ENTITY_RESOURCE;
      symbols.entityClasses["room"] = entity::ENTITY_ROOM;
      symbols.entityClasses["object"] = entity::ENTITY_OBJECT;
      symbols.entityClasses["creature"] = entity::ENTITY_CREATURE;
      symbols.entityClasses["player"] = entity::ENTITY_PLAYER;
   }

   /***************************************************************************/

   void Instantiator::assertBool(const Vocabulary &vocabulary, std::string value) {

      // TODO: also allow strings "false" and "true"
      if (value != "1" && value != "0") {
         throw ValidationException("value must be 1 for true or 0 for false");
      }
   }

   /***************************************************************************/

   void Instantiator::assertInt(const Vocabulary &vocabulary, std::string value) {

      if (!isValidInteger(value)) {
         throw ValidationException("value is not a valid integer");
      }
   }

   /***************************************************************************/

   void Instantiator::assertDouble(const Vocabulary &vocabulary, std::string value) {

      if (!isValidDouble(value)) {
         throw ValidationException("value is not a valid number");
      }
   }

   /***************************************************************************/

   void Instantiator::assertProbability(const Vocabulary &vocabulary, std::string value) {

      std::string errorMsg = "value is not a valid probability (must be between 0 and 1)";

      if (!isValidDouble(value)) {
         throw ValidationException(errorMsg);
      }

      double dValue = stod(value);

      if (dValue < 0 || dValue > 1) {
         throw ValidationException(errorMsg);
      }
   }

   /***************************************************************************/

   void Instantiator::assertString(const Vocabulary &vocabulary, std::string value) {

      // Any string is valid, so never throw an exception
      return;
   }

   /***************************************************************************/

   void Instantiator::assertValidASTArguments(const std::shared_ptr<ASTOperationNode> &operation,
   unsigned int numArgs) {

      if (operation->size() != numArgs) {
         throw ValidationException(
            ASTOperationNode::getOperationStr(operation->getOperation()) +
            ": invalid number of arguments. This is a bug."
         );
      }
   }

   /***************************************************************************/

   void Instantiator::assertValidASTArguments(const std::shared_ptr<ASTOperationNode> &operation,
   unsigned int minNumArgs, unsigned int maxNumArgs) {

      if (operation->size() < minNumArgs || operation->size() > maxNumArgs) {
         throw ValidationException(
            ASTOperationNode::getOperationStr(operation->getOperation()) +
            ": invalid number of arguments. This is a bug."
         );
      }
   }

   /***************************************************************************/

   void Instantiator::assertValidASTArguments(const std::shared_ptr<ASTOperationNode> &operation,
   unsigned int minArgs, std::unordered_map<std::string, unsigned int> targetTypeToNumArgs) {

      if (operation->size() < minArgs) {
         throw ValidationException(
            ASTOperationNode::getOperationStr(operation->getOperation()) +
            ": invalid number of arguments. This is a bug."
         );
      }

      std::string targetType = operation->getChildren()[0]->getValue();

      if (targetTypeToNumArgs.end() == targetTypeToNumArgs.find(targetType)) {
         throw ValidationException(
            ASTOperationNode::getOperationStr(operation->getOperation()) +
            ": invalid target type '" + targetType + "'. This is a bug."
         );
      }

      else if (operation->size() != targetTypeToNumArgs[targetType]) {
         throw ValidationException(
            ASTOperationNode::getOperationStr(operation->getOperation()) +
            ": invalid number of arguments. This is a bug."
         );
      }
   }

   /***************************************************************************/

   void Instantiator::assertTargetExists(std::string targetType, std::string targetName,
   std::string action, int lineNumber) {

      if (0 == targetType.compare("entity")) {

         if (symbols.entities.end() == symbols.entities.find(targetName)) {
            throw ValidationException(
               std::string("cannot " + action + " '") + targetName +
               "' because it hasn't been defined" +
               (lineNumber ? " (line " + std::to_string(lineNumber) + ")" : "")
            );
         }
      }

      else if (0 == targetType.compare("class")) {

         if (symbols.entityClasses.end() == symbols.entityClasses.find(targetName)) {
            throw ValidationException(
               std::string("cannot " + action + " class '") + targetName +
               "' because it hasn't been defined" +
               (lineNumber ? " (line " + std::to_string(lineNumber) + ")" : "")
            );
         }
      }
   }

   /***************************************************************************/

   void Instantiator::mapPreOperations() {

      // Validates and records the definition of custom directions
      preOperations[DEFINE_DIRECTION] = [this](const std::shared_ptr<ASTOperationNode> &operation) {

         assertValidASTArguments(operation, 1);

         std::string direction = operation->getChildren()[0]->getValue();

         if (vocabulary.isDirection(direction)) {
            throw ValidationException(
               std::string("cannot redefine built-in direction '") + direction + "'" +
                  (operation->getLineNumber() ? " (line " +
                  std::to_string(operation->getLineNumber()) + ")" : "")
            );
         }

         else if (customVocabulary.directions.end() != customVocabulary.directions.find(direction)
         ) {
            throw ValidationException(
               std::string("custom direction '") + direction + "' has already been defined" +
                  (operation->getLineNumber() ? " (line " +
                  std::to_string(operation->getLineNumber()) + ")" : "")
            );
         }

         else {
            customVocabulary.directions.insert(direction);
         }
      };

      /**********/

      // Validates and records the definition of custom direction synonyms
      preOperations[DEFINE_DIRECTION_SYNONYM] = [this](const std::shared_ptr<ASTOperationNode> &operation) {

         assertValidASTArguments(operation, 2);

         std::string direction = operation->getChildren()[0]->getValue();
         std::string synonym = operation->getChildren()[1]->getValue();

         if (vocabulary.isDirection(synonym)) {
            throw ValidationException(
               std::string("cannot set built-in direction '") + synonym +
                  "' as a synonym for another direction" +
                  (operation->getLineNumber() ? " (line " +
                  std::to_string(operation->getLineNumber()) + ")" : "")
            );
         }

         else if (customVocabulary.directions.end() != customVocabulary.directions.find(synonym)) {
            throw ValidationException(
               std::string("cannot set custom direction '") + synonym +
                  "' as a synonym for another direction" +
                  (operation->getLineNumber() ? " (line " +
                  std::to_string(operation->getLineNumber()) + ")" : "")
            );
         }

         else if (
            !vocabulary.isDirection(direction) &&
            customVocabulary.directions.end() == customVocabulary.directions.find(direction)
         ) {
            throw ValidationException(
               std::string("'") + synonym + "' cannot be set as a synonym for '" +
                  direction + "' because that direction hasn't been defined" +
                  (operation->getLineNumber() ? " (line " +
                  std::to_string(operation->getLineNumber()) + ")" : "")
            );
         }

         else {
            customVocabulary.directionSynonyms[synonym] = direction;
         }
      };

      /**********/

      // Validates and adds newly defined Entities to the symbol table
      preOperations[DEFINE_VERB_SYNONYM] = [this](const std::shared_ptr<ASTOperationNode> &operation) {

         assertValidASTArguments(operation, 2);

         std::string verb = operation->getChildren()[0]->getValue();
         std::string synonym = operation->getChildren()[1]->getValue();

         if (vocabulary.isVerb(synonym) && !vocabulary.isVerbSynonym(synonym)) {
            throw ValidationException(
               std::string("cannot set '") + synonym +
                  "' as a synonym for a verb because it's already a verb" +
                  (operation->getLineNumber() ? " (line " +
                  std::to_string(operation->getLineNumber()) + ")" : "")
            );
         }

         else {
            customVocabulary.verbSynonyms[synonym] = verb;
         }
      };

      /**********/

      // Validates and adds newly defined Entities to the symbol table
      preOperations[DEFINE_ENTITY] = [this](const std::shared_ptr<ASTOperationNode> &operation) {

         assertValidASTArguments(operation, 3, 4);

         std::string name = operation->getChildren()[0]->getValue();
         std::string className = operation->getChildren()[2]->getValue();
         std::string typeStr = operation->getChildren()[1]->getValue();
         entity::EntityType type = entity::Entity::strToType(typeStr);

         if (4 == operation->getChildren().size() && type != entity::ENTITY_RESOURCE) {
            throw ValidationException("Only resources can have a plural name.");
         }

         else if (entity::ENTITY_UNDEFINED == type) {
            throw ValidationException(std::string("invalid entity type '") +
               operation->getChildren()[1]->getValue() + "'.");
         }

         else if (symbols.entities.end() != symbols.entities.find(name)) {
            throw ValidationException(
               typeStr + " named '" + name + "' was already previously defined" +
                  (operation->getLineNumber() ? " (line " +
                  std::to_string(operation->getLineNumber()) + ")" : "")
            );
         }

         else if (symbols.entityClasses.end() == symbols.entityClasses.find(className)) {
            throw ValidationException(
               typeStr + " class '" + className + "' hasn't been defined" +
               (operation->getLineNumber() ?
                  " (line " + std::to_string(operation->getLineNumber()) + ")" : "")
            );
         }

         else {
            symbols.entities[name] = {
               name,
               className,
               type
            };
         }
      };

      /**********/

      // Validates and adds new Entity classes to the symbol table
      preOperations[DEFINE_ENTITY_CLASS] = [this](const std::shared_ptr<ASTOperationNode> &operation) {

         assertValidASTArguments(operation, 2);

         std::string className = operation->getChildren()[0]->getValue();
         std::string classTypeStr = operation->getChildren()[1]->getValue();
         entity::EntityType classType = entity::Entity::strToType(classTypeStr);

         if (symbols.entityClasses.end() != symbols.entityClasses.find(className)) {
            throw ValidationException(std::string("class '") + className +
               "' already defines a type of " +
               entity::Entity::typeToStr(symbols.entityClasses[className]));
         }

         else {
            symbols.entityClasses[className] = classType;
         }
      };

      /**********/

      // Validation
      preOperations[SET_MESSAGE] = [this](const std::shared_ptr<ASTOperationNode> &operation) {

         assertValidASTArguments(operation, 3, {
            {"entity", 4},
            {"class", 4},
            {"defaultPlayer", 3}
         });

         std::string targetType = operation->getChildren()[0]->getValue();

         if (
            0 == targetType.compare("entity") ||
            0 == targetType.compare("class")
         ) {
            assertTargetExists(
               targetType,
               operation->getChildren()[3]->getValue(),
               "set a message for",
               operation->getLineNumber()
            );
         }
      };

      /**********/

      // Validation
      preOperations[SET_TAG] = [this](const std::shared_ptr<ASTOperationNode> &operation) {

         assertValidASTArguments(operation, 2, {
            {"entity", 3},
            {"class", 3},
            {"defaultPlayer", 2}
         });

         std::string targetType = operation->getChildren()[0]->getValue();

         if (
            0 == targetType.compare("entity") ||
            0 == targetType.compare("class")
         ) {
            assertTargetExists(
               targetType,
               operation->getChildren()[2]->getValue(),
               "set a tag on",
               operation->getLineNumber()
            );
         }
      };

      /**********/

      // Validation
      preOperations[REMOVE_TAG] = [this](const std::shared_ptr<ASTOperationNode> &operation) {

         assertValidASTArguments(operation, 2, {
            {"entity", 3},
            {"class", 3},
            {"defaultPlayer", 2}
         });

         std::string targetType = operation->getChildren()[0]->getValue();

         if (
            0 == targetType.compare("entity") ||
            0 == targetType.compare("class")
         ) {
            assertTargetExists(
               targetType,
               operation->getChildren()[2]->getValue(),
               "remove a tag from",
               operation->getLineNumber()
            );
         }
      };

      /**********/

      // Validation
      preOperations[LOAD_SCRIPT] = [this](const std::shared_ptr<ASTOperationNode> &operation) {

         assertValidASTArguments(operation, 3, {
            {"entity", 4},
            {"class", 4},
            {"game", 3}
         });

         std::string targetType = operation->getChildren()[0]->getValue();
         std::string scriptMode = operation->getChildren()[1]->getValue();

         if (0 != scriptMode.compare("file") && 0 != scriptMode.compare("string")) {
            throw ValidationException("LOAD_SCRIPT: invalid script mode '" + scriptMode + "'. Should be either 'file' or 'string'. This is a bug.");
         }

         else if (0 == scriptMode.compare("file")) {

            std::string filename = operation->getChildren()[2]->getValue();
            std::ifstream scriptFile(filename.c_str());

            if (!scriptFile) {
               throw ValidationException(
                  "cannot open " + filename + (operation->getLineNumber() ?
                     " (line " + std::to_string(operation->getLineNumber()) + ")" : "")
               );
            }
         }

         if (
            0 == targetType.compare("entity") ||
            0 == targetType.compare("class")
         ) {
            assertTargetExists(
               targetType,
               operation->getChildren()[3]->getValue(),
               "parse script for",
               operation->getLineNumber()
            );
         }
      };

      /**********/

      // Validation
      preOperations[SET_EVENT] = [this](const std::shared_ptr<ASTOperationNode> &operation) {

         assertValidASTArguments(operation, 3, {
            {"entity", 4},
            {"class", 4},
            {"game", 3}
         });

         std::string targetType = operation->getChildren()[0]->getValue();

         if (
            0 == targetType.compare("entity") ||
            0 == targetType.compare("class")
         ) {
            assertTargetExists(
               targetType,
               operation->getChildren()[3]->getValue(),
               "set event for",
               operation->getLineNumber()
            );
         }
      };

      /**********/

      // Validation
      preOperations[SET_ALIAS] = [this](const std::shared_ptr<ASTOperationNode> &operation) {

         assertValidASTArguments(operation, 3);

         std::string targetType = operation->getChildren()[0]->getValue();

         if (0 != targetType.compare("entity") && 0 != targetType.compare("class")) {
            throw ValidationException(std::string("SET_ALIAS: invalid target type '") +
               targetType + "'. This is a bug.");
         }

         if (
            0 == targetType.compare("entity") ||
            0 == targetType.compare("class")
         ) {

            std::string entityOrClassName = operation->getChildren()[2]->getValue();

            assertTargetExists(
               targetType,
               entityOrClassName,
               "set alias for",
               operation->getLineNumber()
            );

            entity::EntityType entityOrClassType =
               0 == targetType.compare("entity") ?
                  symbols.entities[entityOrClassName].type :
                  symbols.entityClasses[entityOrClassName];

            // TODO: having a function that returns whether or not a certain
            // entity type is also another (in this case, ENTITY_THING) would
            // clean up my code here and possibly elsewhere
            if (
               entity::ENTITY_CREATURE != entityOrClassType &&
               entity::ENTITY_PLAYER != entityOrClassType &&
               entity::ENTITY_OBJECT != entityOrClassType
            ) {
               throw ValidationException(
                  std::string("aliases can only be set for creatures, players, or objects") +
                  (operation->getLineNumber() ?
                     " (line " + std::to_string(operation->getLineNumber()) + ")" : "")
               );
            }
         }
      };

      /**********/

      // Validation
      preOperations[SET_META] = [this](const std::shared_ptr<ASTOperationNode> &operation) {

         assertValidASTArguments(operation, 3, {
            {"entity", 4},
            {"class", 4},
            {"game", 3}
         });

         std::string targetType = operation->getChildren()[0]->getValue();

         if (
            0 == targetType.compare("entity") ||
            0 == targetType.compare("class")
         ) {
            assertTargetExists(
               targetType,
               operation->getChildren()[3]->getValue(),
               "set meta value for",
               operation->getLineNumber()
            );
         }
      };

      /**********/

      // Validation
      preOperations[SET_ATTRIBUTE] = [this](const std::shared_ptr<ASTOperationNode> &operation) {

         assertValidASTArguments(operation, 3, {
            {"entity", 4},
            {"class", 4},
            {"defaultPlayer", 3}
         });

         std::string targetType = operation->getChildren()[0]->getValue();
         std::string attribute = operation->getChildren()[1]->getValue();
         std::string value = operation->getChildren()[2]->getValue();

         if (!isValidInteger(value)) {
            throw ValidationException(std::string("attribute '") + attribute + "' is not a valid integer");
         }

         if (
            0 == targetType.compare("entity") ||
            0 == targetType.compare("class")
         ) {

            std::string entityOrClassName = operation->getChildren()[3]->getValue();

            assertTargetExists(
               targetType,
               entityOrClassName,
               "set attribute for",
               operation->getLineNumber()
            );

            entity::EntityType entityOrClassType =
               0 == targetType.compare("entity") ?
                  symbols.entities[entityOrClassName].type :
                  symbols.entityClasses[entityOrClassName];

            // TODO: having a function that returns whether or not a certain
            // entity type is also another (in this case, ENTITY_THING) would
            // clean up my code here and possibly elsewhere
            if (
               entity::ENTITY_CREATURE != entityOrClassType &&
               entity::ENTITY_PLAYER != entityOrClassType
            ) {
               throw ValidationException(
                  std::string("attributes can only be set for creatures or players") +
                  (operation->getLineNumber() ?
                     " (line " + std::to_string(operation->getLineNumber()) + ")" : "")
               );
            }
         }
      };

      /**********/

      // Validation
      preOperations[SET_PROPERTY] = [this](const std::shared_ptr<ASTOperationNode> &operation) {

         assertValidASTArguments(operation, 3, {
            {"entity", 4},
            {"class", 4},
            {"defaultPlayer", 3},
            {"game", 3}
         });

         std::string targetType = operation->getChildren()[0]->getValue();
         std::string property = operation->getChildren()[1]->getValue();
         std::string value = operation->getChildren()[2]->getValue();

         if (
            0 == targetType.compare("entity") ||
            0 == targetType.compare("class")
         ) {

            std::string entityOrClassName = operation->getChildren()[3]->getValue();

            assertTargetExists(
               targetType,
               entityOrClassName,
               "set property for",
               operation->getLineNumber()
            );

            std::string entityTypeStr = entity::Entity::typeToStr(
               0 == targetType.compare("entity") ?
                  symbols.entities[entityOrClassName].type :
                  symbols.entityClasses[entityOrClassName]
            );

            if (entityPropValidators.end() == entityPropValidators.find(entityTypeStr)) {
               throw UndefinedException(std::string(
                  "Setting property on unsupported entity type: ") +
                  (operation->getLineNumber() ?
                  " (line " + std::to_string(operation->getLineNumber()) + ")" : "")
               );
            }

            else if (
               entityPropValidators[entityTypeStr].end() ==
               entityPropValidators[entityTypeStr].find(property)
            ) {
               throw UndefinedException(std::string(
                  "Attempt to set undefined entity property ") +
                  (operation->getLineNumber() ?
                  " (line " + std::to_string(operation->getLineNumber()) + ")" : "")
               );
            }

            // TODO: entity type is a string for historical reasons, but I'd
            // rather be hashing the enum value instead, so go back and
            // refactor this later
            entityPropValidators[entityTypeStr][property](vocabulary, value);
         }

         else if (0 == targetType.compare("defaultPlayer")) {
            // TODO: entity type is a string for historical reasons, but I'd
            // rather be hashing the enum value instead, so go back and
            // refactor this later
            entityPropValidators["player"][property](vocabulary, value);
         }

         else {
            gamePropValidators[property](vocabulary, value);
         }
      };

      /**********/

      // Validation
      preOperations[INSERT_INTO_INVENTORY] = [this](const std::shared_ptr<ASTOperationNode> &operation) {

         assertValidASTArguments(operation, 2);

         std::string objectName = operation->getChildren()[0]->getValue();
         std::string beingName = operation->getChildren()[1]->getValue();

         if (
            symbols.entities.end() == symbols.entities.find(objectName) ||
            entity::ENTITY_OBJECT != symbols.entities[objectName].type
         ) {
            throw ValidationException(objectName +
               " doesn't exist or is not an object" +
               (operation->getLineNumber() ?
                  " (line " + std::to_string(operation->getLineNumber()) + ")" : ""));
         }

         else if (
            symbols.entities.end() == symbols.entities.find(beingName) || (
               entity::ENTITY_PLAYER != symbols.entities[beingName].type &&
               entity::ENTITY_CREATURE != symbols.entities[beingName].type
            )
         ) {
            throw ValidationException(beingName +
               " doesn't exist or is not a player or creature" +
               (operation->getLineNumber() ?
                  " (line " + std::to_string(operation->getLineNumber()) + ")" : ""));
         }
      };

      /**********/

      // Validation
      preOperations[INSERT_INTO_PLACE] = [this](const std::shared_ptr<ASTOperationNode> &operation) {

         assertValidASTArguments(operation, 2);

         std::string thingName = operation->getChildren()[0]->getValue();
         std::string roomName = operation->getChildren()[1]->getValue();

         if (
            symbols.entities.end() == symbols.entities.find(thingName) || (
               entity::ENTITY_OBJECT != symbols.entities[thingName].type &&
               entity::ENTITY_PLAYER != symbols.entities[thingName].type &&
               entity::ENTITY_CREATURE != symbols.entities[thingName].type
            )
         ) {
            throw ValidationException(thingName +
               " is a " +
               entity::Entity::typeToStr(symbols.entities[thingName].type) +
               ", but you can only insert creatures, players, or objects into a room" +
               (operation->getLineNumber() ?
                  " (line " + std::to_string(operation->getLineNumber()) + ")" : ""));
         }

         else if (
            symbols.entities.end() == symbols.entities.find(roomName) ||
            entity::ENTITY_ROOM != symbols.entities[roomName].type
         ) {
            throw ValidationException(roomName +
               " doesn't exist or is not a room" +
               (operation->getLineNumber() ?
                  " (line " + std::to_string(operation->getLineNumber()) + ")" : ""));
         }
      };

      /**********/

      // Validation
      preOperations[CONNECT_ROOMS] = [this](const std::shared_ptr<ASTOperationNode> &operation) {

         assertValidASTArguments(operation, 4, 5);

         std::string targetType = operation->getChildren()[0]->getValue();
         std::string sourceRoomOrClass = operation->getChildren()[1]->getValue();
         std::string connectToRoom = operation->getChildren()[2]->getValue();
         std::string direction = operation->getChildren()[3]->getValue();

         assertTargetExists(
            targetType,
            sourceRoomOrClass,
            "connect a room to",
            operation->getLineNumber()
         );

         if (
            symbols.entities.end() == symbols.entities.find(connectToRoom) ||
            entity::ENTITY_ROOM != symbols.entities[connectToRoom].type
         ) {
            throw ValidationException(connectToRoom +
               " doesn't exist or is not a room" +
               (operation->getLineNumber() ?
                  " (line " + std::to_string(operation->getLineNumber()) + ")" : ""));
         }

         else if (
            !vocabulary.isDirection(direction) &&
            customVocabulary.directions.end() == customVocabulary.directions.find(direction)
         ) {
            throw ValidationException(direction + " is not a valid direction" +
               (operation->getLineNumber() ?
                  " (line " + std::to_string(operation->getLineNumber()) + ")" : ""));
         }
      };

      /**********/

      // Validates new period for the timer
      preOperations[SET_TIMER_PERIOD] = [this](const std::shared_ptr<ASTOperationNode> &operation) {

         assertValidASTArguments(operation, 1);
         std::string value = operation->getChildren()[0]->getValue();

         if (!isValidInteger(value)) {
            throw ValidationException("timer period is not a valid integer");
         }
      };
   }

   /***************************************************************************/

   void Instantiator::mapEntityPropValidators() {

      // An Entity's title
      entityPropValidators["resource"]["title"] =
      entityPropValidators["room"]["title"] =
      entityPropValidators["object"]["title"] =
      entityPropValidators["creature"]["title"] =
      entityPropValidators["player"]["title"] = assertString;

      // An Entity's long description
      entityPropValidators["resource"]["longDesc"] =
      entityPropValidators["room"]["longDesc"] =
      entityPropValidators["object"]["longDesc"] =
      entityPropValidators["creature"]["longDesc"] =
      entityPropValidators["player"]["longDesc"] = assertString;

      // An Entity's short description
      entityPropValidators["resource"]["shortDesc"] =
      entityPropValidators["room"]["shortDesc"] =
      entityPropValidators["object"]["shortDesc"] =
      entityPropValidators["creature"]["shortDesc"] =
      entityPropValidators["player"]["shortDesc"] = assertString;

      // A Being's default starting health
      entityPropValidators["creature"]["health"] =
      entityPropValidators["player"]["health"] = assertInt;

      // A Being's maximum health
      entityPropValidators["creature"]["maxhealth"] =
      entityPropValidators["player"]["maxhealth"] = assertInt;

      // The probability that a Being will be wounded when it's attacked
      entityPropValidators["creature"]["woundrate"] =
      entityPropValidators["player"]["woundrate"] = assertProbability;

      // How many damage points a Being inflicts on a successful bare-handed
      // attack
      entityPropValidators["creature"]["damagebarehands"] =
      entityPropValidators["player"]["damagebarehands"] = assertInt;

      // Whether or not a Being can respawn
      entityPropValidators["creature"]["respawn.enabled"] =
      entityPropValidators["player"]["respawn.enabled"] = assertBool;

      // How long it takes before a Being respawns
      entityPropValidators["creature"]["respawn.interval"] =
      entityPropValidators["player"]["respawn.interval"] = assertInt;

      // How many lives a Being has (how many times it can respawn)
      entityPropValidators["creature"]["respawn.lives"] =
      entityPropValidators["player"]["respawn.lives"] = assertInt;

      // How much total weight a Being's inventory can contain
      entityPropValidators["creature"]["inventory.weight"] =
      entityPropValidators["player"]["inventory.weight"] = assertInt;

      // Whether or not a Creature will respond to an attack with one of its own
      entityPropValidators["creature"]["counterattack"] = assertBool;

      // This is a special property that, while technically being set, doesn't
      // actually require a value and instead sets counterattack to its
      // default value.
      entityPropValidators["creature"]["counterattack.default"] = assertString;

      // A creature's allegiance (friend, neutral, or enemy)
      entityPropValidators["creature"]["allegiance"] = [](const Vocabulary &vocabulary,
      std::string value) {

         if (
            0 == value.compare("friend") &&
            0 == value.compare("neutral") &&
            0 == value.compare("enemy")
         ) {
            throw ValidationException("allegiance should be one of 'friend', 'neutral', or 'enemy.'");
         }
      };

      // Whether or not autoattack is enabled for a Creature
      entityPropValidators["creature"]["autoattack.enabled"] = assertBool;

      // Whether or not a Creature's automatic attack should repeat
      entityPropValidators["creature"]["autoattack.repeat"] = assertBool;

      // If enabled and repeating, the interval at which a Creature's
      // autoattack is triggered
      entityPropValidators["creature"]["autoattack.interval"] = assertInt;

      // Whether or not a Creature will wander throughout the game
      entityPropValidators["creature"]["wandering.enabled"] = assertBool;

      // How many clock ticks pass before the Creature considers wanders again
      entityPropValidators["creature"]["wandering.interval"] = assertInt;

      // The probability that a Creature will actually wander each time it
      // considers moving
      entityPropValidators["creature"]["wandering.wanderlust"] = assertProbability;

      // An Object's weight
      entityPropValidators["object"]["weight"] = assertInt;

      // How much damage the Object inflicts if it's a weapon
      entityPropValidators["object"]["damage"] = assertInt;

      // A resource's total available amount
      entityPropValidators["resource"]["amountAvailable"] = assertDouble;

      // Whether or not a resource should only be allocated in integer amounts
      entityPropValidators["resource"]["requireIntegerAllocations"] = assertBool;

      // The maximum amount of a resource that a tangible entity can possess at
      // any one time
      entityPropValidators["resource"]["maxAmountPerDepositor"] = assertDouble;

      // A Resource's plural title
      entityPropValidators["resource"]["pluralTitle"] = assertString;
   }

   /***************************************************************************/

   void Instantiator::mapGamePropValidators() {

      // Whether or not a game introduction is enabled
      gamePropValidators["introduction.enabled"] = assertBool;

      // If a game introduction is enabled, all players will see this text once
      // before beginning the game
      gamePropValidators["introduction.text"] = assertString;
   }

   /***************************************************************************/

   void Instantiator::registerOperation(ASTOperation operation,
   std::function<void(const std::shared_ptr<ASTOperationNode> &operation)> opFunc) {

      operations[operation] = opFunc;
   }

   /***************************************************************************/

   void Instantiator::executeOperation(const std::shared_ptr<ASTOperationNode> &operation) {

      if (operations.end() == operations.find(operation->getOperation())) {
         throw UndefinedException(std::string("Undefined operation: ") +
            ASTOperationNode::getOperationStr(operation->getOperation()));
      }

      // If a preoperation function was defined, execute that first
      if (preOperations.end() != preOperations.find(operation->getOperation())) {
         preOperations[operation->getOperation()](operation);
      }

      operations[operation->getOperation()](operation);
   }

   /***************************************************************************/

   void Instantiator::instantiate(const std::shared_ptr<ASTNode> &ast) {

      // Make sure at least one room named "start" exists in the game
      bool atLeastOneRoomNamedStart = false;

      // For now, we assume that each child of the root ASTNode is an operation.
      // This might very well change as the AST becomes more complex.
      for (const auto &astChild: ast->getChildren()) {

         if (AST_OPERATION == astChild->getType()) {

            auto operation = std::dynamic_pointer_cast<ASTOperationNode>(astChild);

            if (DEFINE_ENTITY == operation->getOperation()) {

               std::string entityName = operation->getChildren()[0]->getValue();
               entity::EntityType entityType = entity::Entity::strToType(
                  operation->getChildren()[1]->getValue()
               );

               if (0 == entityName.compare("start") && entity::ENTITY_ROOM == entityType) {
                  atLeastOneRoomNamedStart = true;
               }
            }

            executeOperation(operation);
         }

         else {
            throw UndefinedException("Attempted to execute nonoperable AST node. This is a bug.");
         }
      }

      if (!atLeastOneRoomNamedStart) {
         throw ValidationException("There must be at least one room in the game named \"start\" so that the engine knows where to insert new players.");
      }

      // If implemented in child, this will be like a hook at the end of instantiate()
      afterInstantiate();
   }

   /***************************************************************************/

   // Does nothing unless implemented in a derived class
   void Instantiator::afterInstantiate() {}
}
