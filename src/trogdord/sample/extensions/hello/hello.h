#ifndef SAMPLE_H
#define SAMPLE_H

// Defines ScopeController
#include <trogdord/scopes/controller.h>


/*
	This is an example scope that can be invoked by sending the following
	request to trogdord:

	{"method":"get","scope":"hello"}

	The following will be trogdord's reply:

	{"status":200,"message":"Hello, world!"}

	The singleton pattern used by this example is the same style used by other
	code in trogdord, but is not required for other extensions. However memory
	is allocated, though, care must be taken to ensure that instances remain
	valid for the lifetime of the extension.
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


#endif
