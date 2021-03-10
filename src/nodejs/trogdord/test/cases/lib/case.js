'use strict';


/**
 * All test cases should inherit from this base class.
 */
class Case {

	// Array of unit tests to run
	#tests = [];

	/*************************************************************************/

	/**
	 * Initializes the instance of Case. Calls to this.addTest() should go
	 * here.
	 */
	init() {

		return new Promise((resolve, reject) => {
			reject('init() method must be implemented for test case to be valid.');
		});
	}

	/*************************************************************************/

	/**
	 * Test cases should override this if there are conditions under which it
	 * should be skipped and should return a promise that rejects if the test
	 * should be executed and resolves if the test should be skipped.
	 */
	skip() {

		return new Promise((resolve, reject) => {
			reject();
		});
	}

	/*************************************************************************/

	/**
	 * Executes the test case. This method must be implemented for the test
	 * case to be valid.
	 */
	run() {

		return new Promise((resolve, reject) => {

			if (!this.#tests.length) {
				reject('run() method must have at least one test to execute.');
			}

			return this.#tests.reduce((chain, currentTest) => {
				return chain.then(currentTest.test).catch(error => {
					reject({test: currentTest.name, error: error});
				});
			}, Promise.resolve()).then(() => {
				resolve();
			});
		});
	}

	/*************************************************************************/

	/**
	 * Inserts a tests to be performed. At least one test must be inserted for
	 * the instance of Case to be valid at runtime.
	 *
	 * TODO: figure out how I'm going to display the name of each test being
	 * executed so that there's more context.
	 */
	addTest(name, callback) {

		this.#tests.push({name: name, test: callback});
		return this;
	}
};

module.exports = Case;
