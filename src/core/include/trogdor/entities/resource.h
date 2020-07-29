#ifndef RESOURCE_H
#define RESOURCE_H

#include <memory>
#include <optional>
#include <unordered_map>

#include <trogdor/entities/entity.h>

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
         // resource available,
         std::optional<double> maxAmountPerDepositor;

         // Keeps track of who holds the resource and how much
         std::map<
            std::weak_ptr<Tangible>,
            double,
            std::owner_less<std::weak_ptr<Tangible>>
         > depositors;

         // Total amount of the resource that's currently allocated
         double totalAmountAllocated = 0;

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
            bool requireIntegerAllocations = false
         );

         /*
            Constructor for cloning one Resource into another. IMPORTANT:
            cloning a resource will NOT copy over its allocations. It will be
            treated as an entirely separate resource.

            Input:
               Reference to entity to copy
               Name of copy (std::string)
         */
         Resource(const Resource &e, std::string n);

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
