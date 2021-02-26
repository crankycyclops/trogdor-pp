'use strict';

// All test cases should inherit from this base class.
class Case {

	// Test cases should override this if there are conditions under which it
	// should be skipped.
	skip() {

		return false;
	}

	/*************************************************************************/

	// Executes the test case. This method must be implemented for the test
	// case to be valid.
	run() {
		throw new Error('run() method must be implemented for test case to be valid.');
	}
};

module.exports = Case;
