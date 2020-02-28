#ifndef SCOPE_ENTITY_H
#define SCOPE_ENTITY_H


#include <trogdor/entitymap.h>
#include "controller.h"


class EntityController: public ScopeController {

	private:

		// Singleton instance of EntityController
		static std::unique_ptr<EntityController> instance;

	protected:

		// Actions served by the "entity" scope
		static const char *LIST_ACTION;
		static const char *OUTPUT_ACTION;
		static const char *INPUT_ACTION;

		// Error messages
		static const char *MISSING_ENTITY_NAME;
		static const char *INVALID_ENTITY_NAME;
		static const char *ENTITY_NOT_FOUND;

		// Constructor should only be called internally by get(), which will
		// ensure we only ever have a single instance of the class.
		EntityController();
		EntityController(const EntityController &) = delete;

		// Converts an entity to a JSON object
		static JSONObject entityToJSONObject(trogdor::entity::Entity *ePtr);

		// Returns a pointer to the game entity of the specified name. More
		// specific entity controllers should return only pointers of the
		// correct type. For example, EntityController's implementation will
		// return the result of Game::getEntity(), while PlayerController's
		// implementation should return the result of Game::getPlayer().
		// Should throw an instance of EntityNotFound (or one of its children)
		// if the entity cannot be found.
		virtual trogdor::entity::Entity *getEntityPtr(
			std::unique_ptr<trogdor::Game> &game,
			std::string entityName
		);

		// Returns an iterable list of all entity pointers in the game. More
		// specific entity controllers should only return a list of pointers
		// of their type. For example, EntityController's implementation calls
		// Game::getEntities(), while PlayerController's implementation calls
		// Game::getPlayers().
		virtual const trogdor::entity::EntityMap getEntityPtrList(
			std::unique_ptr<trogdor::Game> &game
		);

	public:

		// Scope name that should be used in requests
		static const char *SCOPE;

		// Returns singleton instance of EntityController.
		static std::unique_ptr<EntityController> &get();

		// Returns details about the entity in the specified game
		JSONObject getEntity(JSONObject request);

		// Returns a list of all entities in the specified game
		JSONObject getEntityList(JSONObject request);

		// Returns all unfetched output messages from the given channel for an
		// entity.
		JSONObject getOutput(JSONObject request);

		// Appends an output message to the given entity on the given channel.
		JSONObject appendOutput(JSONObject request);

		// Sends input on behalf of an entity.
		JSONObject postInput(JSONObject request);
};


#endif
