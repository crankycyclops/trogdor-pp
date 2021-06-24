#ifndef JSON_H
#define JSON_H

#include <string>
#include <optional>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>


class JSON {

	public:

		// Convert a RapidJSON Document or Value to a string
		static std::string serialize(const rapidjson::Value &obj) {

			rapidjson::StringBuffer buffer;
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

			obj.Accept(writer);
			return buffer.GetString();
		}

		// Converts a RapidJSON value to a string representation (this method
		// will refuse to convert kObjectType and kArrayType and will return
		// std::nullopt to signal an error if such a value is passed.)
		static std::optional<std::string> valueToStr(const rapidjson::Value &v) {

			switch (v.GetType()) {

				case rapidjson::kNullType:

					return "null";

				case rapidjson::kTrueType:

					return "true";

				case rapidjson::kFalseType:

					return "false";

				case rapidjson::kStringType:

					return v.GetString();

				case rapidjson::kNumberType:

					if (v.IsDouble()) {
						return std::to_string(v.GetDouble());
					}

					else if (v.IsUint()) {
						return std::to_string(v.GetUint());
					}

					else {
						return std::to_string(v.GetInt());
					}

				default:

					return std::nullopt;
			}
		}
};


#endif
