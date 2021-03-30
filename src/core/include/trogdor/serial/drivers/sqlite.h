#ifndef SERIAL_DRIVER_SQLITE_H
#define SERIAL_DRIVER_SQLITE_H


#include <sqlite3.h>
#include <trogdor/serial/driver.h>
#include <trogdor/exception/exception.h>

namespace trogdor::serial {


   // Maps data from an instance of Serializer to a SQLite3 database.
   class Sqlite: public Driver {

      private:

         #if SIZE_MAX == UINT64_MAX
            typedef int64_t IntType;
         #else
            typedef int32_t IntType;
         #endif

         // Tracks the previous rowid for the data table before an insert
         // was performed.
         size_t lastRowId = 0;

         /*
            Creates the schema used to store serialized data.

            Input:
               Database handle (sqlite3 *)

            Output:
               (none)
         */
         void createSchema(sqlite3 *db);

         /*
            Helper method that selects all child rows of a parent (if parent
            is 0, we get all root rows where parent is null.)

            Input:
               Database handle (sqlite3 *)
               Rowid of parent (size_t)

            Output:
               Prepared Statement (sqlite3_stmt *)
         */
         sqlite3_stmt *prepareSelectByParent(sqlite3 *db, size_t parent = 0);

         /*
            Throws a SQLite3 error message, freeing the C-string in the process.

            Input:
               Error message, allocated by SQLite3 (char *)

            Output:
               (none)
         */
         inline void throwSqliteError(char *error) {

            std::string errorStr(error);

            sqlite3_free(error);
            throw Exception(errorStr);
         }

         /*
            Inserts an integer value into the data table.

            Input:
               Database handle (sqlite3 *)
               Id of parent row (size_t)
               Value (int32_t or int64_t, depending on the architecture)
               Key (std::string)

            Output:
               (none)
         */
         inline void insertInt(sqlite3 *db, size_t parent, IntType value, std::string key = "") {

            _insertInt(db, parent, value, 'i', key);
         }

         /*
            Inserts an unsigned integer value into the data table.

            Input:
               Database handle (sqlite3 *)
               Id of parent row (size_t)
               Value (size_t)
               Key (std::string)

            Output:
               (none)
         */
         inline void insertUnsigned(sqlite3 *db, size_t parent, size_t value, std::string key = "") {

            _insertInt(db, parent, value, 'u', key);
         }

         /*
            Inserts a boolean value into the data table.

            Input:
               Database handle (sqlite3 *)
               Id of parent row (size_t)
               Value (bool)
               Key (std::string)

            Output:
               (none)
         */
         inline void insertBool(sqlite3 *db, size_t parent, bool value, std::string key = "") {

            _insertInt(db, parent, value, 'b', key);
         }

         /*
            Inserts any value type that ultimately gets stored as an integer
            in SQLite3. This includes booleans, unsigned, and ordinary signed
            ints.

            Input:
               Database handle (sqlite3 *)
               Id of parent row (size_t)
               Value (int32_t or int64_t, depending on the architecture)
               Type (one of 'b' for boolean, 'i' for integer, or 'u' for unsigned)
               Key (std::string)

            Output:
               (none)
         */
         void _insertInt(sqlite3 *db, size_t parent, IntType value, char type, std::string key = "");

         /*
            Inserts a double precision value into the data table.

            Input:
               Database handle (sqlite3 *)
               Id of parent row (size_t)
               Value (double)
               Key (std::string)

            Output:
               (none)
         */
         void insertDouble(sqlite3 *db, size_t parent, double value, std::string key = "");

         /*
            Inserts a string value into the data table.

            Input:
               Database handle (sqlite3 *)
               Id of parent row (size_t)
               Value (std::string)
               Key (std::string)

            Output:
               (none)
         */
         void insertString(sqlite3 *db, size_t parent, std::string value, std::string key = "");

         /*
            Inserts an object value into the data table.

            Input:
               Database handle (sqlite3 *)
               Id of parent row (size_t)
               Key (std::string)

            Output:
               (none)
         */
         void insertObjectValue(sqlite3 *db, size_t parent, std::string key = "");

         /*
            Inserts an array value into the data table.

            Input:
               Database handle (sqlite3 *)
               Id of parent row (size_t)
               Key (std::string)

            Output:
               (none)
         */
         void insertArrayValue(sqlite3 *db, size_t parent, std::string key = "");

         /*
            Recursively deserializes game data stored in a SQLite3 database.

            Input:
               Database handle (sqlite3 *)
               Id of parent row (size_t)

            Output:
               (none)
         */
         std::shared_ptr<Serializable> doDeserialize(sqlite3 *db, size_t parent = 0);

      protected:

         /*
            These methods are called by doSerialize() and convert each type of
            value to its resulting representation on disk. They take as input
            a parent object of any type (should only be a raw or smart pointer
            to the underlying data structure) where the key/value pair should
            be set, the key (always a string) and the value.

            Input:
               Pointer to the parent data structure (std::any)
               Key (std::string)
               Value (varied)

            Output:
               (none)
         */
         virtual void serializeSizeT(std::any data, std::string key, const size_t &value);
         virtual void serializeInt(std::any data, std::string key, int const &value);
         virtual void serializeDouble(std::any data, std::string key, const double &value);
         virtual void serializeBool(std::any data, std::string key, const bool &value);
         virtual void serializeString(std::any data, std::string key, const std::string &value);
         virtual void serializeSerializable(std::any data, std::string key, const std::shared_ptr<Serializable> &value);
         virtual void serializeStringVector(std::any data, std::string key, const std::vector<std::string> &value);
         virtual void serializeSerializableVector(std::any data, std::string key, const std::vector<std::shared_ptr<Serializable>> &value);

      public:

         // The driver's name
         static constexpr const char *CLASS_NAME = "sqlite";

         /*
            Takes as input a serializable object and outputs a pointer to
            an in-memory database where the data was written.

            IMPORTANT: make *sure* to call sqlite3_close() on the returned
            value when you're done with it. Otherwise, you'll leak memory.

            Input:
               Serializable object (const Serializable &)

            Output:
               Pointer to in-memory database (sqlite3 * returned as std::any)
         */
         virtual std::any serialize(const std::shared_ptr<Serializable> &data);

         /*
            Takes as input the filename of a SQLite3 database where the data
            is stored and outputs a reconstituted instance of Serializable.

            Input:
               SQLite3 database filename (const std::any &)

            Output:
               Populated instance of Serializable (Serializable)
         */
         virtual std::shared_ptr<Serializable> deserialize(const std::any &data);

         /*
            Takes as input a filename and calls deserialize() on the data that
            file contains.

            Input:
               Filename (const std::string)

            Output:
               Populated instance of Serializable (std::shared_ptr<Serializable>)
         */
         virtual std::shared_ptr<Serializable> deserializeFromDisk(const std::string filename);

         /*
            Copies the contents of one SQLite3 database into another. Use this to
            write the output of serialize() to disk.

            Input:
               SQLite3 database object (std::any wrapping sqlite3 *)
               Output filename (std::string)

            Output:
               (none)
         */
        virtual void writeToDisk(std::any data, std::string filename);
   };
}


#endif // SERIAL_DRIVER_SQLITE_H