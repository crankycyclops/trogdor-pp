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
	 * Tests Dump.restore()
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

					// Restore the default slot (should be the latest with the meta key set)
					return dump.restore();
				}).then(() => {

					if (200 != connection.status) {
						reject(new Error("Restoring dumped game should have succeeded"));
					}

					return game.getMeta(['key']);
				}).then(data => {

					if (200 != connection.status) {
						reject(new Error("Restoring dumped game should have succeeded"));
					}

					if ('value' != data['key']) {
						reject(new Error('The latest version of the game should have been restored, but the earlier one was restored instead'));
					}

					return game.destroy(false);
				}).then(() => {

					// Restore the earlier version of the game
					return dump.restore(0);
				}).then(() => {

					return game.getMeta(['key']);
				}).then(data => {

					if ('' != data['key']) {
						reject(new Error('The earlier version of the game should have been restored, but the later one was restored instead'));
					}

					// Clean up (destroys game along with the dump)
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
	 * Tests Dump.slots() and Dump.getSlot()
	 */
	 #testSlotsAndGetSlot = function () {

		return new Promise((resolve, reject) => {

			let game, dump, slots;

			const gameName = "My Game";
			const definition = "game.xml";

			const connection = new Trogdord();

			connection.on('connect', () => {

				connection.newGame(gameName, definition).then(newGame => {

					game = newGame;
					return game.dump();
				}).then(newDump => {

					dump = newDump;
					return dump.slots();
				}).then(result => {

					slots = result;

					if (!Array.isArray(slots)) {
						reject(new Error("Dump.slots() should resolve to an array but doesn't"));
					}

					// We only dumped once, so there should only be one slot
					if (slots.length != 1) {
						reject(new Error("There should be exactly one dump slot, but more or less were found"));
					}

					if ('undefined' == typeof slots[0].slot || !Number.isInteger(slots[0].slot) || slots[0].slot < 0) {
						reject(new Error("Every slot in the list must have a slot member with an unsigned integral value"));
					}

					if ('undefined' == typeof slots[0].timestamp || !Number.isInteger(slots[0].timestamp) || slots[0].timestamp < 0) {
						reject(new Error("Every slot in the list must have a timestamp member with an unsigned integral value"));
					}

					if (slots[0].dump != dump) {
						reject(new Error("slot.dump must contain a reference to a parent instance of Dump"));
					}

					return dump.getSlot(slots[0].slot);
				}).catch(error => {

					reject(error);
				}).then(slot => {

					if ('undefined' == typeof slot.slot || !Number.isInteger(slot.slot) || slot.slot < 0) {
						reject(new Error("Slot must have a slot member with an unsigned integral value"));
					}

					if ('undefined' == typeof slot.timestamp || !Number.isInteger(slot.timestamp) || slot.timestamp < 0) {
						reject(new Error("Slot must have a timestamp member with an unsigned integral value"));
					}

					if (slot.dump != dump) {
						reject(new Error("slot.dump must contain a reference to a parent instance of Dump"));
					}

					if (slot.slot != slots[0].slot) {
						reject(new Error("The result from Dump.slot() doesn't match the object returned by Dump.getSlot()"));
					}

					// Attempt to get a slot that doesn't exist and demonstrate that it fails
					return dump.getSlot(slots[0].slot + 1);
				}).then(() => {

					reject(new Error("Call to Dump.getSlot() should not exist with the slot doesn't exist"));
				}).catch(error => {

					if (404 != connection.status) {
						reject(new Error("Getting non-existent slot should return 404 slot not found but returned a different status"));
					}

					// Cleanup
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
			this.addTest("Dump.restore()", this.#testRestore);
			this.addTest("Dump.slots() and Dump.getSlot()", this.#testSlotsAndGetSlot);
			this.addTest("Dump Getters for id, name, definition, created, and trogdord", this.#testGetters);
			resolve();
		});
	}
};

module.exports = DumpTest;
