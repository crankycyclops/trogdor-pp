'use strict';

const ConnectionRequired = require('./lib/connectionrequired');

class TrogdordTest extends ConnectionRequired {

	/**
	 * Tests Trogdord.prototype.constructor.
	 */
	#testConstructor = function () {

		return new Promise((resolve, reject) => {
			resolve();
		});

		// When rejecting, send instance of Error, like so:
		// reject(new Error("Wee"));
	}

	/**
	 * Tests Trogdord.prototype.connected.
	 */
	#testConnected = function () {

		return new Promise((resolve, reject) => {
			console.log("testConnected: TODO");
			resolve();
		});
	}

	/**
	 * Run all unit tests.
	 */
	run() {

		return this.#testConstructor().then(
			this.#testConnected()
		);
	}
};

module.exports = TrogdordTest;
