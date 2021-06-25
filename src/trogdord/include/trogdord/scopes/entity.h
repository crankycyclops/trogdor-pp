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
		static constexpr const char *LIST_ACTION = "list";
		static constexpr const char *OUTPUT_ACTION = "output";

		// Constructor should only be called internally by get(), which will
		// ensure we only ever have a single instance of the class.
		EntityController();
		EntityController(const EntityController &) = delete;

		// Converts an entity to a JSON object
		virtual rapidjson::Value entityToJSONObject(
			trogdor::entity::Entity *ePtr,
			rapidjson::MemoryPoolAllocator<> &allocator
		);

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
		// references to their appropriate values. If the call was successful,
		// the returned response object will be empty. Otherwise, it will
		// contain an error code and message to be returned to the client.
		rapidjson::Document getEntityHelper(
			const rapidjson::Document &request,
			size_t &gameId,
			std::string &entityName,
			trogdor::entity::Entity *&ePtr,
			std::string missingNameMsg = MISSING_ENTITY_NAME,
			std::string invalidNameMsg = INVALID_ENTITY_NAME,
			std::string notFoundMsg = ENTITY_NOT_FOUND
		);

	public:

		// Error messages
		static constexpr const char *MISSING_OUTPUT_MESSAGE = "missing required message";
		static constexpr const char *INVALID_OUTPUT_MESSAGE = "message must be a string or other scalar type";
		static constexpr const char *MISSING_COMMAND = "missing required command";
		static constexpr const char *MISSING_CHANNEL = "missing required channel";
		static constexpr const char *INVALID_CHANNEL = "invalid channel";
		static constexpr const char *MISSING_ENTITY_NAME = "missing required entity name";
		static constexpr const char *INVALID_ENTITY_NAME = "invalid entity name";
		static constexpr const char *ENTITY_NOT_FOUND = "entity not found";

		// Scope name that should be used in requests
		static constexpr const char *SCOPE = "entity";

		// Destructor
		virtual ~EntityController();

		// Returns singleton instance of EntityController.
		static std::unique_ptr<EntityController> &get();

		// Returns the scope's name
		virtual const char *getName();

		// Returns details about the entity in the specified game
		rapidjson::Document getEntity(const rapidjson::Document &request);

		// Returns a list of all entities in the specified game
		rapidjson::Document getEntityList(const rapidjson::Document &request);

		// Returns all unfetched output messages from the given channel for an
		// entity.
		rapidjson::Document getOutput(const rapidjson::Document &request);

		// Appends an output message to the given entity on the given channel.
		rapidjson::Document appendOutput(const rapidjson::Document &request);
};


#endif
