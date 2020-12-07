#include "mocktimerjob.h"


const char *MockTimerJob::getClassName() {

	return CLASS_NAME;
}

/*****************************************************************************/

void MockTimerJob::execute() {

	executeCallback();
}
