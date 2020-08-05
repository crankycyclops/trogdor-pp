#include <cmath>

#include <trogdor/entities/resource.h>
#include <trogdor/entities/tangible.h>
#include <trogdor/entities/being.h>

#include <trogdor/iostream/nullout.h>
#include <trogdor/iostream/nullerr.h>

namespace trogdor::entity {


   English Resource::language = English();

   /***************************************************************************/

   Resource::Resource(
         Game *g,
         std::string n,
         std::optional<double> amountAvailable,
         std::optional<double> maxAmountPerDepositor,
         bool requireIntegerAllocations,
         std::optional<std::string> pluralName
   ): Entity(g, n, std::make_unique<NullOut>(), std::make_unique<NullErr>()),
   requireIntegerAllocations(requireIntegerAllocations),
   amountAvailable(amountAvailable), maxAmountPerDepositor(maxAmountPerDepositor),
   plural(pluralName ? *pluralName : language.pluralizeNoun(n)),
   pluralTitle(plural) {

      types.push_back(ENTITY_RESOURCE);
      setClass("resource");
   }

   /***************************************************************************/

   Resource::Resource(
      const Resource &r,
      std::string n,
      std::optional<std::string> plural
   ): Entity(r, n), requireIntegerAllocations(r.requireIntegerAllocations),
   amountAvailable(r.amountAvailable), maxAmountPerDepositor(r.maxAmountPerDepositor),
   totalAmountAllocated(0), plural(plural ? *plural : r.plural),
   pluralTitle(r.pluralTitle) {}

   /***************************************************************************/

   void Resource::display(Being *observer, bool displayFull) {

      if (ENTITY_PLAYER == observer->getType()) {

         if (getLongDescription().length() > 0) {
            observer->out("display") << getLongDescription() << std::endl;
         }

         else if (getShortDescription().length() > 0) {
            observer->out("display") << getShortDescription() << std::endl;
         }

         else {
            observer->out("display") << "You see " << getTitle() << '.' << std::endl;
         }
      }
   }

   /***************************************************************************/

   Resource::AllocateStatus Resource::allocate(
      const std::shared_ptr<Tangible> &entity,
      double amount
   ) {

      if (amount <= 0) {
         return ALLOCATE_ZERO_OR_NEGATIVE_AMOUNT;
      }

      double updatedBalance = amount;
      auto shared = getShared();

      // If the entity already possesses some amount of the resource, make sure
      // to take it into account when updating its balance after the allocation
      if (depositors.end() != depositors.find(entity)) {
         updatedBalance += depositors[entity];
      }

      if (requireIntegerAllocations) {

         double intPart, fracPart = modf(amount, &intPart);

         if (fracPart) {
            return ALLOCATE_INT_REQUIRED;
         }
      }

      if (amountAvailable && amount + totalAmountAllocated > *amountAvailable) {
         return ALLOCATE_TOTAL_AMOUNT_EXCEEDED;
      }

      if (maxAmountPerDepositor && updatedBalance > *maxAmountPerDepositor) {
         return ALLOCATE_MAX_PER_DEPOSITOR_EXCEEDED;
      }

      mutex.lock();
      entity->mutex.lock();

      depositors[entity] = updatedBalance;
      entity->recordResourceAllocation(shared, updatedBalance);
      totalAmountAllocated += amount;

      entity->mutex.unlock();
      mutex.unlock();

      return ALLOCATE_SUCCESS;
   }

   /***************************************************************************/

   Resource::FreeStatus Resource::free(
      const std::shared_ptr<Tangible> &entity,
      double amount
   ) {

      if (amount < 0) {
         return FREE_NEGATIVE_VALUE;
      }

      if (requireIntegerAllocations) {

         double intPart, fracPart = modf(amount, &intPart);

         if (fracPart) {
            return FREE_INT_REQUIRED;
         }
      }

      auto shared = getShared();

      if (
         depositors.end() == depositors.find(entity) ||
         depositors[entity] < amount
      ) {
         return FREE_EXCEEDS_ALLOCATION;
      }

      mutex.lock();
      entity->mutex.lock();

      // Free everything
      if (0 == amount) {
         amount = depositors[entity];
      }

      depositors[entity] -= amount;
      entity->resources[shared] -= amount;
      totalAmountAllocated -= amount;

      if (0 == depositors[entity]) {
         depositors.erase(entity);
         entity->removeResourceAllocation(shared);
      }

      entity->mutex.unlock();
      mutex.unlock();

      return FREE_SUCCESS;
   }
}
