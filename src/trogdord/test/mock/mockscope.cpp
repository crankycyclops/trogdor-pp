#include "mockscope.h"


const char *MockScopeController::getName() {

	return SCOPE;
}

/*****************************************************************************/

std::unique_ptr<MockScopeController> MockScopeController::factory() {

	return std::unique_ptr<MockScopeController>(new MockScopeController());
}
