#ifndef SCOPE_ENTITY_H
#define SCOPE_ENTITY_H


#include <trogdor/entities/entity.h>
#include "controller.h"


class EntityController: public ScopeController {

	protected:

		// Singleton instance of EntityController
		static std::unique_ptr<EntityController> instance;

		// Actions served by the "entity" scope
		static const char *LIST_ACTION;

		// Error messages
		static const char *MISSING_ENTITY_NAME;
		static const char *INVALID_ENTITY_NAME;
		static const char *ENTITY_NOT_FOUND;

		// Constructor should only be called internally by get(), which will
		// ensure we only ever have a single instance of the class.
		EntityController();
		EntityController(const EntityController &) = delete;

		// Converts an entity to a JSON object
		JSONObject entityToJSONObject(trogdor::entity::Entity *ePtr);

	public:

		// Scope name that should be used in requests
		static const char *SCOPE;

		// Returns singleton instance of EntityController.
		static std::unique_ptr<EntityController> &get();

		// Returns details about the entity in the specified game
		JSONObject getEntity(JSONObject request);

		// Returns a list of all entities in the specified game
		JSONObject getEntityList(JSONObject request);
};


#endif
