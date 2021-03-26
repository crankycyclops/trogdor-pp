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

      protected:

         /*
            These methods are called by doSerialize() and convert each type of
            value to its resulting representation on disk. They take as input
            a parent object of any type (should only be a raw or smart pointer
            to the underlying data structure) where the key/value pair should
            be set, the key (always a string) and the value.

            If for some reason you're writing a serialization driver that does
            not use doSerialize() (probably a bad idea, but hey, it's your
            driver and I'm sure you have a good reason), you don't have to
            implement this.

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

         /*
            When doSerialize() needs to create a child object, it calls this
            method. If you don't plan to use doSerialize() in your own driver,
            or if you're always going to pass in a non-empty value as the
            second argument to doSerialize(), you don't have to implement this.

            Input:
               (none)

            Output:
               A newly initialized object or handle (std::any)
         */
         virtual std::any initSerializedChild();

         /*
            A recursive method that does the actual work of serializing an
            instance of Serializable. To use this helper method, you must
            implement the optional virtual methods above.

            Input:
               Serializable data
               Parent output object or no value if creating a child

            Output:
               Varied (std::any)
         */
         std::any doSerialize(
            const std::shared_ptr<Serializable> &data,
            std::any output = {}
         );

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

         /*
            Utility method that writes the serialized data to disk. Each driver
            should implement this method.

            Input:
               Data to write (std::any)
               Output filename (std::string)

            Output:
               (none)
         */
        virtual void writeToDisk(std::any data, std::string filename) = 0;
   };
}


#endif // SERIAL_DRIVER_H