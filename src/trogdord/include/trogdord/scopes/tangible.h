#ifndef SCOPE_TANGIBLE_H
#define SCOPE_TANGIBLE_H


#include <trogdor/entities/tangible.h>
#include "entity.h"


class TangibleController: public EntityController {

	private:

		// Singleton instance of TangibleController
		static std::unique_ptr<TangibleController> instance;

	protected:

		// Converts a tangible to a JSON object
		virtual rapidjson::Value entityToJSONObject(
			trogdor::entity::Entity *ePtr,
			rapidjson::MemoryPoolAllocator<> &allocator
		);

		// Returns a pointer to the tangible of the specified name. Throws an
		// instance of TangibleNotFound if it doesn't exist.
		virtual trogdor::entity::Entity *getEntityPtr(
			std::unique_ptr<trogdor::Game> &game,
			std::string entityName
		);

		// Returns an iterable list of all tangible pointers in the game.
		virtual std::vector<trogdor::entity::Entity *> getEntityPtrList(
			std::unique_ptr<trogdor::Game> &game
		);

		// Constructor should only be called internally by get(), which will
		// ensure we only ever have a single instance of the class.
		TangibleController();
		TangibleController(const TangibleController &) = delete;

	public:

		// Scope name that should be used in requests
		static constexpr const char *SCOPE = "tangible";

		// Returns singleton instance of TangibleController.
		static std::unique_ptr<TangibleController> &get();

		// Returns the scope's name
		virtual const char *getName();
};


#endif
