#include <trogdor/serial/serializable.h>
#include <trogdor/exception/undefinedexception.h>

#include "../include/serial/json.h"

namespace trogdor::serial {


	std::shared_ptr<rapidjson::Document> Json::doSerialize(
		rapidjson::MemoryPoolAllocator<> &allocator,
		const std::shared_ptr<Serializable> &data,
		std::shared_ptr<rapidjson::Document> document
	) {

		if (!document) {
			document = std::make_shared<rapidjson::Document>();
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

	std::any Json::serialize(const std::shared_ptr<Serializable> &data) {

		return doSerialize(data);
	}

	/************************************************************************/

	std::shared_ptr<Serializable> Json::deserialize(const std::any &data) {

		std::shared_ptr<Serializable> obj = std::make_shared<Serializable>();

		// TODO
		return obj;
	}
}
