#include <trogdor/serial/drivers/sqlite.h>
#include <trogdor/serial/serializable.h>
#include <trogdor/exception/undefinedexception.h>

namespace trogdor::serial {


	std::any Sqlite::serialize(const std::shared_ptr<Serializable> &data) {

		// TODO
        return std::string("");
	}

	/************************************************************************/

	std::shared_ptr<Serializable> Sqlite::deserialize(const std::any &data) {

        // TODO
        return std::shared_ptr<Serializable>();
	}
}
