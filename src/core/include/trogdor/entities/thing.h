#ifndef THING_H
#define THING_H


#include <vector>
#include <memory>

#include <trogdor/entities/tangible.h>
#include <trogdor/entities/place.h>


namespace trogdor::entity {


   class Thing: public Tangible {

      protected:

         std::weak_ptr<Place> location;       // where the Thing is located
         std::vector<std::string>  aliases;   // list of aliases

         friend void Place::insertPlayer(const std::shared_ptr<Player> &);
         friend void Place::removePlayer(const std::shared_ptr<Player> &);
         friend void Place::insertObject(const std::shared_ptr<Object> &);
         friend void Place::removeObject(const std::shared_ptr<Object> &);
         friend void Place::insertCreature(const std::shared_ptr<Creature> &);
         friend void Place::removeCreature(const std::shared_ptr<Creature> &);

         /*
            Overrides Tangible::display() and shows a Thing's description in the
            proper format.

            Input:
               Being observing the Thing
               Whether or not to always display the long description

            Output:
               (none)
         */
         virtual void display(Being *observer, bool displayFull = false);

         /*
            Overrides Tangible::displayShort() and shows a Thing's short
            description in the proper format.

            Input:
               Being observing the Thing

            Output:
               (none)
         */
         virtual void displayShort(Being *observer);

         /*
            Set's the Thing's current location in the game.

            Input:
               Pointer to Place

            Output:
               (none)
         */
         inline void setLocation(std::weak_ptr<Place> l) {

            mutex.lock();
            location = l;
            mutex.unlock();
         }

      public:

         /*
            Constructor for creating a new Thing.  Requires reference to the
            containing Game object and a name.
         */
         Thing(
            Game *g,
            std::string n,
            std::unique_ptr<Trogout> o,
            std::unique_ptr<Trogerr> e
         );

         /*
            Constructor for cloning a Thing.  Requires a unique name.
         */
         Thing(const Thing &t, std::string n);

         /*
            This constructor deserializes a Thing.

            Input:
               Game the Thing belongs to (Game *)
               Raw deserialized data (const Serializable &)
               Pointer to output stream object (std::unique_ptr<Trogout>)
               Pointer to error stream object (std::unique_ptr<Trogerr>)
         */
         Thing(
            Game *g,
            const serial::Serializable &data,
            std::unique_ptr<Trogout> o,
            std::unique_ptr<Trogerr> e
         );

         /*
            Returns a smart pointer representing a raw Thing pointer. Be careful
            with this and only call it on Entities you know are managed by smart
            pointers. If, for example, you call this method on entities that are
            managed by Lua using new and delete, you're going to have a bad time.

            Input:
               (none)

            Output:
               std::shared_ptr<Thing>
         */
         inline std::shared_ptr<Thing> getShared() {

            return std::dynamic_pointer_cast<Thing>(Entity::getShared());
         }

         /*
            Returns the Thing's current location in the game.  If the Thing
            hasn't been placed anywhere, 0 will be returned.

            Input:
               (none)

            Output:
               std::weak_ptr<Place>
         */
         inline std::weak_ptr<Place> getLocation() const {return location;}

         /*
            Returns list of Thing's aliases.

            Input:
               (none)

            Output:
               std::vector<std::string>
         */
         inline std::vector<std::string> const getAliases() const {return aliases;}

         /*
            Serializes the Thing.

            Input:
               (none)

            Output:
               An object containing easily serializable data (std::shared_ptr<Serializable>)
         */
         virtual std::shared_ptr<serial::Serializable> serialize();

         /*
            Adds an alias to the Thing, which is another name that the Thing can
            be called.

            Input:
               New alias (string)

            Output:
               (none)
         */
         virtual void addAlias(std::string alias);

         // TODO: virtual void removeAlias(string alias);
   };
}


#endif
