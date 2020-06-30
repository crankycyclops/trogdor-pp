#include "mocktrigger.h"


trogdor::event::EventReturn MockTrigger::operator()(trogdor::event::Event e) {

	executeCallback();
	return {allowAction, continueExecution};
}
