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

         /*
            A recursive method that does the actual work of serializing an
            instance of Serializable.

            Input:
               Allocator belonging to the root RapidJSON Document
               Serializable data
               The root document, or nullptr if creating a subdocument

            Output:
               A RapidJSON Document
         */
         std::shared_ptr<rapidjson::Document> doSerialize(
            rapidjson::MemoryPoolAllocator<> &allocator,
            const std::shared_ptr<Serializable> &data,
            std::shared_ptr<rapidjson::Document> document = nullptr
         );

         /*
            Takes a RapidJSON Value (or Document) and returns a Serializable
            object constructed from it.

            Input:
               RapidJSON value (const rapidjson::Value &)

            Output:
               An instance of Serializable
         */
         std::shared_ptr<Serializable> doDeserialize(const rapidjson::Value &jsonObj);

         /*
            Our entrypoint into the actual doSerialize() method that first
            initializes the root RapidJSON document.

            Input:
               Serializable data

            Output:
               A RapidJSON Document
         */
         inline std::shared_ptr<rapidjson::Document> doSerialize(
            const std::shared_ptr<Serializable> &data
         ) {

            std::shared_ptr<rapidjson::Document> object =
               std::make_shared<rapidjson::Document>(rapidjson::kObjectType);
            return doSerialize(object->GetAllocator(), data, object);
         }

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