#ifndef RESOURCE_H
#define RESOURCE_H

#include <cmath>
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

         // Returned by the property validator when setting AmtAvailProperty to
         // a value that's smaller than the total amount of the resource that's
         // currently allocated
         static constexpr int AMOUNT_AVAILABLE_TOO_SMALL = 2;

         // Boolean property that determines whether or not allocations are
         // required to be made in integer amounts
         static constexpr const char *ReqIntAllocProperty = "reqIntAlloc";

         // The total amount of resource that's available to be allocated. If
         // a value is set and allocations have been made, and then this is
         // updated to a lower value, if that lower value would drop below the
         // total amount of the resource currently allocated, an exception will
         // be thrown, indicating that the value has been set too low. This can
         // be resolved either by releasing some of the resource or by setting a
         // higher value. If the value is unset, that indicates that there's an
         // infinite amount of the resource available.
         static constexpr const char *AmtAvailProperty = "amountAvailable";

         // The maximum amount of a resource that a depositor can hold at once.
         // Note that if a depositor allocates a certain amount, and then the
         // max later gets set to a lower value, this will not affect the
         // previous allocation. However, it will prevent the depositor from
         // allocating more, and if they ever let go of that resource and try to
         // allocate the same amount again, the new allocation will also fail.
         // Not setting a value indicates that they can allocate as much of
         // the resource as they wish, subject only to the maximum amount of the
         // resource available.
         static constexpr const char *MaxAmtPerDepositorProperty = "maxAmtPerDepositor";

         // Contains the plural version of the resource's name
         static constexpr const char *PluralNameProperty = "pluralName";

         // The plural analog to Entity::title. By default, this is equivalent
         // to the plural name.
         static constexpr const char *PluralTitleProperty = "pluralTitle";

         // If the sticky tag is set and the resource can be allocated in
         // unlimited amounts, then when a Being picks some up from a Place, the
         // Place will keep its current allocation.
         static constexpr const char *StickyTag = "sticky";

         // Tag is set if the Resource should disappear when it's dropped by a
         // Being instead of appearing in the Room the Being currently resides in.
         static constexpr const char *EphemeralTag = "ephemeral";

         // When calling allocate() or free(), one of these values is returned
         // to indicate success or the reason for failure.
         enum AllocationStatus {

            // Allocation/deallocation was successful
            ALLOCATE_OR_FREE_SUCCESS,

            // The beforeAllocateResource or beforeFreeResource event canceled
            // the allocation or freeing of the resource
            ALLOCATE_OR_FREE_ABORT,

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
            ALLOCATE_MAX_PER_DEPOSITOR_EXCEEDED,

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
         static const English language;

         // By replacing these parameters with their singular or plural version,
         // we can use a single description to fit both cases.
         static const std::unordered_map<
            bool,
            std::unordered_map<std::string, std::string>
         > constTemplateParameters;

         // Lock on this when we're transferring a resource allocation from one
         // tangible entity to another.
         std::mutex transferMutex;

         // Keeps track of who holds the resource and how much
         std::map<
            std::weak_ptr<Tangible>,
            double,
            std::owner_less<std::weak_ptr<Tangible>>
         > depositors;

         // Total amount of the resource that's currently allocated
         double totalAmountAllocated = 0;

         // Modify's a tangible entity's allocation without any checks. This is
         // utilized by the public methods allocate(), free(), and transfer().
         void allocateRaw(const std::shared_ptr<Tangible> &entity, double amount);

         /*
            Replaces parameters in the provided template with their appropriate
            values.

            Input:
               Template (std::string)
               Whether or not we're outputing the plural versions (bool)

            Output:
               A fully hydrated string
         */
         std::string hydrateString(std::string templateStr, bool isPlural);

         /*
            Displays a Resource. I would like to combine display() and
            displayPlural() into a single method that looks something like
            display(Being *observer, double amount), but since display() is
            pure virtual in Entity and needs to stay that way, I need to
            separate this functionality. I'll revisit this later, as I suspect
            I'm doing something stupid.

            Input:
               Being we're outputing a description to (Being *)
               Whether or not the description should be plural (bool)

            Output:
               (none)
         */
         void display(Being *observer, bool plural);

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
         inline void setPluralTitle(std::string pluralTitle) {

            setProperty(PluralTitleProperty, pluralTitle);
         }

         /*
            Returns the Resource's plural name.

            Input:
               (none)

            Output:
               std::string
         */
         inline std::string getPluralName() const {return std::get<std::string>(*getProperty(PluralNameProperty));}

         /*
            Returns the Resource's plural title.

            Input:
               (none)

            Output:
               std::string
         */
         inline std::string getPluralTitle() const {return std::get<std::string>(*getProperty(PluralTitleProperty));}

         /*
            Returns true if the given name is a plural representation of the
            Resource and false if not. The existence of this method will prove
            its worth once we start having to deal with plural aliases.

            Input:
               std::string

            Output:
               bool
         */
         inline bool isPlural(std::string name) const {

            return 0 == std::get<std::string>(*getProperty(PluralNameProperty)).compare(name) ? true : false;
         }

         /*
            This is similar to Tangible::observe() except that there's really
            no specific instance of a thing to observe.

            Input:
               Being doing the observing
               The amount of the resource being observed
               Whether or not to fire before and after events

            Output:
               (none)

            Events Triggered:
               beforeObserve
               afterObserve
         */
         void observe(
            const std::shared_ptr<Being> &observer,
            double amount,
            bool triggerEvents = true
         );

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
         inline std::optional<double> getMaxAmountPerDepositor() const {

            auto maxAmt = getProperty(MaxAmtPerDepositorProperty);
            return maxAmt ? std::optional<double>(std::get<double>(*maxAmt)) : std::nullopt;
         }

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

            if (newMax) {
               setProperty(MaxAmtPerDepositorProperty, *newMax);
            } else {
               unsetProperty(MaxAmtPerDepositorProperty);
            }
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
            A helper function that takes as input some amount and returns,
            depending on the value of requireIntegerAllocations, an integer
            or decimal value in the form of a string.

            Input:
               An arbitrary value (double)

            Output:
               The appropriate string representation (std::string)
         */
         inline std::string amountToString(double amount) const {

            return std::get<bool>(*getProperty(ReqIntAllocProperty)) ?
               std::to_string(std::lround(amount)) : std::to_string(amount);
         }

         /*
            A helper function that takes as input some amount and returns,
            depending on the value passed in and whether or not integer
            allocations are required, a plural or singular title.

            Input:
               An arbitrary value (double)

            Output:
               The appropriate string representation of the title (std::string)
         */
         inline std::string titleToString(double amount) const {

            return std::get<bool>(*getProperty(ReqIntAllocProperty)) && 1 == std::lround(amount) ?
               std::get<std::string>(*getProperty(TitleProperty)) : getPluralTitle();
         }

         /*
            Allocates the specified amount to the specified tangible entity.
            Returns an enum indicating success or the nature of the failure.

            Input:
               Reference to tangible entity (const std::shared_ptr<Tangible> &)
               Amount of resource to allocate (double)
               Whether or not to fire event triggers (bool: default = true)
         */
         AllocationStatus allocate(
            const std::shared_ptr<Tangible> &entity,
            double amount,
            bool triggerEvents = true
         );

         /*
            Deallocates the specified amount of the resource from the entity.
            Returns an enum indicating success or the nature of the failure.

            Input:
               Reference to tangible entity (const std::shared_ptr<Tangible> &)
               Amount of resource to deallocate (double -- 0 means to deallocate everything)
               Whether or not to fire event triggers (bool: default = true)
         */
         AllocationStatus free(
            const std::shared_ptr<Tangible> &entity,
            double amount = 0,
            bool triggerEvents = true
         );

         /*
            Transfers some amount of a resource from one entity to another.

            Input:
               Reference to tangible entity who possesses the resource
                  (const std::shared_ptr<Tangible> &)
               Reference to tangible entity who's to receive the resource
                  (const std::shared_ptr<Tangible> &)
               The amount of the resource to transfer (double)
               Whether or not to fire event triggers (bool: default = true)
         */
         AllocationStatus transfer(
            const std::shared_ptr<Tangible> &depositor,
            const std::shared_ptr<Tangible> &beneficiary,
            double amount,
            bool triggerEvents = true
         );
   };
}


#endif
