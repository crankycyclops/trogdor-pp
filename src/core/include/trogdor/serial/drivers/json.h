#ifndef SERIAL_DRIVER_JSON_H
#define SERIAL_DRIVER_JSON_H


#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <trogdor/serial/driver.h>

namespace trogdor::serial {


   // Maps data from an instance of Serializer to JSON.
   class Json: public Driver {

      private:

         // The allocator we should use when assigning values to a JSON document
         // or one of its children
         rapidjson::MemoryPoolAllocator<> *allocator = nullptr;

         /*
            Takes a RapidJSON Value (or Document) and returns a Serializable
            object constructed from it.

            Input:
               RapidJSON value (const rapidjson::Value &)

            Output:
               An instance of Serializable
         */
         std::shared_ptr<Serializable> doDeserialize(const rapidjson::Value &jsonObj);

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

         /*
            doSerialize() calls this method to create a child object.

            Input:
               (none)

            Output:
               A newly initialized object or handle (std::any)
         */
         virtual std::any initSerializedChild();

      public:

         // The driver's name
         static constexpr const char *CLASS_NAME = "json";

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
            Utility method that writes the serialized data to disk. Each driver
            should implement this method.

            Input:
               Data to write (std::any wrapping std::string)
               Output filename (std::string)

            Output:
               (none)
         */
        virtual void writeToDisk(std::any data, std::string filename);
   };
}


#endif // SERIAL_DRIVER_JSON_H