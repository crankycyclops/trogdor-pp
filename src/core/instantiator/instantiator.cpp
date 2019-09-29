#include <string>

#include "../include/instantiator/instantiator.h"
#include "../include/utility.h"

#include "../include/exception/validationexception.h"
#include "../include/exception/undefinedexception.h"

using namespace std;

namespace trogdor {


   Instantiator::Instantiator() {

      mapEntityPropValidators();
      mapGamePropValidators();
   }

   /***************************************************************************/

   void Instantiator::assertBool(string value) {

      // TODO: also allow strings "false" and "true"
      if (value != "1" && value != "0") {
         throw ValidationException("value must be 1 for true or 0 for false");
      }
   }

   /***************************************************************************/

   void Instantiator::assertInt(string value) {

      if (!isValidInteger(value)) {
         throw ValidationException("value is not a valid integer");
      }
   }

   /***************************************************************************/

   void Instantiator::assertDouble(string value) {

      if (!isValidDouble(value)) {
         throw ValidationException("value is not a valid number");
      }
   }

   /***************************************************************************/

   void Instantiator::assertProbability(string value) {

      string errorMsg = "value is not a valid probability (must be between 0 and 1)";

      if (!isValidDouble(value)) {
         throw ValidationException(errorMsg);
      }

      double dValue = stod(value);

      if (dValue < 0 || dValue > 1) {
         throw ValidationException(errorMsg);
      }
   }

   /***************************************************************************/

   void Instantiator::assertString(string value) {

      // Any string is valid, so never throw an exception
      return;
   }

   /***************************************************************************/

   void Instantiator::mapEntityPropValidators() {

      // An Entity's title
      entityPropValidators["room"]["title"] =
      entityPropValidators["object"]["title"] =
      entityPropValidators["creature"]["title"] =
      entityPropValidators["player"]["title"] = assertString;

      // An Entity's long description
      entityPropValidators["room"]["longDesc"] =
      entityPropValidators["object"]["longDesc"] =
      entityPropValidators["creature"]["longDesc"] =
      entityPropValidators["player"]["longDesc"] = assertString;

      // An Entity's short description
      entityPropValidators["room"]["shortDesc"] =
      entityPropValidators["object"]["shortDesc"] =
      entityPropValidators["creature"]["shortDesc"] =
      entityPropValidators["player"]["shortDesc"] = assertString;

      // Special "property" that sets a meta value for an Entity
      entityPropValidators["room"]["meta"] =
      entityPropValidators["object"]["meta"] =
      entityPropValidators["creature"]["meta"] =
      entityPropValidators["player"]["meta"] = [](string value) {

         string metaKey = value.substr(0, value.find(":"));
         string metaValue = value.substr(value.find(":") + 1, value.length());

         if (!metaKey.length() || !metaValue.length()) {
            throw UndefinedException("not a valid meta -> value pair (this is a bug)");
         }
      };

      // Special "property" that sets a tag for an Entity
      entityPropValidators["room"]["tag.set"] =
      entityPropValidators["object"]["tag.set"] =
      entityPropValidators["creature"]["tag.set"] =
      entityPropValidators["player"]["tag.set"] = assertString;

      // Special "property" that removes a tag for an Entity
      entityPropValidators["room"]["tag.remove"] =
      entityPropValidators["object"]["tag.remove"] =
      entityPropValidators["creature"]["tag.remove"] =
      entityPropValidators["player"]["tag.remove"] = assertString;

      // Special "property" that sets an alias for a Thing
      // TODO: validate this the exact same way I validate an Entity's name
      entityPropValidators["object"]["alias"] =
      entityPropValidators["creature"]["alias"] =
      entityPropValidators["player"]["alias"] = assertString;

      // Whether or not a Being starts out alive
      entityPropValidators["creature"]["alive"] =
      entityPropValidators["player"]["alive"] = assertBool;

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

      // Special "property" that inserts an Object into a Being's inventory
      entityPropValidators["creature"]["inventory.object"] =
      entityPropValidators["player"]["inventory.object"] = [](string value) {

         // TODO: throw error if it's not a valid name (maybe this should be
         // built-in.)
         return;
      };

      // Special "property" that sets a value for a Being's particular attribute
      entityPropValidators["creature"]["attribute"] =
      entityPropValidators["player"]["attribute"] = [](string value) {

         string attr = value.substr(0, value.find(":"));
         string attrValue = value.substr(value.find(":") + 1, value.length());

         if (!attr.length() || !attrValue.length()) {
            throw UndefinedException("Call to set creature or player attribute is invalid. This is a bug.");
         }

         else if (!isValidInteger(attrValue)) {
            throw ValidationException(string("attribute '") + attr + "' is not a valid integer");
         }
      };

      // Whether or not a Creature will respond to an attack with one of its own
      entityPropValidators["creature"]["counterattack"] = assertBool;

      // This is a special property that, while technically being set, doesn't
      // actually require a value and instead sets counterattack to its
      // default value.
      entityPropValidators["creature"]["counterattack.default"] = assertString;

      // A creature's allegiance (friend, neutral, or enemy)
      entityPropValidators["creature"]["allegiance"] = [](string value) {

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

      // Special "property" that sets a connection between two rooms.
      entityPropValidators["room"]["connection"] = [](string value) {

         string direction = value.substr(0, value.find(":"));
         string connectToName = value.substr(value.find(":") + 1, value.length());

         // TODO: also validate that connectToName is a valid Entity name
         if (!direction.length() || !connectToName.length()) {
            throw UndefinedException("Invalid room connection. This is a bug.");
         }
      };

      // Special "property" that inserts a Thing into a Room.
      entityPropValidators["room"]["contains"] = [](string value) {

         // TODO: validate that value is a valid Entity name
         return;
      };

      // An Object's weight
      entityPropValidators["object"]["weight"] = assertInt;

      // How much damage the Object inflicts if it's a weapon
      entityPropValidators["object"]["damage"] = assertInt;
   }

   /***************************************************************************/

   void Instantiator::mapGamePropValidators() {

      // Whether or not a game introduction is enabled
      gamePropValidators["introduction.enabled"] = assertBool;

      // If a game introduction is enabled, all players will see this text once
      // before beginning the game
      gamePropValidators["introduction.text"] = assertString;

      // Whether or not the game should pause after the introduction before
      // continuing (if the introduction is enabled)
      gamePropValidators["introduction.pause"] = assertBool;

      // Special "property" that sets a meta data value for the game
      gamePropValidators["meta"] = [](string value) {

         string metaKey = value.substr(0, value.find(":"));
         string metaValue = value.substr(value.find(":") + 1, value.length());

         if (!metaKey.length() || !metaValue.length()) {
            throw UndefinedException("not a valid meta -> value pair (this is a bug)");
         }
      };

      // Special "property" that sets an action synonym for the game (for
      // example, "shutdown" might be a synonym for "quit")
      gamePropValidators["synonym"] = [](string value) {

         string synonym = value.substr(0, value.find(":"));
         string action = value.substr(value.find(":") + 1, value.length());

         if (!synonym.length() || !action.length()) {
            throw UndefinedException("not a valid synonym -> action pair (this is a bug)");
         }
      };
   }
}

