#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <rapidjson/document.h>
#include <rapidjson/pointer.h>

#include "response.h"


class Request {

	private:

		static void throwInvalidRequest(std::string errMsg) {

			rapidjson::Document response(rapidjson::kObjectType);

			response.AddMember(
				"status",
				rapidjson::Value().SetInt(Response::STATUS_INVALID),
				response.GetAllocator()
			);

			// For some reason, I have to go through this extra step when
			// std::string is involved. Annoying.
			rapidjson::Value messageVal(rapidjson::kStringType);
			messageVal.SetString(rapidjson::StringRef(errMsg.c_str()), response.GetAllocator());
			response.AddMember("message", messageVal.Move(), response.GetAllocator());

			throw response;
		}

	public:

		// String representations of the five request methods
		static constexpr const char *GET = "get";
		static constexpr const char *POST = "post";
		static constexpr const char *PUT = "put";
		static constexpr const char *SET = "set";
		static constexpr const char *DELETE = "delete";

		// Error messages for missing or invalid arguments
		static constexpr const char *MISSING_GAME_ID = "missing required game id";
		static constexpr const char *INVALID_GAME_ID = "invalid game id";

		// Parses an integer argument.
		static int parseArgumentInt(
			const rapidjson::Document &request,
			std::string argument,
			std::string missingErrMsg,
			std::string invalidErrMsg
		) {

			const rapidjson::Value *value = rapidjson::Pointer(argument.c_str()).Get(request);

			if (!value) {
				throwInvalidRequest(missingErrMsg);
			}

			else if (!value->IsInt()) {
				throwInvalidRequest(invalidErrMsg);
			}

			return value->GetInt();
		}

		// Parses an unsigned integer argument.
		static int parseArgumentUInt(
			const rapidjson::Document &request,
			std::string argument,
			std::string missingErrMsg,
			std::string invalidErrMsg
		) {

			const rapidjson::Value *value = rapidjson::Pointer(argument.c_str()).Get(request);

			if (!value) {
				throwInvalidRequest(missingErrMsg);
			}

			#if SIZE_MAX == UINT64_MAX
				else if (!value->IsUint64()) {
			#else
				else if (!value->IsUint()) {
			#endif
				throwInvalidRequest(invalidErrMsg);
			}

			#if SIZE_MAX == UINT64_MAX
				return value->GetUint64();
			#else
				return value->GetUint();
			#endif
		}

		// Parses a double argument.
		static double parseArgumentDouble(
			const rapidjson::Document &request,
			std::string argument,
			std::string missingErrMsg,
			std::string invalidErrMsg
		) {

			const rapidjson::Value *value = rapidjson::Pointer(argument.c_str()).Get(request);

			if (!value) {
				throwInvalidRequest(missingErrMsg);
			}

			else if (!value->IsNumber()) {
				throwInvalidRequest(invalidErrMsg);
			}

			return value->GetDouble();
		}

		// Parses a bool argument.
		static bool parseArgumentBool(
			const rapidjson::Document &request,
			std::string argument,
			std::string missingErrMsg,
			std::string invalidErrMsg
		) {

			const rapidjson::Value *value = rapidjson::Pointer(argument.c_str()).Get(request);

			if (!value) {
				throwInvalidRequest(missingErrMsg);
			}

			else if (!value->IsBool()) {
				throwInvalidRequest(invalidErrMsg);
			}

			return value->GetBool();
		}

		// Parses a string argument.
		static std::string parseArgumentString(
			const rapidjson::Document &request,
			std::string argument,
			std::string missingErrMsg,
			std::string invalidErrMsg
		) {

			const rapidjson::Value *value = rapidjson::Pointer(argument.c_str()).Get(request);

			if (!value) {
				throwInvalidRequest(missingErrMsg);
			}

			else if (!value->IsString()) {
				throwInvalidRequest(invalidErrMsg);
			}

			return value->GetString();
		}

		// Utility function that parses a game id from a request's arguments.
		static size_t parseGameId(
			const rapidjson::Document &request,
			std::string idField = "/args/id"
		) {

			return parseArgumentUInt(request, idField, MISSING_GAME_ID, INVALID_GAME_ID);
		}
};


#endif
