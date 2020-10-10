#ifndef SCOPE_PLAYER_H
#define SCOPE_PLAYER_H


#include <trogdor/entities/player.h>
#include "being.h"


class PlayerController: public BeingController {

	private:

		// Singleton instance of PlayerController
		static std::unique_ptr<PlayerController> instance;

	protected:

		// Actions served by the "Player" scope
		static const char *INPUT_ACTION;

		// Error messages
		static const char *MISSING_PLAYER_NAME;
		static const char *INVALID_PLAYER_NAME;
		static const char *INVALID_COMMAND;
		static const char *PLAYER_NOT_FOUND;

		// Converts a player to a JSON object
		virtual rapidjson::Document entityToJSONObject(trogdor::entity::Entity *ePtr);

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
		static const char *SCOPE;

		// Returns singleton instance of PlayerController.
		static std::unique_ptr<PlayerController> &get();

		// Creates a player in the specified game
		rapidjson::Document createPlayer(const rapidjson::Document &request);

		// Removes a player in the specified game
		rapidjson::Document destroyPlayer(const rapidjson::Document &request);

		// Sends input on behalf of a player
		rapidjson::Document postInput(const rapidjson::Document &request);
};


#endif
