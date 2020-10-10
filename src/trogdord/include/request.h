#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <rapidjson/document.h>
#include <rapidjson/pointer.h>

#include "response.h"


class Request {

	private:

		static void throwInvalidRequest(std::string errMsg) {

			rapidjson::Document response;

			response.AddMember(
				"status",
				rapidjson::Value().SetInt(Response::STATUS_INVALID),
				response.GetAllocator()
			);

			response.AddMember(
				"message",
				rapidjson::Value().SetString(rapidjson::StringRef(errMsg.c_str())),
				response.GetAllocator()
			);

			throw response;
		}

	public:

		// String representations of the five request methods
		static const char *GET;
		static const char *POST;
		static const char *PUT;
		static const char *SET;
		static const char *DELETE;

		// Error messages for missing or invalid arguments
		static const char *MISSING_GAME_ID;
		static const char *INVALID_GAME_ID;

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

			else if (!value->IsUint()) {
				throwInvalidRequest(invalidErrMsg);
			}

			return value->GetUint();
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
