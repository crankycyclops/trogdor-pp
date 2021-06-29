#include "hello.h"


/*
	All exported symbols should be declared as extern "C" to prevent their names
	from being mangled.
*/
extern "C" {

	/*
		The loadScopes() function returns an array of pointers to instances of one
		or more scopes implemented by the extension. This method is optional and
		can be omitted if there are no scopes to export.

		In the HelloController scope defined in hello.cpp, we use a unique_ptr
		singleton, but you can manage your memory however you'd like as long as
		loadScopes() can return a pointer to an instance that remains valid for
		the lifetime of the extension. You should, furthermore, ensure that the
		memory will be freed when the extension is unloaded (this may or may not
		occur when trogdord is terminated.)

		The array of pointers must be null-terminated.
	*/
	ScopeController **loadScopes() {

		static ScopeController *controllers[] = {
			HelloController::get().get(),
			nullptr
		};

		return controllers;
	}
}