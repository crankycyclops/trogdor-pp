#include <trogdor/serial/drivers/sqlite.h>
#include <trogdor/serial/serializable.h>
#include <trogdor/exception/fileexception.h>

namespace trogdor::serial {


	std::any Sqlite::serialize(const std::shared_ptr<Serializable> &data) {

		sqlite3 *db;

		if (SQLITE_OK != sqlite3_open(":memory:", &db)) {
			throw Exception("Failed to initialize in-memory database");
		}

		// TODO
        return db;
	}

	/************************************************************************/

	std::shared_ptr<Serializable> Sqlite::deserialize(const std::any &data) {

        sqlite3 *db;

		std::string filename = typeid(const char *) == data.type() ?
			std::any_cast<const char *>(data) : std::any_cast<std::string>(data);

        if (SQLITE_OK != sqlite3_open(filename.c_str(), &db)) {
			throw FileException(filename + " does not exist or is not a SQLite3 database");
		}

        // TODO: do stuff
		sqlite3_close(db);
        return std::shared_ptr<Serializable>();
	}
}
