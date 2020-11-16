#ifndef SERIAL_DRIVER_H
#define SERIAL_DRIVER_H


#include <any>
#include <optional>
#include <variant>
#include <string>
#include <memory>
#include <unordered_map>

#include "data.h"

namespace trogdor::serial {


   // Maps data from an instance of Serializer to a format defined by an
   // implementing class.
   class Driver {

      public:

         // Destructor
         virtual ~Driver();

         /*
            Takes as input a serializable object and outputs the
            serialized representation defined by the implementing class.

            Input:
               Serializable object (const Serializable &)

            Output:
               Serialized version (std::any)
         */
         virtual std::any serialize(const std::shared_ptr<Serializable> &data) = 0;

         /*
            Takes as input a serialized object and outputs a
            reconstituted instance of Serializable.

            Input:
               Serialized data (const std::any &)

            Output:
               Populated instance of Serializable (Serializable)
         */
         virtual std::shared_ptr<Serializable> deserialize(const std::any &data) = 0;
   };
}


#endif // SERIAL_DRIVER_H