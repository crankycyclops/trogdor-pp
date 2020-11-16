#ifndef SERIAL_DRIVER_JSON_H
#define SERIAL_DRIVER_JSON_H


#include <trogdor/serial/driver.h>

namespace trogdor::serial {


   // Maps data from an instance of Serializer to JSON.
   class Json: public Driver {

      public:

         /*
            Takes as input a serializable object and outputs the
            serialized representation defined by the implementing class.

            Input:
               Serializable object (const Serializable &)

            Output:
               JSON string (std::any)
         */
         virtual std::any serialize(const std::shared_ptr<Serializable> &data);

         /*
            Takes as input a serialized object and outputs a
            reconstituted instance of Serializable.

            Input:
               JSON string (const std::any &)

            Output:
               Populated instance of Serializable (Serializable)
         */
         virtual std::shared_ptr<Serializable> deserialize(const std::any &data);
   };
}


#endif // SERIAL_DRIVER_JSON_H