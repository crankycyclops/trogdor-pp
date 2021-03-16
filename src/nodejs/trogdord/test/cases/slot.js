'use strict';

const Trogdord = require('../../lib/trogdord');
const StateRequired = require('./lib/staterequired');

class SlotTest extends StateRequired {

	/**
	 * Tests Slot.destroy()
	 */
	 #testDestroy = function () {

		return new Promise((resolve, reject) => {

            // TODO
            resolve();
		});
	}

	/**
	 * Tests Slot.restore()
	 */
	 #testRestore = function () {

		return new Promise((resolve, reject) => {

            // TODO
            resolve();
		});
	}

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

			this.addTest("Slot.destroy()", this.#testDestroy);
			this.addTest("Slot.restore()", this.#testRestore);
			this.addTest("Slot Getters for slot, timestamp, and dump", this.#testGetters);
			resolve();
		});
	}
};

module.exports = SlotTest;
