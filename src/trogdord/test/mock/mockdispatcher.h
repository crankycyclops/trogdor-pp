#include <functional>
#include <trogdord/dispatcher.h>

#ifndef MOCK_DISPATCHER_H
#define MOCK_DISPATCHER_H


// Since Dispatcher automatically populates itself with scopes that won't work
// for this test, and since it's a singleton, which isn't ideal, my idea is to
// create a mock class that inherits from it with a public constructor. This
// will allow me to create as many instances as I need and will allow me to
// register only those mock scopes that I need to validate Dispatcher's code.
class MockDispatcher: public Dispatcher {

	public:

		// Constructor takes as input an optional mapping of scopes that
		// should be registered with the mock dispatcher for testing.
		MockDispatcher(std::unordered_map<std::string, ScopeController *> s = {});
};


#endif