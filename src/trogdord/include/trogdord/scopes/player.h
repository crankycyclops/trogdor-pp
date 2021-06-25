#ifndef SCOPE_PLAYER_H
#define SCOPE_PLAYER_H


#include <trogdor/entities/player.h>
#include "being.h"


class PlayerController: public BeingController {

	private:

		// Singleton instance of PlayerController
		static std::unique_ptr<PlayerController> instance;

	protected:

		// Converts a player to a JSON object
		virtual rapidjson::Value entityToJSONObject(
			trogdor::entity::Entity *ePtr,
			rapidjson::MemoryPoolAllocator<> &allocator
		);

		// Returns a pointer to the player of the specified name. Throws an
		// instance of PlayerNotFound if the player doesn't exist.
		virtual trogdor::entity::Entity *getEntityPtr(
			std::unique_ptr<trogdor::Game> &game,
			std::string entityName
		);

		// Returns an iterable list of all player pointers in the game.
		virtual std::vector<trogdor::entity::Entity *> getEntityPtrList(
			std::unique_ptr<trogdor::Game> &game
		);

		// Constructor should only be called internally by get(), which will
		// ensure we only ever have a single instance of the class.
		PlayerController();
		PlayerController(const PlayerController &) = delete;

	public:

		// Scope name that should be used in requests
		static constexpr const char *SCOPE = "player";

		// Actions served by the "Player" scope
		static constexpr const char *INPUT_ACTION = "input";

		// Error messages
		static constexpr const char *MISSING_PLAYER_NAME = "missing required player name";
		static constexpr const char *INVALID_PLAYER_NAME = "invalid player name";
		static constexpr const char *INVALID_COMMAND = "command must be a string";
		static constexpr const char *PLAYER_NOT_FOUND = "player not found";

		// Returns singleton instance of PlayerController.
		static std::unique_ptr<PlayerController> &get();

		// Returns the scope's name
		virtual const char *getName();

		// Creates a player in the specified game
		rapidjson::Document createPlayer(const rapidjson::Document &request);

		// Removes a player in the specified game
		rapidjson::Document destroyPlayer(const rapidjson::Document &request);

		// Sends input on behalf of a player
		rapidjson::Document postInput(const rapidjson::Document &request);
};


#endif
