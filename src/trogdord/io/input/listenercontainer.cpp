#include <trogdord/json.h>
#include <trogdord/config.h>
#include <trogdord/io/input/listenercontainer.h>

#ifdef ENABLE_REDIS
	#include <trogdord/io/input/listeners/redis.h>
#endif

namespace input {


	// Singleton instance of input::ListenerContainer
	std::unique_ptr<ListenerContainer> ListenerContainer::instance = nullptr;

	/************************************************************************/

	ListenerContainer::ListenerContainer() {

		rapidjson::Document listenersArr;

		std::string listenersStr = Config::get()->getString(
			Config::CONFIG_KEY_INPUT_LISTENERS
		);

		listenersArr.Parse(listenersStr.c_str());

		if (listenersArr.HasParseError()) {
			throw ServerException(
				std::string("invalid trogdord.ini value for ") +
				Config::CONFIG_KEY_INPUT_LISTENERS
			);
		}

		else if (rapidjson::kArrayType != listenersArr.GetType()) {
			throw ServerException(
				std::string("trogdord.ini value for ") +
				Config::CONFIG_KEY_INPUT_LISTENERS +
				" must be a JSON array of strings."
			);
		}

		// Instantiate all listeners (only those that have been configured in
		// trogdord.ini will actually be started.)
		#ifdef ENABLE_REDIS
			listeners[Redis::LISTENER_NAME] = std::make_unique<Redis>();
		#endif

		// Start configured listeners
		for (auto listener = listenersArr.Begin(); listener != listenersArr.End(); listener++) {

			if (rapidjson::kStringType == listener->GetType()) {

				if (listeners.find(listener->GetString()) != listeners.end()) {
					listeners[listener->GetString()]->start();
					Config::get()->err(trogdor::Trogerr::INFO) <<
						"Starting " << listener->GetString() <<
						" input listener." << std::endl;
				}

				else {
					throw ServerException(
						std::string("Input listener ") +
						listener->GetString() +
						" doesn't exist."
					);
				}
			}

			else {
				throw ServerException(
					std::string("trogdord.ini value for ") +
						Config::CONFIG_KEY_INPUT_LISTENERS +
						" must be a JSON array of strings."
				);
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
