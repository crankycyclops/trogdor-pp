'use strict';

/**
 * Class representing a game dump that exists inside an instance of trogdord.
 */
class Dump {

	// Slot objects represent a dump slot that exists inside the trogdord instance
	#Slot = require('./slot');

	// Game objects represent games that exist inside the trogdord instance
	#Game = require('../game');

	// The dump's numeric id
	#id;

	// The dumped game's name
	#name;

	// The dumped game's definition file
	#definition;

	// UNIX timestamp representing when the game was created
	#created;

	// Instance of Trogdord that spawned this instance of Dump
	#trogdord;

	/**
	 * Create a new object representing a game dump inside an instance of trogdord.
	 *
	 * @param {Integer} id The dumped game's id
	 * @param {String} name The dumped game's name
	 * @param {String} definition The dumped game's definition filename
	 * @param {Integer} created UNIX timestamp representing when the game was created
	 * @param {Object} trogdord Connection to the instance of trogdord the dump belongs to
	 */
	constructor(id, name, definition, created, trogdord) {

		this.#id = id;
		this.#name = name;
		this.#definition = definition;
		this.#created = created;
		this.#trogdord = trogdord;
	}

	/**
	 * Returns the dumped game's id.
	 */
	get id() {

		return this.#id;
	}

	/**
	 * Returns the dumped game's name.
	 */
	get name() {

		return this.#name;
	}

	/**
	 * Returns the dumped game's definition.
	 */
	get definition() {

		return this.#definition;
	}

	/**
	 * Returns the UNIX timestamp for when the game was created.
	 */
	get created() {

		return this.#created;
	}

	/**
	 * Returns the connection to trogdord associated with the dump.
	 */
	get trogdord() {

		return this.#trogdord;
	}

	/**
	 * Destroys the dump, along with all its slots. From this point forward,
	 * the Dump object will be invalid and network requests that originate 
	 * from it will result in a 404 dump not found error.
	 */
	destroy() {

		return new Promise((resolve, reject) => {

			this.#trogdord.makeRequest({
				method: "delete",
				scope: "game",
				action: "dump",
				args: {id: this.#id}
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
	 * Restores the dump. Takes as input the slot to restore (if no slot is
	 * specified, the most recent one will be chosen) and returns an instance
	 * of Game representing the newly restored game.
	 * 
	 * @param {Integer} slot Which slot to restore (defaults to the most recent)
	 */
	restore(slot = null) {

		return new Promise((resolve, reject) => {

			let args = {id: this.#id};

			if (null !== slot) {

				if (!Number.isInteger(slot) || slot < 0) {
					throw new Error("Slot argument must be an integer greater than or equal to zero");
				}

				args.slot = slot;
			}

			this.#trogdord.makeRequest({
				method: "post",
				scope: "game",
				action: "restore",
				args: args
			}).then(response => {

				if (200 != response.status) {

					let error = new Error(response.message);

					error.status = response.status;
					reject(error);
				}

				resolve(new this.#Game(
					this.#id,
					this.#name,
					this.#definition,
					this.#created,
					this.#trogdord
				));

			}).catch(error => {
				reject(error);
			});
		});
	}

	/**
	 * Gets an object representing the specified dump slot.
	 * 
	 * @param {Integer} slot Which slot to get
	 */
	getSlot(slot) {

		return new Promise((resolve, reject) => {

			this.#trogdord.makeRequest({
				method: "get",
				scope: "game",
				action: "dump",
				args: {id: this.#id, slot: slot}
			}).then(response => {

				if (200 != response.status) {

					let error = new Error(response.message);

					error.status = response.status;
					reject(error);
				}

				resolve(new this.#Slot(slot, response.timestamp_ms, this));

			}).catch(error => {
				reject(error);
			});
		});
	}

	/**
	 * Gets an array of all contained dump slots.
	 */
	slots() {

		return new Promise((resolve, reject) => {

			this.#trogdord.makeRequest({
				method: "get",
				scope: "game",
				action: "dumplist",
				args: {id: this.#id}
			}).then(response => {

				if (200 != response.status) {

					let error = new Error(response.message);

					error.status = response.status;
					reject(error);
				}

				let slots = [];

				response.slots.forEach(slot => {
					slots.push(new this.#Slot(slot.slot, slot.timestamp_ms, this));
				});

				resolve(slots);

			}).catch(error => {
				reject(error);
			});
		});
	}
};

module.exports = Dump;
