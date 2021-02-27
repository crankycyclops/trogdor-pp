'use strict';

// All test cases should inherit from this base class.
class Case {

	// Test cases should override this if there are conditions under which it
	// should be skipped and should return a promise that rejects if the test
	// should be executed and resolves if the test should be skipped.
	skip() {

		return new Promise((resolve, reject) => {
			reject();
		});
	}

	/*************************************************************************/

	// Executes the test case. This method must be implemented for the test
	// case to be valid.
	run() {
		return new Promise((resolve, reject) => {
			reject('run() method must be implemented for test case to be valid.');
		});
	}
};

module.exports = Case;
