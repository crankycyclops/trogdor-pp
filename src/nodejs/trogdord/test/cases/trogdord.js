'use strict';

const Trogdord = require('../../lib/trogdord');
const ConnectionRequired = require('./lib/connectionrequired');

class TrogdordTest extends ConnectionRequired {

	/**
	 * Tests Trogdord.constructor().
	 */
	#testConstructor = function () {

		return new Promise((resolve, reject) => {

			// Test with default arguments
			try {

				let connection = new Trogdord();

				connection.on('connect', () => {
					connection.close();
					resolve('Connected');
				});

				connection.on('error', (e) => {
					reject(new Error(e.message));
				});
			} catch (e) {
				reject(e);
			}
		}).then(() => {

			return new Promise((resolve, reject) => {

				// Test with explicit arguments (for now, I'm not testing the third
				// options argument)
				try {

					let connection = new Trogdord('localhost', 1040);

					connection.on('connect', () => {
						connection.close();
						resolve('Connected');
					});

					connection.on('error', (e) => {
						reject(new Error(e.message));
					});
				} catch (e) {
					reject(e);
				}
			});
		});
	}

	/**
	 * Tests Trogdord.connected().
	 */
	#testConnected = function () {

		let connection;

		return new Promise((resolve, reject) => {

			// 1: Connect
			try {

				connection = new Trogdord();

				connection.on('connect', () => {
					resolve('Connected');
				});

				connection.on('error', (e) => {
					reject(new Error(e.message));
				});
			} catch (e) {
				reject(e);
			}
		}).then(() => {

			return new Promise((resolve, reject) => {

				// 2: Test connected() method while connected
				if (connection.connected) {
					resolve();
				} else {
					reject(new Error("connection.connected getter should have returned true after connecting but didn't"));
				}
			});
		}).then(() => {

			return new Promise((resolve, reject) => {

				// 3: Test connected() method after closing the connection
				connection.close();

				if (!connection.connected) {
					resolve();
				} else {
					reject(new Error("connection.connected getter should have returned false immediately after being closed but didn't"));
				}
			});
		});
	}

	/**
	 * Tests Trogdord.statistics().
	 */
	#testStatistics = function () {

		let connection;

		// I only check these so that if I forget to write checks for additional
		// stats that are added later, I'll get a nice obvious warning in the
		// form of a failed test :)
		let validKeys = [
			'lib_version',
			'version',
			'players'
		];

		return new Promise((resolve, reject) => {

			try {

				connection = new Trogdord();

				connection.on('connect', () => {

					connection.statistics().then(response => {

						if (200 != connection.status) {
							reject(new Error("Request should have returned 200 status but did not"));
						}

						if ('object' != typeof response) {
							reject(new Error("Response should be of type 'object' but isn't"));
						}

						let unexpectedKey = null;
						let responseKeys = Object.keys(response);

						for (let i = 0; i < responseKeys.length; i++) {
							if (!validKeys.includes(responseKeys[i])) {
								reject(new Error("Found unexpected key '" + responseKeys[i] + "' in response. Likely, the unit tests need to be updated."));
							}
						}

						if (!Number.isInteger(response.players)) {
							reject(new Error("response.players should be an integer but is not"));
						}

						if (response.players < 0) {
							reject(new Error("response.players should be >= 0 but is not"));
						}

						resolve();
					});
				});

				connection.on('error', (e) => {
					reject(new Error(e.message));
				});
			} catch (e) {
				reject(e);
			}
		});
	}

	/**
	 * Run all unit tests.
	 */
	run() {

		return new Promise((resolve, reject) => {

			return this.#testConstructor().then(
				this.#testConnected
			).then(
				this.#testStatistics
			).then(() => {
				resolve();
			}).catch(error => {
				reject(error);
			});
		});
	}
};

module.exports = TrogdordTest;
