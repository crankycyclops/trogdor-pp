#include <trogdord/request.h>
#include <trogdord/response.h>
#include <trogdord/scopes/controller.h>


/*
    The test scope implemented here is adapted from the "Hello, World" template
    found in sample/extensions/hello.
*/
class HelloController: public ScopeController {

	private:

		// Singleton instance of HelloController
		static std::unique_ptr<HelloController> instance;

	protected:

		HelloController();
		HelloController(const HelloController &) = delete;

	public:

		// Scope name that should be used in requests
		static constexpr const char *SCOPE = "hello";

		virtual ~HelloController();

		// Returns singleton instance of HelloController.
		static std::unique_ptr<HelloController> &get();

		// Returns the controller's name. This is a virtual method defined by
		// ScopeController. It must be implemented and must return the name
		// used in requests.
		virtual const char *getName();

		// Returns a response containing the string "Hello World"
		rapidjson::Document printHello(const rapidjson::Document &request);
};

/*****************************************************************************/
/*****************************************************************************/

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

/*****************************************************************************/
/*****************************************************************************/

extern "C" {

	ScopeController **loadScopes() {

		static ScopeController *controllers[] = {
			HelloController::get().get(),
			nullptr
		};

		return controllers;
	}
}
