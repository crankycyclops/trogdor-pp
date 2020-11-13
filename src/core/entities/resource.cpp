#include <trogdor/utility.h>

#include <trogdor/entities/resource.h>
#include <trogdor/entities/tangible.h>
#include <trogdor/entities/being.h>

#include <trogdor/iostream/nullout.h>
#include <trogdor/iostream/nullerr.h>

namespace trogdor::entity {


   const English Resource::language = English();

   const std::unordered_map<
      bool,
      std::unordered_map<std::string, std::string>
   > Resource::constTemplateParameters = {

      {false, {
         {"{%A}", "A"},
         {"{%a}", "a"},
         {"{%An}", "An"},
         {"{%an}", "an"},
         {"{%It}", "It"},
         {"{%it}", "it"},
         {"{%Its}", "It's"},
         {"{%its}", "it's"}
      }},

      {true, {
         {"{%A}", "Some"},
         {"{%a}", "some"},
         {"{%An}", "Some"},
         {"{%an}", "some"},
         {"{%It}", "They"},
         {"{%it}", "they"},
         {"{%Its}", "They're"},
         {"{%its}", "they're"}
      }}
   };

   /***************************************************************************/

   Resource::Resource(
         Game *g,
         std::string n,
         std::optional<double> amountAvailable,
         std::optional<double> maxAmountPerDepositor,
         bool requireIntegerAllocations,
         std::optional<std::string> pluralName
   ): Entity(g, n, std::make_unique<NullOut>(), std::make_unique<NullErr>()) {

      if (amountAvailable) {
         setProperty(AmtAvailProperty, *amountAvailable);
      }

      if (maxAmountPerDepositor) {
         setProperty(MaxAmtPerDepositorProperty, *maxAmountPerDepositor);
      }

      setProperty(ReqIntAllocProperty, requireIntegerAllocations);
      setProperty(PluralNameProperty, pluralName ? *pluralName : language.pluralizeNoun(n));
      setProperty(PluralTitleProperty, std::get<std::string>(*getProperty(PluralNameProperty)));

      types.push_back(ENTITY_RESOURCE);
      setClass("resource");
   }

   /***************************************************************************/

   Resource::Resource(
      const Resource &r,
      std::string n,
      std::optional<std::string> plural
   ): Entity(r, n), totalAmountAllocated(0) {}

   /***************************************************************************/

   std::string Resource::hydrateString(std::string templateStr, bool isPlural) {

      std::string result = templateStr;

      std::unordered_map<
         bool,
         std::unordered_map<std::string, std::string>
      > templateParameters = {

         {false, {
            {"{%title}", std::get<std::string>(*getProperty(TitleProperty))},
            {"{%name}", getName()},
            {"{%Title}", capitalize(std::get<std::string>(*getProperty(TitleProperty)))},
            {"{%Name}", capitalize(getName())}
         }},

         {true, {
            {"{%title}", getPluralTitle()},
            {"{%name}", getPluralName()},
            {"{%Title}", capitalize(getPluralTitle())},
            {"{%Name}", capitalize(getPluralName())}
         }}
      };

      for (const auto &parameter: constTemplateParameters.find(isPlural)->second) {
         result = replaceAll(result, parameter.first, parameter.second);
      }

      for (const auto &parameter: templateParameters[isPlural]) {
         result = replaceAll(result, parameter.first, parameter.second);
      }

      return result;
   }

   /***************************************************************************/

   void Resource::display(Being *observer, bool isPlural) {

      if (ENTITY_PLAYER == observer->getType()) {

         auto longDesc = getProperty(LongDescProperty);
         auto shortDesc = getProperty(ShortDescProperty);

         if (longDesc && std::get<std::string>(*longDesc).length() > 0) {
            observer->out("display") << hydrateString(std::get<std::string>(*longDesc), isPlural)
               << std::endl;
         }

         else if (shortDesc && std::get<std::string>(*shortDesc).length() > 0) {
            observer->out("display") << hydrateString(std::get<std::string>(*shortDesc), isPlural)
               << std::endl;
         }

         else {
            observer->out("display") << "You see " <<
               (isPlural ? getPluralTitle() : std::get<std::string>(*getProperty(TitleProperty))) <<
               '.' << std::endl;
         }
      }
   }

   /***************************************************************************/

   void Resource::observe(
      const std::shared_ptr<Being> &observer,
      double amount,
      bool triggerEvents
   ) {

      if (triggerEvents) {
         if (!game->event({
            "beforeObserve",
            {triggers.get(), observer->getEventListener()},
            {this, observer.get(), amount}
         })) {
            return;
         }
      }

      display(
         observer.get(),
         areIntegerAllocationsRequired() && 1 == amount ? false : true
      );

      if (triggerEvents) {
         game->event({
            "afterObserve",
            {triggers.get(), observer->getEventListener()},
            {this, observer.get(), amount}
         });
      }
   }

   /***************************************************************************/

   void Resource::allocateRaw(
      const std::shared_ptr<Tangible> &entity,
      double amount
   ) {

      auto sharedPtr = getShared();
      double updatedBalance = amount;

      mutex.lock();
      entity->mutex.lock();

      // If the entity already possesses some amount of the resource, make sure
      // to take it into account when updating its balance after the allocation
      if (depositors.end() != depositors.find(entity)) {
         updatedBalance += depositors[entity];
      }

      depositors[entity] = updatedBalance;
      entity->recordResourceAllocation(sharedPtr, updatedBalance);
      totalAmountAllocated += amount;

      if (depositors[entity] <= 0) {
         depositors.erase(entity);
         entity->removeResourceAllocation(sharedPtr);
      }

      entity->mutex.unlock();
      mutex.unlock();
   }

   /***************************************************************************/

   Resource::AllocationStatus Resource::allocate(
      const std::shared_ptr<Tangible> &entity,
      double amount,
      bool triggerEvents
   ) {

      if (triggerEvents && !game->event({
         "beforeAllocateResource",
         {triggers.get(), entity->getEventListener()},
         {this, entity.get(), amount}
      })) {
         return ALLOCATE_OR_FREE_ABORT;
      }

      if (amount <= 0) {

         if (triggerEvents) {
            game->event({
               "allocateResourceZeroOrNegativeAmount",
               {triggers.get(), entity->getEventListener()},
               {this, entity.get(), amount}
            });
         }

         return ALLOCATE_ZERO_OR_NEGATIVE_AMOUNT;
      }

      auto shared = getShared();

      if (std::get<bool>(*getProperty(ReqIntAllocProperty))) {

         double intPart, fracPart = modf(amount, &intPart);

         if (fracPart) {

            if (triggerEvents) {
               game->event({
                  "allocateResourceIntegerRequired",
                  {triggers.get(), entity->getEventListener()},
                  {this, entity.get(), amount}
               });
            }

            return ALLOCATE_INT_REQUIRED;
         }
      }

      auto amountAvailable = getProperty(AmtAvailProperty);

      if (amountAvailable && amount + totalAmountAllocated > std::get<double>(*amountAvailable)) {

         if (triggerEvents) {
            game->event({
               "allocateResourceTotalAmountExceeded",
               {triggers.get(), entity->getEventListener()},
               {this, entity.get(), amount}
            });
         }

         return ALLOCATE_TOTAL_AMOUNT_EXCEEDED;
      }

      // If the entity already possesses some amount of the resource, make sure
      // to take it into account when updating its balance after the allocation
      double updatedBalance = amount;

      if (depositors.end() != depositors.find(entity)) {
         updatedBalance += depositors[entity];
      }

      auto maxAmountPerDepositor = getProperty(MaxAmtPerDepositorProperty);

      if (maxAmountPerDepositor && updatedBalance > std::get<double>(*maxAmountPerDepositor)) {

         if (triggerEvents) {
            game->event({
               "allocateResourceMaxPerDepositorExceeded",
               {triggers.get(), entity->getEventListener()},
               {this, entity.get(), amount}
            });
         }

         return ALLOCATE_MAX_PER_DEPOSITOR_EXCEEDED;
      }

      allocateRaw(entity, amount);

      if (triggerEvents) {
         game->event({
            "afterAllocateResource",
            {triggers.get(), entity->getEventListener()},
            {this, entity.get(), amount}
         });
      }

      return ALLOCATE_OR_FREE_SUCCESS;
   }

   /***************************************************************************/

   Resource::AllocationStatus Resource::free(
      const std::shared_ptr<Tangible> &entity,
      double amount,
      bool triggerEvents
   ) {

      if (triggerEvents && !game->event({
         "beforeFreeResource",
         {triggers.get(), entity->getEventListener()},
         {this, entity.get(), amount}
      })) {
         return ALLOCATE_OR_FREE_ABORT;
      }

      if (amount < 0) {

         if (triggerEvents) {
            game->event({
               "freeResourceNegativeValue",
               {triggers.get(), entity->getEventListener()},
               {this, entity.get(), amount}
            });
         }

         return FREE_NEGATIVE_VALUE;
      }

      if (std::get<bool>(*getProperty(ReqIntAllocProperty))) {

         double intPart, fracPart = modf(amount, &intPart);

         if (fracPart) {

            if (triggerEvents) {
               game->event({
                  "freeResourceIntegerRequired",
                  {triggers.get(), entity->getEventListener()},
                  {this, entity.get(), amount}
               });
            }

            return FREE_INT_REQUIRED;
         }
      }

      auto shared = getShared();

      if (
         depositors.end() == depositors.find(entity) ||
         depositors[entity] < amount
      ) {

         if (triggerEvents) {
            game->event({
               "freeResourceExceedsAllocation",
               {triggers.get(), entity->getEventListener()},
               {this, entity.get(), amount}
            });
         }

         return FREE_EXCEEDS_ALLOCATION;
      }

      // Free everything
      if (0 == amount) {
         amount = depositors[entity];
      }

      allocateRaw(entity, -amount);

      if (triggerEvents) {
         game->event({
            "afterFreeResource",
            {triggers.get(), entity->getEventListener()},
            {this, entity.get(), amount}
         });
      }

      return ALLOCATE_OR_FREE_SUCCESS;
   }

   /***************************************************************************/

   Resource::AllocationStatus Resource::transfer(
      const std::shared_ptr<Tangible> &depositor,
      const std::shared_ptr<Tangible> &beneficiary,
      double amount,
      bool triggerEvents
   ) {

      if (triggerEvents && !game->event({
         "beforeTransferResource",
         {triggers.get(), depositor->getEventListener(), beneficiary->getEventListener()},
         {this, depositor.get(), beneficiary.get(), amount}
      })) {
         return ALLOCATE_OR_FREE_ABORT;
      }

      // The transfer should be transactional
      transferMutex.lock();

      auto status = free(depositor, amount, false);

      if (ALLOCATE_OR_FREE_SUCCESS != status) {

         game->event({
            "transferResourceCantFree",
            {triggers.get(), depositor->getEventListener(), beneficiary->getEventListener()},
            {this, depositor.get(), beneficiary.get(), amount}
         });

         transferMutex.unlock();
         return status;
      }

      status = allocate(beneficiary, amount, false);

      if (ALLOCATE_OR_FREE_SUCCESS != status) {

         game->event({
            "transferResourceCantAllocate",
            {triggers.get(), depositor->getEventListener(), beneficiary->getEventListener()},
            {this, depositor.get(), beneficiary.get(), amount}
         });

         // Return the depositor's previous allocation because the transfer failed
         allocateRaw(depositor, amount);

         transferMutex.unlock();
         return status;
      }

      transferMutex.unlock();

      game->event({
         "afterTransferResource",
         {triggers.get(), depositor->getEventListener(), beneficiary->getEventListener()},
         {this, depositor.get(), beneficiary.get(), amount}
      });

      return ALLOCATE_OR_FREE_SUCCESS;
   }
}
