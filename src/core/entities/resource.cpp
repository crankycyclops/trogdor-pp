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

   void Resource::setPropertyValidators() {

      setPropertyValidator(ReqIntAllocProperty, [&](PropertyValue v) -> int {return isPropertyValueBool(v);});
      setPropertyValidator(PluralNameProperty, [&](PropertyValue v) -> int {return isPropertyValueString(v);});
      setPropertyValidator(PluralTitleProperty, [&](PropertyValue v) -> int {return isPropertyValueString(v);});
      setPropertyValidator(MaxAmtPerDepositorProperty, [&](PropertyValue v) -> int {return isPropertyValueDouble(v);});

      // Add property validators after setting initial values of properties for
      // efficiency (I know the defaults are going to be valid, so there's no
      // reason to call a validator.)
      setPropertyValidator(AmtAvailProperty, [&](PropertyValue value) -> int {

         // Value must be a double (only literals with decimals or integers that
         // are explictly cast to double will pass validation when passed to
         // setProperty.)
         if (PROPERTY_VALID != isPropertyValueDouble(value)) {
            return PROPERTY_INVALID_TYPE;
         }

         double newAmount = std::get<double>(value);

         if (newAmount < totalAmountAllocated) {
            return AMOUNT_AVAILABLE_TOO_SMALL;
         }

         return PROPERTY_VALID;
      });
   }

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
      setProperty(PluralTitleProperty, getProperty<std::string>(PluralNameProperty));

      setPropertyValidators();
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

   Resource::Resource(Game *g, const serial::Serializable &data): Entity(g, data) {

      g->addCallback("afterDeserialize",
      std::make_shared<Entity::EntityCallback>([&](std::any) -> bool {

         std::vector<std::shared_ptr<serial::Serializable>> serializedDepositors =
            std::get<std::vector<std::shared_ptr<serial::Serializable>>>(*data.get("depositors"));

         for (auto const &depositor: serializedDepositors) {

            std::shared_ptr<Tangible> owner = g->getTangible(std::get<std::string>(*depositor->get("depositor")));

            if (owner) {
               depositors[owner] = std::get<double>(*depositor->get("amount"));
               totalAmountAllocated += std::get<double>(*depositor->get("amount"));
            }
         }

         return true;
      }));

      types.push_back(ENTITY_RESOURCE);
      setPropertyValidators();
   }

   /***************************************************************************/

   std::shared_ptr<serial::Serializable> Resource::serialize() {

      std::shared_ptr<serial::Serializable> data = Entity::serialize();
      std::vector<std::shared_ptr<serial::Serializable>> serializedDepositors;

      for (const auto &depositor: depositors) {

         if (const auto owner = depositor.first.lock()) {

            std::shared_ptr<serial::Serializable> serializedDepositor = std::make_shared<serial::Serializable>();

            serializedDepositor->set("depositor", owner->getName());
            serializedDepositor->set("amount", depositor.second);
            serializedDepositors.push_back(serializedDepositor);
         }
      }

      // totalAmountAllocated will be reconstructed during deserialization of
      // depositors and doesn't need to be saved
      data->set("depositors", serializedDepositors);
      return data;
   }

   /***************************************************************************/

   std::string Resource::hydrateString(std::string templateStr, bool isPlural) {

      std::string result = templateStr;

      std::unordered_map<
         bool,
         std::unordered_map<std::string, std::string>
      > templateParameters = {

         {false, {
            {"{%title}", getProperty<std::string>(TitleProperty)},
            {"{%name}", getName()},
            {"{%Title}", capitalize(getProperty<std::string>(TitleProperty))},
            {"{%Name}", capitalize(getName())}
         }},

         {true, {
            {"{%title}", getProperty<std::string>(PluralTitleProperty)},
            {"{%name}", getProperty<std::string>(PluralNameProperty)},
            {"{%Title}", capitalize(getProperty<std::string>(PluralTitleProperty))},
            {"{%Name}", capitalize(getProperty<std::string>(PluralNameProperty))}
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

         if (isPropertySet(LongDescProperty) && getProperty<std::string>(LongDescProperty).length() > 0) {
            observer->out("display") << hydrateString(getProperty<std::string>(LongDescProperty), isPlural)
               << std::endl;
         }

         else if (isPropertySet(ShortDescProperty) && getProperty<std::string>(ShortDescProperty).length() > 0) {
            observer->out("display") << hydrateString(getProperty<std::string>(ShortDescProperty), isPlural)
               << std::endl;
         }

         else {
            observer->out("display") << "You see " <<
               (isPlural ? getProperty<std::string>(PluralTitleProperty) : getProperty<std::string>(TitleProperty)) <<
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
         getProperty<bool>(ReqIntAllocProperty) && 1 == amount ? false : true
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

      if (getProperty<bool>(ReqIntAllocProperty)) {

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

      if (
         isPropertySet(AmtAvailProperty) &&
         amount + totalAmountAllocated > getProperty<double>(AmtAvailProperty)
      ) {

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

      if (
         isPropertySet(MaxAmtPerDepositorProperty) &&
         updatedBalance > getProperty<double>(MaxAmtPerDepositorProperty)
      ) {

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

      if (getProperty<bool>(ReqIntAllocProperty)) {

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
