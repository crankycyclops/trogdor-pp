#ifndef SCOPE_RESOURCE_H
#define SCOPE_RESOURCE_H


#include <trogdor/entities/resource.h>
#include "entity.h"


class ResourceController: public EntityController {

	private:

		// Singleton instance of ResourceController
		static std::unique_ptr<ResourceController> instance;

	protected:

		// Converts a resource to a JSON object
		virtual rapidjson::Value entityToJSONObject(
			trogdor::entity::Entity *ePtr,
			rapidjson::MemoryPoolAllocator<> &allocator
		);

		// Returns a pointer to the resource of the specified name. Throws an
		// instance of ResourceNotFound if it doesn't exist.
		virtual trogdor::entity::Entity *getEntityPtr(
			std::unique_ptr<trogdor::Game> &game,
			std::string entityName
		);

		// Returns an iterable list of all resource pointers in the game.
		virtual std::vector<trogdor::entity::Entity *> getEntityPtrList(
			std::unique_ptr<trogdor::Game> &game
		);

		// Constructor should only be called internally by get(), which will
		// ensure we only ever have a single instance of the class.
		ResourceController();
		ResourceController(const ResourceController &) = delete;

	public:

		// Scope name that should be used in requests
		static constexpr const char *SCOPE = "resource";

		// Returns singleton instance of ResourceController.
		static std::unique_ptr<ResourceController> &get();

		// Returns the scope's name
		virtual const char *getName();
};


#endif
