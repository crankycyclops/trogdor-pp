#include "hello.h"

#include <trogdord/request.h>
#include <trogdord/response.h>


// Singleton instance of HelloController
std::unique_ptr<HelloController> HelloController::instance;

/*****************************************************************************/

HelloController::~HelloController() {}

/*****************************************************************************/

HelloController::HelloController() {

	registerAction(Request::GET, DEFAULT_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
		return this->printHello(request);
	});
}

/*****************************************************************************/

std::unique_ptr<HelloController> &HelloController::get() {

	if (!instance) {
		instance = std::unique_ptr<HelloController>(new HelloController());
	}

	return instance;
}

/*****************************************************************************/

const char *HelloController::getName() {

	return SCOPE;
}

/*****************************************************************************/

rapidjson::Document HelloController::printHello(const rapidjson::Document &request) {

    rapidjson::Document response(rapidjson::kObjectType);

    response.AddMember("status", Response::STATUS_SUCCESS, response.GetAllocator());
    response.AddMember("message", "Hello, world!", response.GetAllocator());

    return response;
}
