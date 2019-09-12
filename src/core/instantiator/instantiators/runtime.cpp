#include "../../include/instantiator/instantiators/runtime.h"


using namespace std;

namespace trogdor { namespace core {

   void Runtime::makeEntityClass(string name, enum entity::EntityType) {

   }

   /***************************************************************************/

   void Runtime::setEntityClassProperty(string className, string property,
   string value) {

   }

   /***************************************************************************/

   void Runtime::makeEntity(string entityName, enum entity::EntityType type,
   string className) {

   }

   /***************************************************************************/

   void Runtime::setEntityProperty(string entityName, string property,
   string value) {

   }

   /***************************************************************************/

   void Runtime::setDefaultPlayerProperty(string property, string value) {

   }

   /***************************************************************************/

   void Runtime::loadGameScript(string script, enum LoadScriptMethod method) {

   }

   /***************************************************************************/

   void Runtime::loadEntityScript(string entityName, string script,
   enum LoadScriptMethod method) {

   }

   /***************************************************************************/

   void Runtime::setGameEvent(string eventName, string function) {

   }

   /***************************************************************************/

   void Runtime::setEntityEvent(string entityName, string eventName,
   string function) {

   }

   /***************************************************************************/

   void Runtime::setEntityMessage(string entityName, string messageName,
   string message) {

   }

   /***************************************************************************/

   // Doesn't actually do anything, but required by the parent class.
   void Runtime::instantiate() {}
}}

