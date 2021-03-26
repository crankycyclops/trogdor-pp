#ifndef SERIAL_DRIVER_SQLITE_H
#define SERIAL_DRIVER_SQLITE_H


#include <sqlite3.h>
#include <trogdor/serial/driver.h>

namespace trogdor::serial {


   // Maps data from an instance of Serializer to a SQLite3 database.
   class Sqlite: public Driver {

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
            Copies the contents of one SQLite3 database into another. Use this to
            write the output of serialize() to disk.

            Input:
               SQLite3 database object (sqlite3 *)
               Output filename (std::string)

            Output:
               (none)
         */
        void writeToDisk(sqlite3 *data, std::string filename);
   };
}


#endif // SERIAL_DRIVER_SQLITE_H