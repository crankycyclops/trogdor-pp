#ifndef SERIAL_DRIVER_SQLITE_H
#define SERIAL_DRIVER_SQLITE_H


// TODO: include sqlite libs

#include <trogdor/serial/driver.h>

namespace trogdor::serial {


   // Maps data from an instance of Serializer to a SQLite3 database.
   class Sqlite: public Driver {

      public:

         // The driver's name
         static constexpr const char *CLASS_NAME = "sqlite";

         /*
            Takes as input a serializable object and outputs the
            filename of a SQLite3 database where the data was written.

            Input:
               Serializable object (const Serializable &)

            Output:
               std::string (std::any)
         */
         virtual std::any serialize(const std::shared_ptr<Serializable> &data);

         /*
            Takes as input the filename of a SQLite3 database where the data
            is stored and outputs a reconstituted instance of Serializable.

            Input:
               SQLite3 database filename as std::string (const std::any &)

            Output:
               Populated instance of Serializable (Serializable)
         */
         virtual std::shared_ptr<Serializable> deserialize(const std::any &data);
   };
}


#endif // SERIAL_DRIVER_SQLITE_H