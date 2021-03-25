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

	/************************************************************************/

	void writeToDisk(sqlite3 *data, std::string filename) {

		sqlite3 *output;
		sqlite3_backup *backup;

		if (SQLITE_OK != sqlite3_open(filename.c_str(), &output)) {
			throw FileException("Cannot open " + filename + " for writing");
		}

		backup = sqlite3_backup_init(output, "main", data, "main");

		if (!backup) {
			throw Exception("Error occurred when attempting to save SQLite3 database to disk");
		}

		sqlite3_backup_step(backup, -1);
		sqlite3_backup_finish(backup);

		if (sqlite3_errcode(output)) {
			throw Exception("Error occurred when attempting to save SQLite3 database to disk");
		}

		sqlite3_close(output);
	}
}
