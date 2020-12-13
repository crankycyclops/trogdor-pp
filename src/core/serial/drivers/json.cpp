#include <trogdor/serial/drivers/json.h>
#include <trogdor/serial/serializable.h>
#include <trogdor/exception/undefinedexception.h>

namespace trogdor::serial {


	std::shared_ptr<rapidjson::Document> Json::doSerialize(
		rapidjson::MemoryPoolAllocator<> &allocator,
		const std::shared_ptr<Serializable> &data,
		std::shared_ptr<rapidjson::Document> document
	) {

		if (!document) {
			document = std::make_shared<rapidjson::Document>(rapidjson::kObjectType);
		}

		for (const auto &value: data->getAll()) {

			std::visit([&](auto &&arg) {

				using T = std::decay_t<decltype(arg)>;

				if constexpr (
					std::is_same_v<T, size_t> ||
					std::is_same_v<T, int> ||
					std::is_same_v<T, double> ||
					std::is_same_v<T, bool>
				) {
					document->AddMember(
						rapidjson::StringRef(value.first.c_str()),
						arg,
						allocator
					);
				}

				else if constexpr (std::is_same_v<T, std::string>){
					document->AddMember(
						rapidjson::StringRef(value.first.c_str()),
						rapidjson::StringRef(arg.c_str()),
						allocator
					);
				}

				else if constexpr (std::is_same_v<T, std::shared_ptr<Serializable>>) {

					std::shared_ptr<rapidjson::Document> obj = doSerialize(allocator, arg);

					document->AddMember(
						rapidjson::StringRef(value.first.c_str()),
						*obj,
						allocator
					);
				}

				else if constexpr (std::is_same_v<T, std::vector<std::string>>) {

					rapidjson::Value stringArr(rapidjson::kArrayType);

					for (const auto &strVal: arg) {
						stringArr.PushBack(rapidjson::StringRef(strVal.c_str()), allocator);
					}

					document->AddMember(
						rapidjson::StringRef(value.first.c_str()),
						stringArr,
						allocator
					);
				}

				else if constexpr (std::is_same_v<T, std::vector<std::shared_ptr<Serializable>>>) {

					rapidjson::Value objArray(rapidjson::kArrayType);

					for (const auto &obj: arg) {
						std::shared_ptr<rapidjson::Document> serializedObj = doSerialize(allocator, obj);
						objArray.PushBack(*serializedObj, allocator);
					}

					document->AddMember(
						rapidjson::StringRef(value.first.c_str()),
						objArray,
						allocator
					);
				}

				else {
					throw UndefinedException("Invalid type encountered in instance of serial::Serializable");
				}
			}, value.second);
		}

		return document;
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
		std::shared_ptr<rapidjson::Document> obj = doSerialize(data);

		obj->Accept(writer);
		return std::string(buffer.GetString());
	}

	/************************************************************************/

	std::shared_ptr<Serializable> Json::deserialize(const std::any &data) {


		std::string json = typeid(const char *) == data.type() ?
			std::any_cast<const char *>(data) : std::any_cast<std::string>(data);

		rapidjson::Document jsonObj;
		jsonObj.Parse(json.c_str());

		if (rapidjson::kObjectType != jsonObj.GetType()) {
			throw UndefinedException("Json::deserialize() can only handle JSON objects, not arrays or scalars");
		}

		return doDeserialize(jsonObj);
	}
}
