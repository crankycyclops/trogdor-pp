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
	 * Tests Trogdord.connected getter.
	 */
	#testConnectedGetter = function () {

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
	 * Tests the Trogdord.status getter.
	 */
	#testStatusGetter = function () {

		let connection;

		return new Promise((resolve, reject) => {

			try {

				connection = new Trogdord();

				connection.on('connect', () => {

					// The value of status should be initialized to zero before
					// the first request has been made
					if (0 != connection.status) {
						reject(new Error("Status getter should return 0 before the first request has been made"));
					}

					// Should get 200 status after a successful call
					connection.statistics().then(response => {

						if (200 != connection.status) {
							reject(new Error(
								"Expected 200 status after successful call to connection.status, but got " +
								connection.status + " instead"
							));
						}

						resolve();

					}).catch(error => {
						reject (new Error("Expected call to connection.status() to succeed: " + error.message));
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

						let responseKeys = Object.keys(response);
						let keysFound = [];

						for (let i = 0; i < responseKeys.length; i++) {
							if (!validKeys.includes(responseKeys[i])) {
								reject(new Error("Found unexpected key '" + responseKeys[i] + "' in response. Likely, the unit tests need to be updated."));
							} else {
								keysFound.push(responseKeys[i]);
							}
						}

						let difference = validKeys.filter(value => !responseKeys.includes(value));

						if (difference.length) {
							reject(new Error("Response is missing required keys: " + difference.toString()));
						}

						if (!Number.isInteger(response.players)) {
							reject(new Error("response.players should be an integer but isn't"));
						}

						if (response.players < 0) {
							reject(new Error("response.players should be >= 0 but isn't"));
						}

						['version', 'lib_version'].forEach(key => {

							if ('object' != typeof response[key]) {
								reject(new Error("response." + key + " should be an object but isn't"));
							}

							['major', 'minor', 'patch'].forEach(verKey => {
								if (!verKey in response[key]) {
									reject(new Error("Missing required '" + verKey + "' in response." + key));
								} else if (!Number.isInteger(response[key][verKey])) {
									reject(new Error("response." + key + "." + verKey + " should be an integer but isn't"));
								}
							});
						});

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
	 * Tests Trogdord.makeRequest().
	 */
	#testMakeRequest = function () {

		let connection;

		return new Promise((resolve, reject) => {

			connection = new Trogdord();

			connection.on('connect', () => {

				// Attempt valid request without timeout
				connection.makeRequest({method: 'get', scope: 'global', action: 'statistics'}).catch(error => {

					reject(new Error(error));
				}).then(response => {

					if (200 != connection.status) {
						reject(new Error('Status of valid request should be 200'));
					}
				}).then(() => {

					// Attempt valid request with timeout
					return connection.makeRequest({method: 'get', scope: 'global', action: 'statistics'}, 500);
				}).catch(error => {

					reject(new Error(error));
				}).then(response => {

					if (200 != connection.status) {
						reject(new Error('Status of valid request with timeout should be 200'));
					}

					// Attempt unroutable request without timeout
					return connection.makeRequest({method: 'notamethod', scope: 'notascope'});
				}).catch(error => {

					reject(new Error(error));
				}).then(response => {

					if (404 != connection.status) {
						reject(new Error('Status of unroutable request without should have been 404'));
					}

					// Attempt unroutable request with timeout
					return connection.makeRequest({method: 'notamethod', scope: 'notascope'}, 500);
				}).catch(error => {

					reject(new Error(error));
				}).then(response => {

					if (404 != connection.status) {
						reject(new Error('Status of unroutable request with timeout should have been 404'));
					}

					// Attempt request after connection has been closed without timeout
					connection.close();
					return connection.makeRequest({method: 'get', scope: 'global', action: 'statistics'});
				}).then(response => {

					reject(new Error('Request without timeout should not have been successful after closing the connection'));
				}).catch(error => {

					// Attempt request after connection has been closed with timeout
					return connection.makeRequest({method: 'get', scope: 'global', action: 'statistics'}, 500);
				}).then(response => {

					reject(new Error('Request with timeout should not have been successful after closing the connection'));
				}).catch(error => {

					resolve();
				});
			});

			connection.on('error', (e) => {
				reject(new Error(e.message));
			});
		});
	}

	/**
	 * Run all unit tests.
	 */
	run() {

		return new Promise((resolve, reject) => {

			return this.#testConstructor().then(
				this.#testConnectedGetter
			).then(
				this.#testStatusGetter
			).then(
				this.#testStatistics
			).then(
				this.#testMakeRequest
			).then(() => {
				resolve();
			}).catch(error => {
				reject(error);
			});
		});
	}
};

module.exports = TrogdordTest;
