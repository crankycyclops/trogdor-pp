#ifndef RESOURCE_H
#define RESOURCE_H

#include <memory>
#include <optional>
#include <unordered_map>

#include <trogdor/entities/entity.h>
#include <trogdor/languages/english.h>

class Tangible;

namespace trogdor::entity {


   // Represents abstract resources that can be divided up in some amount by
   // other entities in a game.
   class Resource: public Entity {

      public:

         // When calling allocate(), one of these values is returned to indicate
         // success or the reason for failure.
         enum AllocateStatus {

            // Allocation was successful
            ALLOCATE_SUCCESS,

            // Only positive values greater than 0 are valid amounts
            ALLOCATE_ZERO_OR_NEGATIVE_AMOUNT,

            // Attempted to allocate an amount with a fractional component when
            // but only
            // integer values can be allocated
            ALLOCATE_INT_REQUIRED,

            // An amount was requested that's greater than the amount available
            // for allocation
            ALLOCATE_TOTAL_AMOUNT_EXCEEDED,

            // Allocation would have resulted in the holder receiving more of
            // the resource than they're allowed to have
            ALLOCATE_MAX_PER_DEPOSITOR_EXCEEDED
         };

         // When calling free(), one of these values is returned to indicate
         // success or the reason for failure.
         enum FreeStatus {

            // Deallocation was successful
            FREE_SUCCESS,

            // Only values >= 0 are valid (0 means we free everything)
            FREE_NEGATIVE_VALUE,

            // Attempted to free an amount with a fractional component when only
            // integer values are allowed
            FREE_INT_REQUIRED,

            // Attempted to free more than the entity's share of the resource
            FREE_EXCEEDS_ALLOCATION
         };

      private:

         // TODO: add support for other languages? Perhaps this can be
         // configurable via the game.xml file and then a global language
         // instance (maybe singleton) can be available for this to use instead
         // of forcing English.
         static English language;

         // If set to true, allocations must be made in whole number values.
         bool requireIntegerAllocations = false;

         // The total amount of resource that's available to be allocated. If
         // a value is set and allocations have been made, and then this is
         // updated to a lower value, if that lower value would drop below the
         // total amount of the resource currently allocated, an exception will
         // be thrown, indicating that the value has been set too low. This can
         // be resolved either by releasing some of the resource or by setting a
         // higher value. If a value isn't set, that indicates that there's an
         // infinite amount of the resource available.
         std::optional<double> amountAvailable;

         // The maximum amount of a resource that a depositor can hold at once.
         // Note that if a depositor allocates a certain amount, and then the
         // max later gets set to a lower value, this will not affect the
         // previous allocation. However, it will prevent the depositor from
         // allocating more, and if they ever let go of that resource and try to
         // allocate the same amount again, the new allocation will also fail.
         // Not setting a value indicates that they can allocate as much of
         // the resource as they wish, subject only to the maximum amount of the
         // resource available.
         std::optional<double> maxAmountPerDepositor;

         // Total amount of the resource that's currently allocated
         double totalAmountAllocated = 0;

         // Contains the plural version of the resource's name
         const std::string plural;

         // The plural analog to Entity::title. By default, this is equivalent
         // to the plural name.
         std::string pluralTitle;

         // Keeps track of who holds the resource and how much
         std::map<
            std::weak_ptr<Tangible>,
            double,
            std::owner_less<std::weak_ptr<Tangible>>
         > depositors;

      public:

         /*
            Constructor for creating a new Resource.

            Input:
               Reference to containing Game (Game *)
               Name (std::string)
         */
         Resource(
            Game *g,
            std::string n,
            std::optional<double> amountAvailable = std::nullopt,
            std::optional<double> maxAmountPerDepositor = std::nullopt,
            bool requireIntegerAllocations = false,
            std::optional<std::string> plural = std::nullopt
         );

         /*
            Constructor for cloning one Resource into another. IMPORTANT:
            cloning a resource will NOT copy over its allocations. It will be
            treated as an entirely separate resource.

            The plural name is a bit of a hack and is necessary only because of
            the way that Entities are spawned from Entity classes. Unless you're
            instantiating from a class, don't pass in a value for this.

            Input:
               Reference to entity to copy
               Name of copy (std::string)
               Plural name (std::optional<std::string>)
         */
         Resource(
            const Resource &e,
            std::string n,
            std::optional<std::string> plural = std::nullopt
         );

         /*
            Returns a smart pointer representing a raw Resource pointer. Be
            careful with this and only call it on Entities you know are managed
            by smart pointers. If, for example, you call this method on entities
            that are managed by Lua using new and delete, you're going to have a
            bad time.

            Input:
               (none)

            Output:
               std::shared_ptr<Entity>
         */
         inline std::shared_ptr<Resource> getShared() {

            return std::dynamic_pointer_cast<Resource>(Entity::getShared());
         }

         /*
            Sets the Resource's plural title.

            Input:
               New plural title (std::string)

            Output:
               (none)
         */
         inline void setPluralTitle(std::string pt) {

            mutex.lock();
            pluralTitle = pt;
            mutex.unlock();
         }

         /*
            Returns the Resource's plural name.

            Input:
               (none)

            Output:
               std::string
         */
         inline std::string getPluralName() const {return plural;}

         /*
            Returns the Resource's plural title.

            Input:
               (none)

            Output:
               std::string
         */
         inline std::string getPluralTitle() const {return pluralTitle;}

         /*
            Displays a Resource.

            Input:
               Being doing the observing
               Whether or not to always display the full description

            Output:
               (none)
         */
         virtual void display(Being *observer, bool displayFull = false);

         /*
            Returns true if integer allocations are required and false if not.

            Input:
               (none)

            Output:
               Whether or not integer allocations are required (bool)
         */
         inline bool areIntegerAllocationsRequired() const {return requireIntegerAllocations;}

         /*
            Set whether or not allocations must be made in integer-only amounts.

            Input:
               Whether or not integer-only allocations are required (bool)

            Output:
               (none)
         */
         inline void setRequireIntegerAllocations(bool required) {

            mutex.lock();
            requireIntegerAllocations = required;
            mutex.unlock();
         }

         /*
            Returns the total amount of the resource available (this has no
            bearing on how much of the resources has been allocated, but rather
            tells the caller how much of the resource currently exists.) A
            value of std::nullopt indicates that the resource has an infinite
            supply.

            Input:
               (none)

            Output:
               How much of the resource exists (std::optional<double>)
         */
         inline std::optional<double> getAmountAvailable() const {return amountAvailable;}

         /*
            Set the total amount of the resource available. Setting this to
            std::nullopt will result in the resource having an infinite supply.
            If caller attempts to set a value that's below the amount currently
            allocated, false will be returned, indicating failure.

            Input:
               The total amount of the resource available (std::optional<double>)

            Output:
               True if the call was successful and false if not (bool)
         */
         inline bool setAmountAvailable(std::optional<double> newAmount = std::nullopt) {

            if (newAmount && *newAmount < totalAmountAllocated) {
               return false;
            }

            mutex.lock();
            amountAvailable = newAmount;
            mutex.unlock();

            return true;
         }

         /*
            Returns the total amount of the resource that can be allocated by a
            tangible entity at once. A value of std::nullopt indicates that
            entities can hold as much as they want.

            Input:
               (none)

            Output:
               How much of the resource can be allocated to each entity at one
                  time (std::optional<double>)
         */
         inline std::optional<double> getMaxAmountPerDepositor() const {return maxAmountPerDepositor;}

         /*
            Set the total amount of the resource a tangible entity can possess
            at one given time. Setting this to std::nullopt will result in no
            limit. If you set this to a lower value than any current allocations
            that have already been made, the call will succeed and the tangible
            entity will retain the allocation they already have, but will not
            be able to allocate more and, if they ever release their amount and
            try to allocate more, the new maximum will be enforced.

            Input:
               How much a tangible entity can possess at one time (std::optional<double>)

            Output:
               (none)
         */
         inline void setMaxAmountPerDepositor(std::optional<double> newMax = std::nullopt) {

            mutex.lock();
            maxAmountPerDepositor = newMax;
            mutex.unlock();
         }

         /*
            Returns the total amount of the resource currently allocated to
            various entities.

            Input:
               (none)

            Output:
               How much of the resource is currently allocated (double)
         */
         inline double getTotalAmountAllocated() const {return totalAmountAllocated;}

         /*
            Returns a const reference to all tangible entities that hold some
            amount of the resource.

            Input:
               (none)

            Output:
               An unordered_map of entity weak_ptrs -> allocated amount
         */
         inline const auto &getDepositors() const {return depositors;}

         /*
            Allocates the specified amount to the specified tangible entity.
            Returns an enum indicating success or the nature of the failure.

            Input:
               Reference to tangible entity (const std::shared_ptr<Tangible> &)
               Amount of resource to allocate (double)
         */
         AllocateStatus allocate(const std::shared_ptr<Tangible> &entity, double amount);

         /*
            Deallocates the specified amount of the resource from the entity.
            Returns an enum indicating success or the nature of the failure.

            Input:
               Reference to tangible entity (const std::shared_ptr<Tangible> &)
               Amount of resource to deallocate (double -- 0 means to deallocate everything)
         */
         FreeStatus free(const std::shared_ptr<Tangible> &entity, double amount = 0);
   };
}


#endif
