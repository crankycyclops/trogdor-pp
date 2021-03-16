'use strict';

/**
 * Class representing a game dump slot that exists inside an instance of trogdord.
 */
class Slot {

	// Game objects represent games that exist inside the trogdord instance
	#Game = require('../game');

	// The slot number
	#slot;

	// UNIX timestamp when the slot was created
	#timestamp;

	// Instance of Dump representing the dump the slot belongs to
	#dump;

	/**
	 * Create a new object representing a game dump slot inside an instance of trogdord.
	 *
	 * @param {Integer} slot The slot number
	 * @param {Integer} timestamp UNIX timestamp representing when the dump slot was created
	 * @param {Object} dump Object representing the dump the slot belongs to
	 */
	constructor(slot, timestamp, dump) {

		this.#slot = slot;
		this.#timestamp = timestamp;
		this.#dump = dump;
	}

	/**
	 * Returns the slot's number.
	 */
	get slot() {

		return this.#slot;
	}

	/**
	 * Returns the timestamp (in milliseconds) for when the slot was created.
	 */
	get timestamp() {

		return this.#timestamp;
	}

	/**
	 * Returns the dump object the slot belongs to.
	 */
	get dump() {

		return this.#dump;
	}

	/**
	 * Destroys the dump slot. From this point forward, the Dump object will be
	 * invalid and network requests that originate  from it will result in a
	 * 404 dump not found error.
	 */
	 destroy() {

		return new Promise((resolve, reject) => {

			this.#dump.trogdord.makeRequest({
				method: "delete",
				scope: "game",
				action: "dump",
				args: {id: this.#dump.id, slot: this.#slot}
			}).then(response => {

				if (200 != response.status) {

					let error = new Error(response.message);

					error.status = response.status;
					reject(error);
				}

				resolve(response);

			}).catch(error => {
				reject(error);
			});
		});
	}

	/**
	 * Restores the dump slotand returns an instance of Game representing the
	 * newly restored game.
	 */
	restore() {

		return new Promise((resolve, reject) => {

			this.#dump.trogdord.makeRequest({
				method: "post",
				scope: "game",
				action: "restore",
				args: {id: this.#dump.id, slot: this.#slot}
			}).then(response => {

				if (200 != response.status) {

					let error = new Error(response.message);

					error.status = response.status;
					reject(error);
				}

				resolve(new this.#Game(
					this.#dump.id,
					this.#dump.name,
					this.#dump.definition,
					this.#dump.created,
					this.#dump.trogdord
				));

			}).catch(error => {
				reject(error);
			});
		});
	}
};

module.exports = Slot;
