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
					document->AddMember(rapidjson::StringRef(value.first.c_str()), arg, allocator);
				}

				else if constexpr (std::is_same_v<T, std::string>){
					document->AddMember(
						rapidjson::StringRef(value.first.c_str()),
						rapidjson::StringRef(arg.c_str()),
						allocator
					);
				}

				else if constexpr (std::is_same_v<T, std::shared_ptr<Serializable>>) {
					// TODO
				}

				else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
					for (const auto &strVal: arg) {
						// TODO
					}
				}

				else if constexpr (std::is_same_v<T, std::vector<std::shared_ptr<Serializable>>>) {
					for (const auto &strVal: arg) {
						// TODO
					}
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
