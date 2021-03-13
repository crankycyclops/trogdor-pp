'use strict';

const ConnectionRequired = require('./lib/connectionrequired');
const Trogdord = require('../../lib/trogdord');

class GameTest extends ConnectionRequired {

	/**
	 * Tests Game.statistics()
	 */
	#testStatistics = function () {

		return new Promise((resolve, reject) => {

			let game;

			const gameName = "My Game";
			const definition = "game.xml";

			const connection = new Trogdord();

			connection.on('connect', () => {

				connection.newGame(gameName, definition).then(newGame => {

					game = newGame;
					return newGame.statistics();
				}).then(response => {

					const validKeys = ['created', 'players', 'current_time', 'is_running'];

					let responseKeys = Object.keys(response);
					let keysFound = [];

					if (200 != connection.status) {
						reject(new Error("Request should have returned 200 status but did not"));
					}

					if ('object' != typeof response) {
						reject(new Error("Response should be of type 'object' but isn't"));
					}

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

					// TOOD: this should be a UNIX timestamp
					if (isNaN(Date.parse(response.created))) {
						reject(new Error('response.created should be a string that can be parsed into a timestamp'));
					}

					if (!Number.isInteger(response.players)) {
						reject(new Error("response.players should be an integer but isn't"));
					}

					if (!Number.isInteger(response.current_time)) {
						reject(new Error("response.current_time should be an integer but isn't"));
					}

					if (response.current_time < 0) {
						reject(new Error("response.current_time should be unsigned but isn't"));
					}

					if ('boolean' != typeof(response.is_running)) {
						reject(new Error("response.is_running should be a boolean value but isn't"));
					}

					// Clean up
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
		});
	}

	/**
	 * Tests Game.start(), Game.stop(), and Game.isRunning()
	 */
	#testIsRunningAndStartStop = function () {

		return new Promise((resolve, reject) => {

			let game;
			const connection = new Trogdord();

			connection.on('connect', () => {

				connection.newGame("My Game", "game.xml").then(newGame => {

					game = newGame;
					return game.stop();
				}).then(() => {

					if (200 != connection.status) {
						reject(new Error('Stopping game should have been successful'));
					}

					return game.isRunning();
				}).then(response => {

					if (200 != connection.status) {
						reject(new Error('Call to game.isRunning() should have been successful'));
					}

					if (response) {
						reject(new Error('game.isRunning() should resolve to false but does not'));
					}

					return game.start();
				}).then(response => {

					if (200 != connection.status) {
						reject(new Error('Starting game should have been successful'));
					}

					return game.isRunning();
				}).then(response => {

					if (200 != connection.status) {
						reject(new Error('Call to game.isRunning() should have been successful'));
					}

					if (!response) {
						reject(new Error('game.isRunning() should resolve to true but does not'));
					}

					// Clean up
					return game.destroy();
				}).then(() => {

					if (200 != connection.status) {
						reject(new Error('Destroying game should have been successful'));
					}

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
	 * Tests Game.getTime()
	 */
	#testGetTime = function () {

		return new Promise((resolve, reject) => {

			let game;
			const connection = new Trogdord();

			connection.on('connect', () => {

				connection.newGame("My Game", "game.xml").then(newGame => {

					game = newGame;
					return game.stop();
				}).then(() => {

					if (200 != connection.status) {
						reject(new Error('Stopping game should have been successful'));
					}

					return game.getTime();
				}).then(time => {

					if (200 != connection.status) {
						reject(new Error('Call to game.getTime() should have been successful'));
					}

					if (0 != time) {
						reject(new Error('Game time should be 0 when game is initialized in stopped state'));
					}

					return game.start();
				}).then(response => {

					if (200 != connection.status) {
						reject(new Error('Call to game.start() should have been successful'));
					}

					return new Promise((resolve, reject) => {

						// Wait for the time to advance
						setTimeout(() => {

							resolve();
						}, 1500);
					});
				}).then(() => {

					return game.getTime();
				}).then(time => {

					if (200 != connection.status) {
						reject(new Error('Call to game.getTime() should have been successful'));
					}

					if (time <= 0) {
						reject(new Error('Game time should be greater than 0 after starting but is not'));
					}

					// Clean up
					return game.destroy();
				}).then(() => {

					if (200 != connection.status) {
						reject(new Error('Destroying game should have been successful'));
					}

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
	 * Tests Game.dump()
	 */
	#testDump = function (that) {

		return new Promise((resolve, reject) => {

			that.isStateEnabled().then(enabled => {

				const name = "My Game";
				const definition = "game.xml";

				if (enabled) {

					const connection = new Trogdord();

					connection.on('connect', () => {

						let game;

						connection.newGame(name, definition).then(newGame => {

							if (200 != connection.status) {
								reject(new Error('Creating game should have been successful'));
							}

							game = newGame;
							return game.dump();
						}).then(dump => {

							if (200 != connection.status) {
								reject(new Error('Dumping game should have been successful'));
							}

							if (game.id != dump.id || !Number.isInteger(dump.id)) {
								reject(new Error('Dump.id not correctly initialized on return from Game.dump()'));
							}

							if (name != dump.name) {
								reject(new Error('Dump.name not correctly initialized on return from Game.dump()'));
							}

							if (definition != dump.definition) {
								reject(new Error('Dump.definition not correctly initialized on return from Game.dump()'));
							}

							if (!Number.isInteger(dump.created) || dump.created != game.created) {
								reject(new Error('Dump.created not correctly initialized on return from Game.dump()'));
							}

							if (connection != dump.trogdord) {
								reject(new Error('Dump.trogdord not correctly initialized on return from Game.dump()'));
							}

							// Clean up
							return game.destroy(true);
						}).then(() => {

							if (200 != connection.status) {
								reject(new Error('Destroying game and dump should have been successful'));
							}

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

						let game;

						connection.newGame(name, definition).then(newGame => {

							if (200 != connection.status) {
								reject(new Error('Creating game should have been successful'));
							}

							game = newGame;
							return game.dump();
						}).then(() => {

							reject(new Error('Dumping game should not be successful when state feature is turned off'));
						}).catch(error => {

							if (501 != connection.status) {
								reject(new Error('Attempting to dump when state is disabled should return a 501 unsupported status'));
							}

							return game.destroy();
						}).then(() => {

							if (200 != connection.status) {
								reject(new Error('Destroying game should have been successful'));
							}

							resolve();
						}).catch(error => {

							reject(error);
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
	 * Tests Game.destroy()
	 */
	#testDestroy = function (that) {

		return new Promise((resolve, reject) => {

			let game;

			const name = "My Game";
			const definition = "game.xml";

			that.isStateEnabled().then(enabled => {

				if (enabled) {

					let dump;
					const connection = new Trogdord();

					connection.on('connect', () => {

						connection.newGame(name, definition).then(newGame => {

							if (200 != connection.status) {
								reject(new Error('Creating game should have been successful'));
							}

							game = newGame;
							return game.dump();
						}).then(newDump => {

							if (200 != connection.status) {
								reject(new Error('Destroying game should have been successful'));
							}

							if (game.id != newDump.id) {
								reject(new Error("Dump id doesn't match game id"));
							}

							dump = newDump;

							// 1: test destroyDump = false
							return game.destroy(false);
						}).then(() => {

							if (200 != connection.status) {
								reject(new Error('Destroying game should have been successful'));
							}

							return connection.getDump(dump.id);
						}).then(gottenDump => {

							if (200 != connection.status) {
								reject(new Error('Destroying game should have been successful'));
							}

							if (gottenDump.id != dump.id) {
								reject(new Error("Dump ids don't match"));
							}

							return connection.getGame(game.id);
						}).then(() => {

							reject(new Error("Getting destroyed game should have failed but didn't"));
						}).catch(error => {

							if (404 != connection.status) {
								reject(new Error("Getting destroyed game should have resulted in 404 game not found but didn't"));
							}

							return connection.newGame(name, definition);
						}).then(newGame => {

							if (200 != connection.status) {
								reject(new Error('Creating game should have been successful'));
							}

							game = newGame;
							return game.dump();
						}).then(newDump => {

							if (200 != connection.status) {
								reject(new Error('Destroying game should have been successful'));
							}

							if (game.id != newDump.id) {
								reject(new Error("Dump id doesn't match game id"));
							}

							dump = newDump;

							// 2: test destroyDump = true
							return game.destroy(true);
						}).then(() => {

							if (200 != connection.status) {
								reject(new Error('Destroying game should have been successful'));
							}

							return connection.getDump(dump.id);
						}).then(() => {

							reject(new Error("Dump should have been destroyed by call to game.destroy(true) but was not"));
						}).catch(error => {

							if (404 != connection.status) {
								reject(new Error("Getting destroyed dump should have resulted in 404 dump not found but didn't"));
							}

							return connection.getGame(game.id);
						}).then(() => {

							reject(new Error("Getting destroyed game should have failed but didn't"));
						}).catch(error => {

							if (404 != connection.status) {
								reject(new Error("Getting destroyed game should have resulted in 404 game not found but didn't"));
							}

							return connection.newGame(name, definition);
						}).then(newGame => {

							if (200 != connection.status) {
								reject(new Error('Creating game should have been successful'));
							}

							game = newGame;
							return game.dump();
						}).then(newDump => {

							if (200 != connection.status) {
								reject(new Error('Destroying game should have been successful'));
							}

							if (game.id != newDump.id) {
								reject(new Error("Dump id doesn't match game id"));
							}

							dump = newDump;

							// 3: test with default argument (destroyDump = true)
							return game.destroy();
						}).then(() => {

							if (200 != connection.status) {
								reject(new Error('Destroying game should have been successful'));
							}

							return connection.getDump(dump.id);
						}).then(() => {

							reject(new Error("Dump should have been destroyed by call to game.destroy() but was not"));
						}).catch(error => {

							if (404 != connection.status) {
								reject(new Error("Getting destroyed dump should have resulted in 404 dump not found but didn't"));
							}

							return connection.getGame(game.id);
						}).then(() => {

							reject(new Error("Getting destroyed game should have failed but didn't"));
						}).catch(error => {

							if (404 != connection.status) {
								reject(new Error("Getting destroyed game should have resulted in 404 game not found but didn't"));
							}

							resolve();
						});
					});

					connection.on('error', (e) => {
						reject(new Error(e.message));
					});
				}

				// When the state feature is disabled, the argument that you
				// pass to Game.destroy() shouldn't matter; the game should
				// always be destroyed.
				else {

					const connection = new Trogdord();

					connection.on('connect', () => {

						connection.newGame(name, definition).then(newGame => {

							if (200 != connection.status) {
								reject(new Error('Creating game should have been successful'));
							}

							game = newGame;

							// 1: test default argument
							return game.destroy();
						}).then(() => {

							if (200 != connection.status) {
								reject(new Error('Destroying game should have been successful'));
							}

							// Make sure game was destroyed
							return connection.getGame(game.id);
						}).then(() => {

							reject(new Error('Got game that should have been deleted'));
						}).catch(() => {

							if (404 != connection.status) {
								reject(new Error('Should have gotten 404 game not found'));
							}

							return connection.newGame(name, definition);
						}).then(newGame => {

							game = newGame;

							// 2: test with destroyDump = false
							return game.destroy(false);
						}).then(() => {

							if (200 != connection.status) {
								reject(new Error('Destroying game should have been successful'));
							}

							// Make sure game was destroyed
							return connection.getGame(game.id);
						}).then(() => {

							reject(new Error('Got game that should have been deleted'));
						}).catch(() => {

							if (404 != connection.status) {
								reject(new Error('Should have gotten 404 game not found'));
							}

							return connection.newGame(name, definition);
						}).then(newGame => {

							game = newGame;

							// 3: test with destroyDump = true
							return game.destroy(true);
						}).then(() => {

							if (200 != connection.status) {
								reject(new Error('Destroying game should have been successful'));
							}

							// Make sure game was destroyed
							return connection.getGame(game.id);
						}).then(() => {

							reject(new Error('Got game that should have been deleted'));
						}).catch(() => {

							if (404 != connection.status) {
								reject(new Error('Should have gotten 404 game not found'));
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
	 * Tests Game.setMeta() and Game.getMeta()
	 */
	#testGetAndSetMeta = function () {

		return new Promise((resolve, reject) => {

			const connection = new Trogdord();

			connection.on('connect', () => {

				// TODO
				resolve();
			});

			connection.on('error', (e) => {
				reject(new Error(e.message));
			});
		});
	}

	/**
	 * Tests Game.createPlayer()
	 */
	#testCreatePlayer = function () {

		return new Promise((resolve, reject) => {

			const connection = new Trogdord();

			connection.on('connect', () => {

				// TODO
				resolve();
			});

			connection.on('error', (e) => {
				reject(new Error(e.message));
			});
		});
	}

	/**
	 * Tests Game Entity List Methods
	 */
	#testEntityListMethods = function () {

		return new Promise((resolve, reject) => {

			const connection = new Trogdord();

			connection.on('connect', () => {

				// TODO
				resolve();
			});

			connection.on('error', (e) => {
				reject(new Error(e.message));
			});
		});
	}

	/**
	 * Tests Game Entity Getter Methods
	 */
	#testEntityGetterMethods = function () {

		return new Promise((resolve, reject) => {

			const connection = new Trogdord();

			connection.on('connect', () => {

				// TODO
				resolve();
			});

			connection.on('error', (e) => {
				reject(new Error(e.message));
			});
		});
	}

	/**
	 * Tests various getters for the Game class.
	 */
	#testGetters = function () {

		return new Promise((resolve, reject) => {

			const gameName = "My Game";
			const definition = "game.xml";

			const connection = new Trogdord();

			connection.on('connect', () => {

				connection.newGame(gameName, definition).then(game => {

					if (!Number.isInteger(game.id) || game.id < 0) {
						reject(new Error('Value of Game.id is invalid'));
					}

					if (gameName != game.name) {
						reject(new Error('Value of Game.name is invalid'));
					}

					if (definition != game.definition) {
						reject(new Error('Value of Game.definition is invalid'));
					}

					if (!Number.isInteger(game.created) || game.created < 0) {
						reject(new Error('Value of Game.created is invalid'));
					}

					if (connection != game.trogdord) {
						reject(new Error('Value of Game.trogdord is invalid'));
					}

					// Clean up
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
		});
	}

	/**
	 * Initialize tests.
	 */
	init() {

		return new Promise((resolve, reject) => {

			this.addTest("Game.statistics()", this.#testStatistics);
			this.addTest("Game.start(), Game.stop(), and Game.isRunning()", this.#testIsRunningAndStartStop);
			this.addTest("Game.getTime()", this.#testGetTime);
			this.addTest("Game.dump()", () => this.#testDump(this));
			this.addTest("Game.destroy()", () => this.#testDestroy(this));
			this.addTest("Game.setMeta() and Game.getMeta()", this.#testGetAndSetMeta);
			this.addTest("Game.createPlayer()", this.#testCreatePlayer);
			this.addTest("Game entity list methods", this.#testEntityListMethods);
			this.addTest("Game entity getter methods", this.#testEntityGetterMethods);
			this.addTest("Game Getters for id, name, definition, created, and trogdord", this.#testGetters);
			resolve();
		});
	}
};

module.exports = GameTest;
