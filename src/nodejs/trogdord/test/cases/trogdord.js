'use strict';

const ConnectionRequired = require('./lib/connectionrequired');

const Trogdord = require('../../lib/trogdord');
const Game = require('../../lib/game');

class TrogdordTest extends ConnectionRequired {

	/**
	 * Tests Trogdord.constructor().
	 */
	#testConstructor = function () {

		return new Promise((resolve, reject) => {

			// Test with default arguments
			let connection = new Trogdord();

			connection.on('connect', () => {
				connection.close();
				resolve('Connected');
			});

			connection.on('error', (e) => {
				reject(new Error(e.message));
			});
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
			connection = new Trogdord();

			connection.on('connect', () => {
				resolve('Connected');
			});

			connection.on('error', (e) => {
				reject(new Error(e.message));
			});
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

		return new Promise((resolve, reject) => {

			let connection = new Trogdord();

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
		});
	}

	/**
	 * Tests Trogdord.statistics().
	 */
	#testStatistics = function () {

		return new Promise((resolve, reject) => {

			// I only check these so that if I forget to write checks for additional
			// stats that are added later, I'll get a nice obvious warning in the
			// form of a failed test :)
			let validKeys = [
				'lib_version',
				'version',
				'players'
			];

			let connection = new Trogdord();

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
		});
	}

	/**
	 * Tests Trogdord.definitions().
	 */
	#testDefinitions = function () {

		return new Promise((resolve, reject) => {

			let connection = new Trogdord();

			connection.on('connect', () => {

				connection.definitions().then(response => {

					if (200 != connection.status) {
						reject(new Error("Status after successful call to Trogdord.definitions() should have been 200 but wasn't"));
					}

					if (!Array.isArray(response)) {
						reject(new Error("Response should have been an array of definitions but wasn't"));
					}

					response.forEach((definition, response) => {
						if ("string" != typeof definition) {
							reject(new Error("One or more returned definitions was not a string but should have been"));
						}
					});

					resolve();
				});
			});

			connection.on('error', (e) => {
				reject(new Error(e.message));
			});
		});
	}

	/**
	 * Tests Trogdord.games(), Trogdord.newGame(), and Trogdord.getGame().
	 */
	#testNewGameAndGetGame = function () {

		return new Promise((resolve, reject) => {

			let createdGame;
			let connection = new Trogdord();

			connection.on('connect', () => {

				// Attempt to get a game that doesn't exist
				connection.getGame(0).then(response => {

					reject(new Error("Call to getGame() with non-existent id should have failed but didn't"));
				}).catch(error => {

					if (404 != error.status) {
						reject(new Error("Call to getGame() with non-existent id should have resulted in 404 status but didn't"));
					}

					// Get a list of all games so far and verify the list is empty
					connection.games().then(response => {

						if (200 != connection.status) {
							reject(new Error("Trogdord.games() should have resulted in 200 status but didn't"));
						}

						if (!Array.isArray(response)) {
							reject(new Error("Trogdord.games() should have returned an array but didn't"));
						}

						if (0 != response.length) {
							reject(new Error("Games list should be empty before one has been created but wasn't"));
						}

						// Attempt to create a new game
						return connection.newGame("My Game", "game.xml");
					}).then(game => {

						if (200 != connection.status) {
							reject(new Error("Trogdord.newGame() should have resulted in 200 status but didn't"));
						}

						if (!game instanceof Game) {
							reject(new Error("Trogdord.newGame() should resolve to an instance of Game but doesn't"));
						}

						// Attempt to get the game we just created
						createdGame = game;
						return connection.getGame(game.id);
					}).then(game => {

						if (200 != connection.status) {
							reject(new Error("Trogdord.getGame() should return 200 status on success but doesn't"));
						}

						if (!game instanceof Game) {
							reject(new Error("Return value of Trogdord.getGame() should resolve to instance of Game but doesn't"));
						}

						if (createdGame.id != game.id) {
							reject(new Error("Game id of new game should have matched the game we retrieved, but it didn't"));
						}

						// Get a list of games again and show that the new games is now part of it
						return connection.games();
					}).then(games => {

						if (200 != connection.status) {
							reject(new Error("Trogdord.games() should have resulted in 200 status but didn't"));
						}

						if (!Array.isArray(games)) {
							reject(new Error("Trogdord.games() should have returned an array but didn't"));
						}

						if (1 != games.length) {
							reject(new Error("Games list should contain exactly one value but doesn't"));
						}

						if (!games[0] instanceof Game) {
							reject(new Error("Members of array returned by Trogdord.games() should be instances of Game"));
						}

						if (games[0].id != createdGame.id) {
							reject(new Error("Id of game in list doesn't match id of game returned by Trogdord.newGame()"));
						}

						// TODO: make sure Game instance has all its members correctly initialized

						// Clean up
						return games[0].destroy();
					}).then(response => {

						if (200 != connection.status) {
							reject(new Error("Game should have been successfully destroyed but wasn't"));
						}

						resolve();
					}).catch(error => {

						reject(error);
					})
				})
			});

			connection.on('error', (e) => {
				reject(new Error(e.message));
			});
		});
	}

	/**
	 * Tests Trogdord.games() with filters.
	 */
	#testGamesWithFilters = function () {

		return new Promise((resolve, reject) => {

			// TODO
			resolve();
		});
	}

	/**
	 * Tests Trogdord.makeRequest() and Trogdord.close().
	 */
	#testMakeRequestAndClose = function () {

		return new Promise((resolve, reject) => {

			let connection = new Trogdord();

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
	 * Initialize tests.
	 */
	init() {

		return new Promise((resolve, reject) => {

			this.addTest("Trogdord.connected Getter", this.#testConnectedGetter);
			this.addTest("Trogdord.status Getter", this.#testStatusGetter);
			this.addTest("Trogdord.statistics()", this.#testStatistics);
			this.addTest("Trogdord.definitions()", this.#testDefinitions);
			this.addTest("Trogdord.games(), Trogdord.newGame(), and Trogdord.getGame()", this.#testNewGameAndGetGame);
			this.addTest("Trogdord.games() with filters", this.#testGamesWithFilters);
			this.addTest("Trogdord.makeRequest() and Trogdord.close()", this.#testMakeRequestAndClose);

			resolve();
		});
	}
};

module.exports = TrogdordTest;
