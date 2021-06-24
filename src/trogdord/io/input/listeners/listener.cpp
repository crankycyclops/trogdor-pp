#include <rapidjson/pointer.h>
#include <trogdor/entities/player.h>

#include <trogdord/json.h>
#include <trogdord/gamecontainer.h>
#include <trogdord/io/input/listeners/listener.h>

namespace input {


	Listener::~Listener() {}

	/************************************************************************/

	// Invalid input will be ignored. No notification will be sent back to the
	// outside process.
	void Listener::dispatch(std::string input) {

		rapidjson::Document data;

		data.Parse(input.c_str());

		if (
			data.HasParseError() ||
			!data.HasMember("game_id") || !data["game_id"].IsUint() ||
			!data.HasMember("entity")  || !data["entity"].IsString() ||
			!data.HasMember("command") || !data["command"].IsString()
		) {
			return;
		}

		size_t gameId = data["game_id"].GetUint();
		std::string entity = data["entity"].GetString();
		std::string command = data["command"].GetString();

		if (auto &game = GameContainer::get()->getGame(gameId)) {
			if (auto player = game->get()->getPlayer(entity)) {
				player->input(command);
			}
		}
	}
}
