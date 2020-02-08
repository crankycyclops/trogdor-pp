#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <string>
#include <memory>

class Dispatcher {

	private:

		// Singleton instance of Dispatcher
		static std::unique_ptr<Dispatcher> instance;

		// Constructor should only be called internally by get(), which will
		// ensure we only ever have a single instance of the class.
		Dispatcher();
		Dispatcher(const Dispatcher &) = delete;

	public:

		// Returns singleton instance of Dispatcher.
		static std::unique_ptr<Dispatcher> &get();

		// Dispatches a request.
		void dispatch(std::string request); 
};


#endif
