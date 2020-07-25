#ifndef SCOPE_ENTITY_H
#define SCOPE_ENTITY_H


#include <vector>
#include "controller.h"


class EntityController: public ScopeController {

	private:

		// Singleton instance of EntityController
		static std::unique_ptr<EntityController> instance;

	protected:

		// Actions served by the "entity" scope
		static const char *LIST_ACTION;
		static const char *OUTPUT_ACTION;

		// Error messages
		static const char *MISSING_OUTPUT_MESSAGE;
		static const char *MISSING_COMMAND;
		static const char *MISSING_CHANNEL;
		static const char *INVALID_CHANNEL;
		static const char *MISSING_ENTITY_NAME;
		static const char *INVALID_ENTITY_NAME;
		static const char *ENTITY_NOT_FOUND;

		// Constructor should only be called internally by get(), which will
		// ensure we only ever have a single instance of the class.
		EntityController();
		EntityController(const EntityController &) = delete;

		// Converts an entity to a JSON object
		virtual JSONObject entityToJSONObject(trogdor::entity::Entity *ePtr);

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
		virtual std::vector<trogdor::entity::Entity *> getEntityPtrList(
			std::unique_ptr<trogdor::Game> &game
		);

		// Parses a request and sets the gameId, entityName and ePtr
		// references to their appropriate values. Returns a JSONObject
		// response if there was an error and nothing if the call was
		// successful.
		std::optional<JSONObject> getEntityHelper(
			JSONObject request,
			size_t &gameId,
			std::string &entityName,
			trogdor::entity::Entity *&ePtr,
			std::string missingNameMsg = MISSING_ENTITY_NAME,
			std::string invalidNameMsg = INVALID_ENTITY_NAME,
			std::string notFoundMsg = ENTITY_NOT_FOUND
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
};


#endif
