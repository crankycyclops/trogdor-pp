'use strict';

const ConnectionRequired = require('./lib/connectionrequired');

const Trogdord = require('../../lib/trogdord');
const Game = require('../../lib/game');
const Dump = require('../../lib/dump/dump');
const Slot = require('../../lib/dump/slot');

class TrogdordTest extends ConnectionRequired {

	/**
	 * Tests Trogdord.constructor().
	 */
	#testConstructor = function () {

		return new Promise((resolve, reject) => {

			// Test with default arguments
			const connection = new Trogdord();

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

					const connection = new Trogdord('localhost', 1040);

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

			const connection = new Trogdord();

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
	 * Tests Trogdord.config().
	 */
	#testConfig = function () {

		return new Promise((resolve, reject) => {

			const connection = new Trogdord();

			connection.on('connect', () => {

				connection.config().then(response => {

					if ('object' != typeof response) {
						reject(new Error("Response of Trogdord.config() should be an object but wasn't"));
					}

					resolve();
				}).catch(error => {
					reject(new Error(error.message));
				}).catch(error => {

					reject(error);
				});
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
			const validKeys = [
				'lib_version',
				'version',
				'players'
			];

			const connection = new Trogdord();

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
				}).catch(error => {

					reject(error);
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

			const connection = new Trogdord();

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
				}).catch(error => {

					reject(error);
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
			const connection = new Trogdord();

			const gameName = "My Game";
			const definition = "game.xml";

			let gameWithMeta;

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
						return connection.newGame(gameName, definition);
					}).then(game => {

						if (200 != connection.status) {
							reject(new Error("Trogdord.newGame() should have resulted in 200 status but didn't"));
						}

						if (!game instanceof Game) {
							reject(new Error("Trogdord.newGame() should resolve to an instance of Game but doesn't"));
						}

						if (!Number.isInteger(game.id)) {
							reject(new Error("newGame(): game.id not correctly initialized"));
						}

						if (game.name !== gameName) {
							reject(new Error("newGame(): game.name not correctly initialized"));
						}

						if (game.definition !== definition) {
							reject(new Error("newGame(): game.definition not correctly initialized"));
						}

						if (!Number.isInteger(game.created)) {
							reject(new Error("newGame(): game.created not correctly initialized"));
						}

						if (game.trogdord != connection) {
							reject(new Error("newGame(): game.trogdord not correctly initialized"));
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

						if (createdGame.id !== game.id) {
							reject(new Error("Game id of new game should have matched the game we retrieved, but it didn't"));
						}

						if (!Number.isInteger(game.id)) {
							reject(new Error("getGame(): game.id not correctly initialized"));
						}

						if (game.name !== gameName) {
							reject(new Error("getGame(): game.name not correctly initialized"));
						}

						if (game.definition !== definition) {
							reject(new Error("getGame(): game.definition not correctly initialized"));
						}

						if (!Number.isInteger(game.created)) {
							reject(new Error("getGame(): game.created not correctly initialized"));
						}

						if (game.trogdord != connection) {
							reject(new Error("getGame(): game.trogdord not correctly initialized"));
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

						if (games[0].id !== createdGame.id) {
							reject(new Error("Id of game in list doesn't match id of game returned by Trogdord.newGame()"));
						}

						// Make sure Game instances returned by Trogdord.games()
						// have their other members correctly initialized
						if (games[0].name !== gameName) {
							reject(new Error("games(): games[0].name not correctly initialized"));
						}

						if (games[0].definition !== definition) {
							reject(new Error("games(): games[0].definition not correctly initialized"));
						}

						if (!Number.isInteger(games[0].created)) {
							reject(new Error("games(): games[0].created not correctly initialized"));
						}

						if (games[0].trogdord != connection) {
							reject(new Error("games(): games[0].trogdord not correctly initialized"));
						}

						// Clean up
						return games[0].destroy();
					}).then(response => {

						if (200 != connection.status) {
							reject(new Error("Game should have been successfully destroyed but wasn't"));
						}

						// Attempt to create a second game, this time with third optional meta argument
						return connection.newGame(gameName, definition, {int: 1, str: "wee", boolean: true});
					}).then(game => {

						if (200 != connection.status) {
							reject(new Error("Game creation should have been successful but wasn't"));
						}

						gameWithMeta = game;
						return game.getMeta();
					}).then(data => {

						if (!data.hasOwnProperty('int') || '1' != data.int) {
							reject(new Error('int meta value was not set or was formatted incorrectly'));
						}

						if (!data.hasOwnProperty('str') || 'wee' != data.str) {
							reject(new Error('str meta value was not set or was formatted incorrectly'));
						}

						if (!data.hasOwnProperty('boolean') || 'true' != data.boolean) {
							reject(new Error('boolean meta value was not set or was formatted incorrectly'));
						}

						// Clean up after second game
						return gameWithMeta.destroy();
					}).then(() => {

						if (200 != connection.status) {
							reject(new Error("Destroying game with meta should have been successful but wasn't"));
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

			const gameName1 = "aaa";
			const gameName2 = "bbb";
			const definition = "game.xml";
	
			const connection = new Trogdord();

			// Game1 is named "aaa" and will be started.
			// Game2 is named "bbb" and will be started.
			// Game3 is named "aaa" and will be stopped.
			// Game4 is named "bbb" and will be stopped.
			let game1, game2, game3, game4;

			connection.on('connect', () => {

				connection.newGame(gameName1, definition).then(game => {

					game1 = game;
					return connection.newGame(gameName2, definition);
				}).then(game => {

					game2 = game;
					return connection.newGame(gameName1, definition);
				}).then(game => {

					game3 = game;
					return connection.newGame(gameName2, definition);
				}).then(game => {

					game4 = game;
					return game1.start();
				}).then(response => {

					return game2.start();
				}).then(response => {

					return game3.stop();
				}).then(response => {

					return game4.stop();
				}).then(response => {

					// Test is_running = false filter
					return connection.games({is_running: false});
				}).then(games => {

					if (2 != games.length) {
						reject(new Error('There should be exactly two stopped games'));
					}

					games.forEach(game => {
						if (game3.id != game.id && game4.id != game.id) {
							reject(new Error("Got a game that wasn't stopped when filtering for stopped games"));
						}
					});

					// Test is_running = true filter
					return connection.games({is_running: true});
				}).then(games => {

					if (2 != games.length) {
						reject(new Error('There should be exactly two started games'));
					}

					games.forEach(game => {
						if (game1.id != game.id && game2.id != game.id) {
							reject(new Error("Got a game that wasn't started when filtering for started games"));
						}
					});

					// Test name_starts = 'a' filter
					return connection.games({name_starts: 'a'});
				}).then(games => {

					if (2 != games.length) {
						reject(new Error("There should be exactly two games that start with 'a'"))
					}

					games.forEach(game => {
						if (game1.id != game.id && game3.id != game.id) {
							reject(new Error("Got a game that doesn't start with 'a'"));
						}
					});

					// Test name_starts = 'b' filter
					return connection.games({name_starts: 'b'});
				}).then(games => {

					if (2 != games.length) {
						reject(new Error("There should be exactly two games that start with 'b'"))
					}

					games.forEach(game => {
						if (game2.id != game.id && game4.id != game.id) {
							reject(new Error("Got a game that doesn't start with 'b'"));
						}
					});

					// Test filter group name_starts = 'a' && is_running = true
					return connection.games({is_running: true, name_starts: 'a'});
				}).then(games => {

					if (1 != games.length) {
						reject(new Error("There should be exactly one started game that start with 'a'"))
					}

					if (game1.id != games[0].id) {
						reject(new Error("Got a game that wasn't started or that didn't begin with 'a'"));
					}

					// Test filter union name_starts = 'a' || is_running = true
					return connection.games([{is_running: true}, {name_starts: 'a'}]);
				}).then(games => {

					if (3 != games.length) {
						reject(new Error("There should be exactly three games that are either started or begin with 'a'"))
					}

					games.forEach(game => {
						if (game1.id != game.id && game2.id != game.id && game3.id != game.id) {
							reject(new Error("Got a game that wasn't started or didn't begin with 'a'"));
						}
					});

				}).then(() => {

					// Clean up
					return game1.destroy();
				}).then(() => {

					return game2.destroy();
				}).then(() => {

					return game3.destroy();
				}).then(() => {

					return game4.destroy();
				}).then(() => {

					resolve();
				}).catch(error => {

					reject(error);
				});
			});

			connection.on('error', (e) => {
				reject(new Error(e.message));
			});
		});
	}

	/**
	 * Tests Trogdord.dump(), Trogdord.restore(), and Trogdord.dumped(). Since
	 * ES6 doesn't support real private methods and I instead have to create
	 * this function with no binding to the class, I'm passing in a "this"
	 * reference called "that."
	 */
	#testDumpRestoreAndDumped = function (that) {

		return new Promise((resolve, reject) => {

			that.isStateEnabled().then(enabled => {

				if (enabled) {

					const gameName = "My Game";
					const definition = "game.xml";

					let game1, game2;
					let dumps = [];

					const connection = new Trogdord();

					connection.on('connect', () => {

						connection.newGame(gameName, definition).then(game => {

							game1 = game;
							return connection.newGame(gameName, definition);
						}).then(game => {

							game2 = game;
							return connection.dump();
						}).then(() => {

							if (200 != connection.status) {
								reject(new Error('connection.status should return 200 after successful call to connection.dump()'));
							}

							return connection.dumped();
						}).then(dumped => {

							if (2 != dumped.length) {
								reject(new Error('Two games should have been dumped, but two games were not returned by Trogdord.dumped()'));
							}

							dumped.forEach(dump => {
								if (dump.id != game1.id && dump.id != game2.id) {
									reject(new Error("One or more dumped game objects have ids that don't match either of the two created games"));
								}
							});

							dumps = dumped;
							return game1.destroy(false);
						}).then(() => {

							return game2.destroy(false);
						}).then(() => {

							return connection.games();
						}).then(games => {

							if (0 != games.length) {
								reject(new Error('Both games should have been destroyed but were not'));
							}

							return connection.restore();
						}).then(() => {

							if (200 != connection.status) {
								reject(new Error('connection.status should return 200 after successful call to connection.restore()'));
							}

							return connection.games();
						}).then(games => {

							if (2 != games.length) {
								reject(new Error('Both games should have been restored but were not'));
							}

							games.forEach(game => {
								if (game.id != game1.id && game.id != game2.id) {
									reject(new Error("One or more dumped games were improperly restored"));
								}
							});

							// Clean up
							return game1.destroy(true);
						}).then(() => {

							return game2.destroy(true);
						}).then(() => {

							resolve();
						}).catch(error => {

							reject(new Error(e.message));
						});
					});

					connection.on('error', (e) => {
						reject(new Error(e.message));
					});
				}

				else {

					const connection = new Trogdord();

					connection.on('connect', () => {

						connection.dumped().then(response => {

							reject(new Error("Call to Trogdord.dumped() shouldn't succeed when the state feature is disabled in trogdord.ini"));
						}).catch(error => {

							if (501 != error.status) {
								reject(new Error("501 should have been error status but wasn't"));
							}

							else if (501 != connection.status) {
								reject(new Error("501 should be the return value of connection.status but wasn't"));
							}

							return connection.dump();
						}).then(response => {

							reject(new Error("Call to Trogdord.dump() shouldn't succeed when the state feature is disabled in trogdord.ini"));
						}).catch(error => {

							if (501 != error.status) {
								reject(new Error("501 should have been error status but wasn't"));
							}

							else if (501 != connection.status) {
								reject(new Error("501 should be the return value of connection.status but wasn't"));
							}

							return connection.restore();
						}).then(response => {

							reject(new Error("Call to Trogdord.restore() shouldn't succeed when the state feature is disabled in trogdord.ini"));
						}).catch(error => {

							if (501 != error.status) {
								reject(new Error("501 should have been error status but wasn't"));
							}

							else if (501 != connection.status) {
								reject(new Error("501 should be the return value of connection.status but wasn't"));
							}

							resolve();
						});
					});

					connection.on('error', (e) => {
						reject(new Error(e.message));
					});
				}

			}).catch(error => {
				reject(error);
			});
		});
	}

	/**
	 * Tests Trogdord.getDump(). Since ES6 doesn't support real private
	 * methods and I instead have to create this function with no binding to
	 * the class, I'm passing in a "this" reference called "that."
	 */
	#testGetDump = function (that) {

		return new Promise((resolve, reject) => {

			that.isStateEnabled().then(enabled => {

				if (enabled) {

					const gameName = "My Game";
					const definition = "game.xml";

					let game, dump;

					const connection = new Trogdord();

					connection.on('connect', () => {

						// 1. Test getting a non-existent dump
						connection.getDump(0).then(response => {

							reject(new Error('Getting non-existent dump should not have been successful'));
						}).catch(error => {

							if (404 != connection.status) {
								reject(new Error("Should have received a 404 status for non-existent dump but didn't"));
							}

							// 2. Create a game, dump it, and make sure we can retrieve it
							return connection.newGame(gameName, definition);
						}).then(newGame => {

							if (200 != connection.status) {
								reject(new Error('Creating game should have been successful'));
							}

							game = newGame;
							return game.dump();
						}).then(newDump => {

							if (200 != connection.status) {
								reject(new Error('Dumping game should have been successful'));
							}

							dump = newDump;

							// Validate properties of returnd Dump object
							if (game.id != dump.id) {
								reject(new Error('Dump id has the following invalid value: ' + dump.id));
							}

							if (game.name != dump.name) {
								reject(new Error('Dump name has the following invalid value: ' + dump.name));
							}

							if (game.definition != dump.definition) {
								reject(new Error('Dump definition has the following invalid value: ' + dump.definition));
							}

							if (game.created != dump.created) {
								reject(new Error('Dump created property has the following invalid value: ' + dump.created));
							}

							if (game.trogdord != dump.trogdord) {
								reject(new Error('Dump trogdord property is invalid'));
							}

							// 3. Clean up game and dump
							return game.destroy();
						}).then(response => {

							resolve();
						}).catch(error => {

							reject(error);
						});
					});

					connection.on('error', (e) => {
						reject(new Error(e.message));
					});
				}

				else {

					const connection = new Trogdord();

					connection.on('connect', () => {

						connection.getDump(0).then(response => {

							reject(new Error("Call to Trogdord.dumped() shouldn't succeed when the state feature is disabled in trogdord.ini"));
						}).catch(error => {

							if (501 != error.status) {
								reject(new Error("501 should have been error status but wasn't"));
							}

							else if (501 != connection.status) {
								reject(new Error("501 should be the return value of connection.status but wasn't"));
							}

							resolve();
						});
					});

					connection.on('error', (e) => {
						reject(new Error(e.message));
					});
				}

			}).catch(error => {

				reject(error);
			});
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

			this.addTest("Trogdord.construct()", this.#testConstructor);
			this.addTest("Trogdord.connected Getter", this.#testConnectedGetter);
			this.addTest("Trogdord.status Getter", this.#testStatusGetter);
			this.addTest("Trogdord.config()", this.#testConfig);
			this.addTest("Trogdord.statistics()", this.#testStatistics);
			this.addTest("Trogdord.definitions()", this.#testDefinitions);
			this.addTest("Trogdord.games() without filters, Trogdord.newGame(), and Trogdord.getGame()", this.#testNewGameAndGetGame);
			this.addTest("Trogdord.games() with filters", this.#testGamesWithFilters);
			this.addTest("Trogdord.dump(), Trogdord.restore(), and Trogdord.dumped()", () => this.#testDumpRestoreAndDumped(this));
			this.addTest("Trogdord.getDump()", () => this.#testGetDump(this));
			this.addTest("Trogdord.makeRequest() and Trogdord.close()", this.#testMakeRequestAndClose);

			resolve();
		});
	}
};

module.exports = TrogdordTest;
