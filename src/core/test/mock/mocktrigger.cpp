#include "mocktrigger.h"


trogdor::event::EventReturn MockTrigger::operator()(trogdor::event::Event e) {

	executeCallback();
	return {allowAction, continueExecution};
}

/************************************************************************/

const char *MockTrigger::getClassName() {

	return CLASS_NAME;
}

/************************************************************************/

// I haven't actually implemented this yet because it's not necessary. I
// also haven't registered this type with a call to
// EventTrigger::RegisterType(). I'll do that if and when it becomes
// necessary for the unit tests to pass.
std::shared_ptr<trogdor::serial::Serializable> MockTrigger::serialize() {

	std::shared_ptr<trogdor::serial::Serializable> data;
	return data;
}
