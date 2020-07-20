#include "mockscope.h"


// Scope name that should be used in requests
const char *MockScopeController::SCOPE = "mock";

/*****************************************************************************/

std::unique_ptr<MockScopeController> MockScopeController::factory() {

	return std::unique_ptr<MockScopeController>(new MockScopeController());
}
