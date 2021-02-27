'use strict';

const Trogdord = require('../../lib/trogdord');
const ConnectionRequired = require('./lib/connectionrequired');

class TrogdordTest extends ConnectionRequired {

	/**
	 * Tests Trogdord.prototype.constructor.
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
	}

	/**
	 * Tests Trogdord.prototype.connected.
	 */
	#testConnected = function () {

		return new Promise((resolve, reject) => {

			// 1: Connect
			try {

				let connection = new Trogdord();

				connection.on('connect', () => {
					connection.close();
					resolve('Connected');
				});

				connection.on('error', (e) => {
					reject(new Error(e.message));
				});

				return connection;
			} catch (e) {
				reject(e);
			}
		}).then(connection => {

			// 2: Test connected() method while connected
			if (connection.connected()) {
				resolve();
			} else {
				reject(new Error("connection.connected() should have returned true after connecting but didn't"));
			}

			return connection;
		}).then(connection => {

			// 3: Test connected() method after closing the connection
			connection.close();

			if (!connection.connected()) {
				resolve();
			} else {
				reject(new Error("connection.connected() should have returned false immediately after being closed but didn't"));
			}
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
