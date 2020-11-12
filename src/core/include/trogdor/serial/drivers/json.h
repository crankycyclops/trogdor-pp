#ifndef SERIAL_JSON_H
#define SERIAL_JSON_H


#include <trogdor/serial/driver.h>

namespace trogdor::serial {


   class Json: public Driver {

      public:

         /*
            Takes as input a serializable object and outputs the
            serialized representation defined by the implementing class.

            Input:
               Serializable object (const Serializable &)

            Output:
               Serialized version (std::any)
         */
         virtual std::any serialize(const Serializable &data);

         /*
            Takes as input a serialized object and outputs a
            reconstituted instance of Serializable.

            Input:
               Serialized data (const std::any &)

            Output:
               Populated instance of Serializable (Serializable)
         */
         virtual Serializable deserialize(const std::any &data);
   };
}


#endif // SERIAL_JSON_H