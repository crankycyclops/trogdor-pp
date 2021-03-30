#include <fstream>
#include <sstream>

#include <trogdor/filesystem.h>
#include <trogdor/serial/drivers/json.h>
#include <trogdor/serial/serializable.h>
#include <trogdor/exception/undefinedexception.h>
#include <trogdor/exception/fileexception.h>

namespace trogdor::serial {


   void Json::serializeSizeT(std::any data, std::string key, const size_t &value) {

      rapidjson::Value keyVal(rapidjson::kStringType);
      keyVal.SetString(rapidjson::StringRef(key.c_str()), *allocator);

      std::any_cast<std::shared_ptr<rapidjson::Document>>(data)->AddMember(
         keyVal,
         value,
         *allocator
      );
   }

   /************************************************************************/

   void Json::serializeInt(std::any data, std::string key, const int &value) {

      rapidjson::Value keyVal(rapidjson::kStringType);
      keyVal.SetString(rapidjson::StringRef(key.c_str()), *allocator);

      std::any_cast<std::shared_ptr<rapidjson::Document>>(data)->AddMember(
         keyVal,
         value,
         *allocator
      );
   }

   /************************************************************************/

   void Json::serializeDouble(std::any data, std::string key, const double &value) {

      rapidjson::Value keyVal(rapidjson::kStringType);
      keyVal.SetString(rapidjson::StringRef(key.c_str()), *allocator);

      std::any_cast<std::shared_ptr<rapidjson::Document>>(data)->AddMember(
         keyVal,
         value,
         *allocator
      );
   }

   /************************************************************************/

   void Json::serializeBool(std::any data, std::string key, const bool &value) {

      rapidjson::Value keyVal(rapidjson::kStringType);
      keyVal.SetString(rapidjson::StringRef(key.c_str()), *allocator);

      std::any_cast<std::shared_ptr<rapidjson::Document>>(data)->AddMember(
         keyVal,
         value,
         *allocator
      );
   }

   /************************************************************************/

   void Json::serializeString(std::any data, std::string key, const std::string &value) {

      rapidjson::Value keyVal(rapidjson::kStringType);
      keyVal.SetString(rapidjson::StringRef(key.c_str()), *allocator);

      rapidjson::Value strVal(rapidjson::kStringType);
      strVal.SetString(rapidjson::StringRef(value.c_str()), *allocator);

      std::any_cast<std::shared_ptr<rapidjson::Document>>(data)->AddMember(
         keyVal,
         strVal,
         *allocator
      );
   }

   /************************************************************************/

   void Json::serializeSerializable(
      std::any data,
      std::string key,
      const std::shared_ptr<Serializable> &value
   ) {

      rapidjson::Value keyVal(rapidjson::kStringType);
      keyVal.SetString(rapidjson::StringRef(key.c_str()), *allocator);

      std::shared_ptr<rapidjson::Document> obj =
         std::any_cast<std::shared_ptr<rapidjson::Document>>(doSerialize(value));

      std::any_cast<std::shared_ptr<rapidjson::Document>>(data)->AddMember(
         keyVal,
         *obj,
         *allocator
      );
   }

   /************************************************************************/

   void Json::serializeStringVector(
      std::any data,
      std::string key,
      const std::vector<std::string> &value
   ) {

      rapidjson::Value stringArr(rapidjson::kArrayType);
      rapidjson::Value keyVal(rapidjson::kStringType);

      keyVal.SetString(rapidjson::StringRef(key.c_str()), *allocator);

      for (const auto &strVal: value) {

         rapidjson::Value strValJSON(rapidjson::kStringType);
         strValJSON.SetString(rapidjson::StringRef(strVal.c_str()), *allocator);

         stringArr.PushBack(strValJSON, *allocator);
      }

      std::any_cast<std::shared_ptr<rapidjson::Document>>(data)->AddMember(
         keyVal,
         stringArr.Move(),
         *allocator
      );
   }

   /************************************************************************/

   void Json::serializeSerializableVector(
      std::any data,
      std::string key,
      const std::vector<std::shared_ptr<Serializable>> &value
   ) {

      rapidjson::Value objArray(rapidjson::kArrayType);
      rapidjson::Value keyVal(rapidjson::kStringType);

      keyVal.SetString(rapidjson::StringRef(key.c_str()), *allocator);

      for (const auto &obj: value) {
         std::shared_ptr<rapidjson::Document> serializedObj =
            std::any_cast<std::shared_ptr<rapidjson::Document>>(doSerialize(obj));
         objArray.PushBack(*serializedObj, *allocator);
      }

      std::any_cast<std::shared_ptr<rapidjson::Document>>(data)->AddMember(
         keyVal,
         objArray.Move(),
         *allocator
      );
   }

   /************************************************************************/

   std::any Json::initSerializedChild() {

      return std::make_shared<rapidjson::Document>(rapidjson::kObjectType);
   }

   /************************************************************************/

   std::shared_ptr<Serializable> Json::doDeserialize(const rapidjson::Value &jsonObj) {

      std::shared_ptr<Serializable> obj = std::make_shared<Serializable>();

      for (auto it = jsonObj.MemberBegin(); it != jsonObj.MemberEnd(); it++) {

         switch (it->value.GetType()) {

            case rapidjson::kTrueType:
            case rapidjson::kFalseType:

               obj->set(it->name.GetString(), it->value.GetBool());
               break;

            case rapidjson::kNumberType:

               if (it->value.IsDouble()) {
                  obj->set(it->name.GetString(), it->value.GetDouble());
               }

               else if (it->value.IsUint()) {
                  obj->set(it->name.GetString(), static_cast<size_t>(it->value.GetUint()));
               }

               #if SIZE_MAX == UINT64_MAX
               else if (it->value.IsUint64()) {
                  obj->set(it->name.GetString(), static_cast<size_t>(it->value.GetUint64()));
               }
               #endif

               else {
                  obj->set(it->name.GetString(), static_cast<int>(it->value.GetInt()));
               }

               break;

            case rapidjson::kStringType:

               obj->set(it->name.GetString(), it->value.GetString());
               break;

            // Currently, Serializable only supports arrays of all
            // strings and all sub-objects. That makes my code here a
            // lot simpler.
            case rapidjson::kArrayType:

               // If the array is empty, we can treat it as any type,
               // because when we eventually iterate through the
               // resulting std::vector, we won't actually end up
               // having any std::variants to access and therefore
               // won't have to worry about accesing the wrong type.
               if (!it->value.Size()) {
                  obj->set(it->name.GetString(), std::vector<std::string>());
               }

               // Deserializing an array of strings
               else if (it->value.Begin()->IsString()) {

                  std::vector<std::string> strArray;

                  for (auto arrIt = it->value.Begin(); arrIt != it->value.End(); arrIt++) {
                     strArray.push_back(arrIt->GetString());
                  }

                  obj->set(it->name.GetString(), strArray);
               }

               // Deserializing an array of objects
               else if (it->value.Begin()->IsObject()) {

                  std::vector<std::shared_ptr<Serializable>> objArray;

                  for (auto arrIt = it->value.Begin(); arrIt != it->value.End(); arrIt++) {
                     objArray.push_back(doDeserialize(*arrIt));
                  }

                  obj->set(it->name.GetString(), objArray);
               }

               else {
                  throw UndefinedException("Can only deserialize arrays of objects or strings");
               }

               break;

            case rapidjson::kObjectType:

               obj->set(it->name.GetString(), doDeserialize(it->value));
               break;

            case rapidjson::kNullType:
            default:

               throw UndefinedException("Json::deserialize() doesn't support null values");
         }
      }

      return obj;
   }

   /************************************************************************/

   std::any Json::serialize(const std::shared_ptr<Serializable> &data) {

      rapidjson::StringBuffer buffer;
      rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
      std::shared_ptr<rapidjson::Document> object =
         std::make_shared<rapidjson::Document>(rapidjson::kObjectType);

      allocator = &object->GetAllocator();
      doSerialize(data, object);

      object->Accept(writer);
      return std::string(buffer.GetString());
   }

   /************************************************************************/

   std::shared_ptr<Serializable> Json::deserialize(const std::any &data) {


      std::string json = typeid(const char *) == data.type() ?
         std::any_cast<const char *>(data) : std::any_cast<std::string>(data);

      rapidjson::Document jsonObj;
      jsonObj.Parse(json.c_str());

      // TODO: if data is invalid, make sure I properly detect and signal this
      // to the user
      if (rapidjson::kObjectType != jsonObj.GetType()) {
         throw UndefinedException("Json::deserialize() can only handle JSON objects, not arrays or scalars");
      }

      return doDeserialize(jsonObj);
   }

   /************************************************************************/

   std::shared_ptr<Serializable> Json::deserializeFromDisk(const std::string filename) {

      std::ifstream jsonFile(filename);

      if (!jsonFile.is_open()) {

         if (!STD_FILESYSTEM::exists(filename)) {
            throw FileException(filename + " does not exist");
         }

         else if (!STD_FILESYSTEM::is_regular_file(filename)) {
            throw FileException(filename + " is not a file");
         }

         else {
            throw FileException(std::string("Could not open ") + filename);
         }
      }

      std::ostringstream jsonStr;

      jsonStr << jsonFile.rdbuf();
      return deserialize(jsonStr.str());
   }

   /************************************************************************/

   void Json::writeToDisk(std::any data, std::string filename) {

      std::ofstream outputFile(filename);
      std::string json = std::any_cast<std::string>(data);

      if (outputFile.is_open()) {
         outputFile << json;
         outputFile.close();
      } else {
         throw FileException(std::string("Failed to write ") + filename + " to disk");
      }
   }
}
