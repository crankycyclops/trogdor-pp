'use strict';

const Trogdord = require('../../lib/trogdord');
const StateRequired = require('./lib/staterequired');

class SlotTest extends StateRequired {

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
