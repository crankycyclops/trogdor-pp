#include <trogdor/serial/drivers/sqlite.h>
#include <trogdor/serial/serializable.h>
#include <trogdor/exception/fileexception.h>

namespace trogdor::serial {


   void Sqlite::_insertInt(sqlite3 *db, size_t parent, Sqlite::IntType value, char type, std::string key) {

      sqlite3_stmt *insert;
      std::string query = std::string("INSERT INTO data(key, parent, type, int_val) VALUES (?, ?, '") + type + "', ?)";

      if (sqlite3_prepare_v2(
         db,
         query.c_str(),
         -1,
         &insert,
         nullptr
      )) {
         throw Exception("_insertInt(): Preparing INSERT INTO data query failed");
      }

      int status;

      if (key.length()) {
         status = sqlite3_bind_text(insert, 1, key.c_str(), -1, nullptr);
      } else {
         status = sqlite3_bind_null(insert, 1);
      }

      if (status) {
         throw Exception("_insertInt(): Failed to bind key parameter to INSERT INTO data query");
      }

      if (parent < 1) {
         status = sqlite3_bind_null(insert, 2);
      } else {
         status = sqlite3_bind_int64(insert, 2, parent);
      }

      if (status) {
         throw Exception("_insertInt(): Failed to bind parent parameter to INSERT INTO data query");
      }

      if (sqlite3_bind_int64(insert, 3, value)) {
         throw Exception("_insertInt(): Failed to bind value parameter to INSERT INTO data query");
      }

      if (int status = sqlite3_step(insert); SQLITE_DONE != status) {
         throw Exception("_insertInt(): Failed to execute INSERT INTO DATA query (" + std::to_string(status) + ")");
      }

      lastRowId++;
   }

   /**************************************************************************/

   void Sqlite::insertDouble(sqlite3 *db, size_t parent, double value, std::string key) {

      sqlite3_stmt *insert;

      if (sqlite3_prepare_v2(
         db,
         "INSERT INTO data(key, parent, type, double_val) VALUES (?, ?, 'd', ?)",
         -1,
         &insert,
         nullptr
      )) {
         throw Exception("insertDouble(): Preparing INSERT INTO data query failed");
      }

      int status;

      if (key.length()) {
         status = sqlite3_bind_text(insert, 1, key.c_str(), -1, nullptr);
      } else {
         status = sqlite3_bind_null(insert, 1);
      }

      if (status) {
         throw Exception("insertDouble(): Failed to bind key parameter to INSERT INTO data query");
      }

      if (parent < 1) {
         status = sqlite3_bind_null(insert, 2);
      } else {
         status = sqlite3_bind_int64(insert, 2, parent);
      }

      if (status) {
         throw Exception("insertDouble(): Failed to bind parent parameter to INSERT INTO data query");
      }

      if (sqlite3_bind_double(insert, 3, value)) {
         throw Exception("insertDouble(): Failed to bind value parameter to INSERT INTO data query");
      }

      if (int status = sqlite3_step(insert); SQLITE_DONE != status) {
         throw Exception("insertDouble(): Failed to execute INSERT INTO DATA query (" + std::to_string(status) + ")");
      }

      lastRowId++;
   }

   /**************************************************************************/

   void Sqlite::insertString(sqlite3 *db, size_t parent, std::string value, std::string key) {

      sqlite3_stmt *insert;

      if (sqlite3_prepare_v2(
         db,
         "INSERT INTO data(key, parent, type, string_val) VALUES (?, ?, 's', ?)",
         -1,
         &insert,
         nullptr
      )) {
         throw Exception("insertString(): Preparing INSERT INTO data query failed");
      }

      int status;

      if (key.length()) {
         status = sqlite3_bind_text(insert, 1, key.c_str(), -1, nullptr);
      } else {
         status = sqlite3_bind_null(insert, 1);
      }

      if (status) {
         throw Exception("insertString(): Failed to bind key parameter to INSERT INTO data query");
      }

      if (parent < 1) {
         status = sqlite3_bind_null(insert, 2);
      } else {
         status = sqlite3_bind_int64(insert, 2, parent);
      }

      if (status) {
         throw Exception("insertString(): Failed to bind parent parameter to INSERT INTO data query");
      }

      if (sqlite3_bind_text(insert, 3, value.c_str(), -1, nullptr)) {
         throw Exception("insertString(): Failed to bind value parameter to INSERT INTO data query");
      }

      if (int status = sqlite3_step(insert); SQLITE_DONE != status) {
         throw Exception("insertString(): Failed to execute INSERT INTO DATA query (" + std::to_string(status) + ")");
      }

      lastRowId++;
   }

   /**************************************************************************/

   void Sqlite::insertObjectValue(sqlite3 *db, size_t parent, std::string key) {

      sqlite3_stmt *insert;

      if (sqlite3_prepare_v2(
         db,
         "INSERT INTO data(key, parent, type) VALUES (?, ?, 'o')",
         -1,
         &insert,
         nullptr
      )) {
         throw Exception("insertObjectValue(): Preparing INSERT INTO data query failed");
      }

      int status;

      if (key.length()) {
         status = sqlite3_bind_text(insert, 1, key.c_str(), -1, nullptr);
      } else {
         status = sqlite3_bind_null(insert, 1);
      }

      if (status) {
         throw Exception("insertObjectValue(): Failed to bind key parameter to INSERT query");
      }

      if (lastRowId < 1) {
         status = sqlite3_bind_null(insert, 2);
      } else {
         status = sqlite3_bind_int64(insert, 2, parent);
      }

      if (status) {
         throw Exception("insertObjectValue(): Failed to bind parent parameter to INSERT query");
      }

      if (int status = sqlite3_step(insert); SQLITE_DONE != status) {
         throw Exception("insertObjectValue(): Failed to execute INSERT query (" + std::to_string(status) + ")");
      }

      lastRowId++;
   }

   /**************************************************************************/

   void Sqlite::insertArrayValue(sqlite3 *db, size_t parent, std::string key) {

      sqlite3_stmt *insert;

      if (sqlite3_prepare_v2(
         db,
         "INSERT INTO data(key, parent, type) VALUES (?, ?, 'a')",
         -1,
         &insert,
         nullptr
      )) {
         throw Exception("insertArrayValue(): Preparing INSERT INTO data query failed");
      }

      int status;

      if (key.length()) {
         status = sqlite3_bind_text(insert, 1, key.c_str(), -1, nullptr);
      } else {
         status = sqlite3_bind_null(insert, 1);
      }

      if (status) {
         throw Exception("insertArrayValue(): Failed to bind key parameter to INSERT query");
      }

      if (lastRowId < 1) {
         status = sqlite3_bind_null(insert, 2);
      } else {
         status = sqlite3_bind_int64(insert, 2, parent);
      }

      if (status) {
         throw Exception("insertArrayValue(): Failed to bind parent parameter to INSERT query");
      }

      if (int status = sqlite3_step(insert); SQLITE_DONE != status) {
         throw Exception("insertArrayValue(): Failed to execute INSERT query (" + std::to_string(status) + ")");
      }

      lastRowId++;
   }

   /**************************************************************************/

   void Sqlite::serializeSizeT(std::any data, std::string key, const size_t &value) {

      std::tuple<sqlite3 *, size_t> handle = std::any_cast<std::tuple<sqlite3 *, size_t>>(data);
      insertUnsigned(std::get<0>(handle), std::get<1>(handle), value, key);
   }

   /**************************************************************************/

   void Sqlite::serializeInt(std::any data, std::string key, int const &value) {

      std::tuple<sqlite3 *, size_t> handle = std::any_cast<std::tuple<sqlite3 *, size_t>>(data);
      insertInt(std::get<0>(handle), std::get<1>(handle), value, key);
   }

   /**************************************************************************/

   void Sqlite::serializeDouble(std::any data, std::string key, const double &value) {

      std::tuple<sqlite3 *, size_t> handle = std::any_cast<std::tuple<sqlite3 *, size_t>>(data);
      insertDouble(std::get<0>(handle), std::get<1>(handle), value, key);
   }

   /**************************************************************************/

   void Sqlite::serializeBool(std::any data, std::string key, const bool &value) {

      std::tuple<sqlite3 *, size_t> handle = std::any_cast<std::tuple<sqlite3 *, size_t>>(data);
      insertBool(std::get<0>(handle), std::get<1>(handle), value, key);
   }

   /**************************************************************************/

   void Sqlite::serializeString(std::any data, std::string key, const std::string &value) {

      std::tuple<sqlite3 *, size_t> handle = std::any_cast<std::tuple<sqlite3 *, size_t>>(data);
      insertString(std::get<0>(handle), std::get<1>(handle), value, key);
   }

   /**************************************************************************/

   void Sqlite::serializeSerializable(
      std::any data,
      std::string key,
      const std::shared_ptr<Serializable> &value
   ) {

      std::tuple<sqlite3 *, size_t> handle = std::any_cast<std::tuple<sqlite3 *, size_t>>(data);

      insertObjectValue(std::get<0>(handle), std::get<1>(handle), key);
      doSerialize(value, std::tuple<sqlite3 *, size_t>({std::get<0>(handle), lastRowId}));
   }

   /**************************************************************************/

   void Sqlite::serializeStringVector(
      std::any data,
      std::string key,
      const std::vector<std::string> &value
   ) {

      size_t arrayId;
      std::tuple<sqlite3 *, size_t> handle = std::any_cast<std::tuple<sqlite3 *, size_t>>(data);

      insertArrayValue(std::get<0>(handle), std::get<1>(handle), key);
      arrayId = lastRowId;

      for (const auto &strVal: value) {
         insertString(std::get<0>(handle), arrayId, strVal);
      }
   }

   /**************************************************************************/

   void Sqlite::serializeSerializableVector(
      std::any data,
      std::string key,
      const std::vector<std::shared_ptr<Serializable>> &value
   ) {

      size_t arrayId;
      std::tuple<sqlite3 *, size_t> handle = std::any_cast<std::tuple<sqlite3 *, size_t>>(data);

      insertArrayValue(std::get<0>(handle), std::get<1>(handle), key);
      arrayId = lastRowId;

      for (const auto &obj: value) {

         size_t childId = lastRowId + 1;

         insertObjectValue(std::get<0>(handle), arrayId);
         doSerialize(obj, std::tuple<sqlite3 *, size_t>({std::get<0>(handle), childId}));
      }
   }

   /**************************************************************************/

   void Sqlite::createSchema(sqlite3 *db) {

      char *zErrMsg = nullptr;

      if (sqlite3_exec(
         db,
         "CREATE TABLE data("
         "  key TEXT,"
         "  parent BIGINT,"
         "  type CHAR(1) NOT NULL,"
         "  int_val BIGINT,"
         "  double_val DOUBLE,"
         "  string_val TEXT"
         ")",
         nullptr,
         nullptr,
         &zErrMsg
      )) {
         throwSqliteError(zErrMsg);
      }
   }

   /**************************************************************************/

   std::any Sqlite::serialize(const std::shared_ptr<Serializable> &data) {

      sqlite3 *db;

      if (SQLITE_OK != sqlite3_open(":memory:", &db)) {
         throw Exception("Failed to initialize in-memory database");
      }

      createSchema(db);
      doSerialize(data, std::tuple<sqlite3 *, size_t>({db, 0}));

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

   void Sqlite::writeToDisk(std::any data, std::string filename) {

      sqlite3 *output;
      sqlite3_backup *backup;

      if (SQLITE_OK != sqlite3_open(filename.c_str(), &output)) {
         throw FileException("Cannot open " + filename + " for writing");
      }

      backup = sqlite3_backup_init(output, "main", std::any_cast<sqlite3 *>(data), "main");

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
