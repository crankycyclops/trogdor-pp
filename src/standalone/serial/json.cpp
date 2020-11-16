#include <trogdor/serial/serializable.h>
#include "../include/serial/json.h"

namespace trogdor::serial {


	std::any Json::serialize(const std::shared_ptr<Serializable> &data) {

		std::any output;

		// TODO
		return output;
	}

	/************************************************************************/

	std::shared_ptr<Serializable> Json::deserialize(const std::any &data) {

		std::shared_ptr<Serializable> obj = std::make_shared<Serializable>();

		// TODO
		return obj;
	}
}
