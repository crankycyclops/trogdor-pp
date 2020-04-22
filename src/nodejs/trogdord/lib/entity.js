'use strict';

class Entity {

	// The game the entity belongs to
	#game;

	// The entity's name
	#name;

	/**
	 * Create a new object representing an entity inside of a trogdord game.
	 *
	 * @param {Object} game The game the entity belongs to
	 * @param {String} name The entity's name
	 */
	constructor(game, name) {

		this.#game = game;
		this.#name = name;
	}

	/**
	 * Returns the game the entity belongs to
	 */
	get game() {

		return this.#game;
	}

	/**
	 * Returns the entity's name
	 */
	get name() {

		return this.#name;
	}

	/**
	 * Sends a command to an entity's input stream and returns a promise that
	 * resolves into a success status.
	 *
	 * @param {String} command Command to send to the entity's input stream
	 */
	input(command) {

		return new Promise((resolve, reject) => {

			this.#game.trogdord.makeRequest({
				method: "post",
				scope: "entity",
				action: "input",
				args: {
					game_id: this.#game.id,
					name: this.#name,
					command: command
				}
			}).then((response) => {

				if (200 != response.status) {

					let error = new Error(response.message);

					error.status = response.status;
					reject(error);
				}

				resolve(response);

			}).catch((error) => {
				reject(error);
			});
		});
	}
};

module.exports = Entity;