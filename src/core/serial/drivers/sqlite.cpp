#include <trogdor/serial/drivers/sqlite.h>
#include <trogdor/serial/serializable.h>
#include <trogdor/exception/fileexception.h>

namespace trogdor::serial {


   /**************************************************************************/

   void Sqlite::serializeSizeT(std::any data, std::string key, const size_t &value) {

      throw UndefinedException("TODO: serializeSizeT");
   }

   /**************************************************************************/

   void Sqlite::serializeInt(std::any data, std::string key, int const &value) {

      throw UndefinedException("TODO: serializeInt");
   }

   /**************************************************************************/

   void Sqlite::serializeDouble(std::any data, std::string key, const double &value) {

      throw UndefinedException("TODO: serializeDouble");
   }

   /**************************************************************************/

   void Sqlite::serializeBool(std::any data, std::string key, const bool &value) {

      throw UndefinedException("TODO: serializeBool");
   }

   /**************************************************************************/

   void Sqlite::serializeString(std::any data, std::string key, const std::string &value) {

      throw UndefinedException("TODO: serializeString");
   }

   /**************************************************************************/

   void Sqlite::serializeSerializable(
      std::any data,
      std::string key,
      const std::shared_ptr<Serializable> &value
   ) {

      throw UndefinedException("TODO: serializeSerializable");
   }

   /**************************************************************************/

   void Sqlite::serializeStringVector(
      std::any data,
      std::string key,
      const std::vector<std::string> &value
   ) {

      throw UndefinedException("TODO: serializeStringVector");
   }

   /**************************************************************************/

   void Sqlite::serializeSerializableVector(
      std::any data,
      std::string key,
      const std::vector<std::shared_ptr<Serializable>> &value
   ) {

      sqlite3_stmt *insert;
      std::tuple<sqlite3 *, size_t> handle = std::any_cast<std::tuple<sqlite3 *, size_t>>(data);

      if (sqlite3_prepare_v2(
         std::get<0>(handle),
         "INSERT INTO data(key, parent, type) VALUES (?, ?, 'a')",
         -1,
         &insert,
         nullptr
      )) {
         throw Exception("serializeSerializableVector(): Preparing INSERT INTO data query failed");
      }

      if (sqlite3_bind_text(insert, 1, key.c_str(), -1, nullptr)) {
         throw Exception("serializeSerializableVector(): Failed to bind key parameter to INSERT query");
      }

      int status;

      if (lastRowId < 1) {
         status = sqlite3_bind_null(insert, 2);
      } else {
         status = sqlite3_bind_int64(insert, 2, std::get<1>(handle));
      }

      if (status) {
         throw Exception("serializeSerializableVector(): Failed to bind parent parameter to INSERT query");
      }

      if (int status = sqlite3_step(insert); SQLITE_DONE != status) {
         throw Exception("serializeSerializableVector(): Failed to execute INSERT query (" + std::to_string(status) + ")");
      }

      // This should be incremented every time an insert is performed
      lastRowId++;

      for (const auto &obj: value) {
         doSerialize(obj, std::tuple<sqlite3 *, size_t>({std::get<0>(handle), lastRowId}));
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
         "  unsigned_val UNSIGNED BIGINT,"
         "  int_val BIGINT,"
         "  double_val DOUBLE,"
         "  bool_val BOOLEAN,"
         "  string_val TEXT,"
         "  object_val BIGINT"
         ")",
         nullptr,
         nullptr,
         &zErrMsg
      )) {
         throwSqliteError(zErrMsg);
      }

      if (sqlite3_exec(
         db,
         "CREATE TABLE array("
         "  parent BIGINT NOT NULL,"
         "  child BIGINT NOT NULL,"
         "  PRIMARY KEY(parent, child)"
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
