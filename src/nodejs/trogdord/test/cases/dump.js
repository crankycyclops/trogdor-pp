'use strict';

const Trogdord = require('../../lib/trogdord');
const StateRequired = require('./lib/staterequired');

class DumpTest extends StateRequired {

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

			this.addTest("Dump Getters for id, name, definition, created, and trogdord", this.#testGetters);
			resolve();
		});
	}
};

module.exports = DumpTest;
