#include <trogdord/scopes/controller.h>


// This class defines a scope that conflicts with the built-in global scope.
class GlobalController: public ScopeController {

	private:

		// Singleton instance of GlobalController
		static std::unique_ptr<GlobalController> instance;

	protected:

		GlobalController();
		GlobalController(const GlobalController &) = delete;

	public:

		// Scope name that should be used in requests
		static constexpr const char *SCOPE = "global";

		virtual ~GlobalController();

		// Returns singleton instance of GlobalController.
		static std::unique_ptr<GlobalController> &get();

		// Returns the controller's name. This is a virtual method defined by
		// ScopeController. It must be implemented and must return the name
		// used in requests.
		virtual const char *getName();
};

/*****************************************************************************/
/*****************************************************************************/

std::unique_ptr<GlobalController> GlobalController::instance;

/*****************************************************************************/

GlobalController::~GlobalController() {}

/*****************************************************************************/

GlobalController::GlobalController() {}

/*****************************************************************************/

std::unique_ptr<GlobalController> &GlobalController::get() {

	if (!instance) {
		instance = std::unique_ptr<GlobalController>(new GlobalController());
	}

	return instance;
}

/*****************************************************************************/

const char *GlobalController::getName() {

	return SCOPE;
}

/*****************************************************************************/
/*****************************************************************************/

extern "C" {

	ScopeController **loadScopes() {

		static ScopeController *controllers[] = {
			GlobalController::get().get(),
			nullptr
		};

		return controllers;
	}
}
