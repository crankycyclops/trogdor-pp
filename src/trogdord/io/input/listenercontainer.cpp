#include "../../include/json.h"
#include "../../include/config.h"
#include "../../include/io/input/listenercontainer.h"

#ifdef ENABLE_REDIS
	#include "../../include/io/input/listeners/redis.h"
#endif

namespace input {


	// Singleton instance of input::ListenerContainer
	std::unique_ptr<ListenerContainer> ListenerContainer::instance = nullptr;

	/************************************************************************/

	ListenerContainer::ListenerContainer() {

		JSONObject listenersArr;

		std::string listenersStr = Config::get()->value<std::string>(
			Config::CONFIG_KEY_INPUT_LISTENERS
		);

		try {
			listenersArr = JSON::deserialize(listenersStr);
		}

		catch (const boost::property_tree::json_parser_error &e) {
			throw ServerException(
				std::string("invalid trogdord.ini value for ") +
				Config::CONFIG_KEY_INPUT_LISTENERS + ":" + e.what()
			);
		}

		// Instantiate all listeners (only those that have been configured in
		// trogdord.ini will actually be started.)
		#ifdef ENABLE_REDIS
			listeners[Redis::LISTENER_NAME] = std::make_unique<Redis>();
		#endif

		// Start configured listeners
		for (auto &listener: listenersArr) {

			if (!listener.second.data().empty()) {

				if (listeners.find(listener.second.data()) != listeners.end()) {
					listeners[listener.second.data()]->start();
					Config::get()->err(trogdor::Trogerr::INFO) <<
						"Started " + listener.second.data() + " input listener." <<
						std::endl;
				}

				else {
					throw ServerException(
						std::string("Input listener ") +
						listener.second.data() +
						" doesn't exist."
					);
				}
			}
		}
	}

	/************************************************************************/

	ListenerContainer::~ListenerContainer() {

		for (auto &listener: listeners) {
			if (listener.second->isStarted()) {
				listener.second->stop();
			}
		}

		listeners.clear();
	}

	/************************************************************************/

	std::unique_ptr<ListenerContainer> &ListenerContainer::get() {

		if (!instance) {
			instance = std::unique_ptr<ListenerContainer>(new ListenerContainer());
		}

		return instance;
	}
}
