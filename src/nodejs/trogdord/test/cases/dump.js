'use strict';

const ConnectionRequired = require('./lib/connectionrequired');

const Trogdord = require('../../lib/trogdord');

class DumpTest extends ConnectionRequired {

	/**
	 * Tests various getters for the Dump class.
	 */
	#testGetters = function () {

		return new Promise((resolve, reject) => {

            // TODO
            resolve();
		});
	}

	/**
	 * Initialize tests.
	 */
	init() {

		return new Promise((resolve, reject) => {

			this.addTest("Dump Getters for id, name, definition, created, and trogdord", this.#testGetters);
			resolve();
		});
	}
};

module.exports = DumpTest;
