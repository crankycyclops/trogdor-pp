'use strict';

const Trogdord = require('../../lib/trogdord');
const StateRequired = require('./lib/staterequired');

class SlotTest extends StateRequired {

	/**
	 * Tests Slot.destroy()
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
					return game.dump();
				}).then(() => {

					return dump.getSlot(1);
				}).catch(error => {

					reject(error);
				}).then(slot => {

					return slot.destroy();
				}).then(() => {

					return dump.getSlot(1);
				}).then(() => {

					reject(new Error("Getting destroyed dump slot should have failed"));
				}).catch(error => {

					if (404 != connection.status) {
						reject(new Error("Getting destroyed dump slot should have resulted in 404 slot not found but got different status: " + connection.status));
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
	 * Tests Slot.restore()
	 */
	 #testRestore = function () {

		return new Promise((resolve, reject) => {

			let game, dump;

			const gameName = "My Game";
			const definition = "game.xml";

			const connection = new Trogdord();

			connection.on('connect', () => {

				connection.newGame(gameName, definition).then(newGame => {

					game = newGame;

					// Dump one version of the game without a meta key set
					return game.dump();
				}).then(newDump => {

					dump = newDump;
					return game.setMeta({key: "value"});
				}).then(response => {

					// Dump another version of the game without a meta key set
					return game.dump();
				}).then(() => {

					return game.destroy(false);
				}).then(() => {

					return dump.getSlot(1);
				}).then(slot => {

					return slot.restore();
				}).then(() => {

					return game.getMeta(['key']);
				}).then(data => {

					if ('value' != data['key']) {
						reject(new Error('The latest version of the game should have been restored, but the earlier one was restored instead'));
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
			});;
		});
	}

	/**
	 * Tests various getters for the Slot class.
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
					return dump.getSlot(0);
				}).then(slot => {

					if (200 != connection.status) {
						reject(new Error("Getting dump slot should have been successful"));
					}

					if (!Number.isInteger(slot.slot) || slot.slot < 0) {
						reject(new Error("Slot member should be unsigned integer"));
					}

					if (!Number.isInteger(slot.timestamp) || slot.timestamp < 0) {
						reject(new Error("Timestamp member should be unsigned integer"));
					}

					if (dump != slot.dump) {
						reject(new Error("Dump member was not properly initialized"));
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

			this.addTest("Slot.destroy()", this.#testDestroy);
			this.addTest("Slot.restore()", this.#testRestore);
			this.addTest("Slot Getters for slot, timestamp, and dump", this.#testGetters);
			resolve();
		});
	}
};

module.exports = SlotTest;
