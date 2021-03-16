'use strict';

const Trogdord = require('../../lib/trogdord');
const StateRequired = require('./lib/staterequired');

class DumpTest extends StateRequired {

	/**
	 * Tests Dump.destroy()
	 */
	 #testDestroy = function () {

		return new Promise((resolve, reject) => {

			let game, dump;

			const gameName = "My Game";
			const definition = "game.xml";

			const connection = new Trogdord();

			connection.on('connect', () => {

				connection.newGame(gameName, definition).then(newGame => {

					game = newGame;
					return game.dump();
				}).then(newDump => {

					dump = newDump;
					return connection.getDump(dump.id);
				}).then(gottenDump => {

					if (200 != connection.status) {
						reject(new Error("Getting newly created dump should have been successful"));
					}

					if (gottenDump.id != dump.id) {
						reject(new Error("Id of the fetched dump should match the newly created dump's id"));
					}

					return dump.destroy();
				}).catch(error => {

					reject(error);
				}).then(() => {

					return connection.getDump(dump.id);
				}).then(dump => {

					reject(new Error("Getting destroyed dump should have failed but didn't"));
				}).catch(error => {

					if (404 != connection.status) {
						reject(new Error("Getting destroyed dump should have failed with 404 but failed with some other status instead"));
					}

					// Clean up
					return game.destroy();
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
	 * Tests various getters for the Dump class.
	 */
	#testGetters = function () {

		return new Promise((resolve, reject) => {

			let game, dump;

			const gameName = "My Game";
			const definition = "game.xml";

			const connection = new Trogdord();

			connection.on('connect', () => {

				connection.newGame(gameName, definition).then(newGame => {

					game = newGame;
					return game.dump();
				}).then(newDump => {

					dump = newDump;

					if (dump.id != game.id) {
						reject(new Error("Dump id must match game id but doesn't"));
					}

					if (dump.name != game.name) {
						reject(new Error("Dump name must match game name but doesn't"));
					}

					if (dump.definition != game.definition) {
						reject(new Error("Dump definition must match game definition but doesn't"));
					}

					if (dump.created != game.created) {
						reject(new Error("Dump created property must match game created property but doesn't"));
					}

					if (dump.trogdord != connection) {
						reject(new Error("Dump trogdord property must refer to the connection that spawned it but doesn't"));
					}

					// Clean up
					return game.destroy();
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
	 * Initialize tests.
	 */
	init() {

		return new Promise((resolve, reject) => {

			this.addTest("Dump.destroy()", this.#testDestroy);
			this.addTest("Dump Getters for id, name, definition, created, and trogdord", this.#testGetters);
			resolve();
		});
	}
};

module.exports = DumpTest;
