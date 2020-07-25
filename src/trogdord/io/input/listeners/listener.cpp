#include <trogdor/entities/player.h>

#include "../../../include/json.h"
#include "../../../include/gamecontainer.h"
#include "../../../include/io/input/listeners/listener.h"

namespace input {


	// Invalid input will be ignored. No notification will be sent back to the
	// outside process.
	void Listener::dispatch(std::string input) {

		JSONObject data;

		try {
			data = JSON::deserialize(input);
		}

		catch (const boost::property_tree::json_parser_error &e) {
			return;
		}

		boost::optional gameId = data.get_optional<size_t>("game_id");
		boost::optional entity = data.get_optional<std::string>("entity");
		boost::optional command = data.get_optional<std::string>("command");

		if (gameId && entity && command) {

			if (auto &game = GameContainer::get()->getGame(*gameId)) {
				if (auto player = game->get()->getPlayer(*entity)) {
					player->input(*command);
				}
			}
		}
	}
}
