'use strict';

const ConnectionRequired = require('./lib/connectionrequired');

const Trogdord = require('../../lib/trogdord');

class SlotTest extends ConnectionRequired {

	/**
	 * Tests various getters for the Slot class.
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

			this.addTest("Slot Getters for slot, timestamp, and dump", this.#testGetters);
			resolve();
		});
	}
};

module.exports = SlotTest;
