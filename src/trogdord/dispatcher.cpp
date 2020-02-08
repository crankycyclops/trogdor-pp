#include <iostream>
#include "include/dispatcher.h"


std::unique_ptr<Dispatcher> Dispatcher::instance = nullptr;

/*****************************************************************************/

Dispatcher::Dispatcher() {

	// TODO
}

/*****************************************************************************/

std::unique_ptr<Dispatcher> &Dispatcher::get() {

	if (!instance) {
		instance = std::unique_ptr<Dispatcher>(new Dispatcher());
	}

	return instance;
}

/*****************************************************************************/

void Dispatcher::dispatch(std::string request) {

	// TODO: actual routing of requests
	std::cout << "Request said: " << request << std::endl;
}
