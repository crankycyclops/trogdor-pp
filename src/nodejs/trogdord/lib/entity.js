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
	 * Either sends a message to an Entity's output stream on the specified
	 * channel or retrieves all messages currently in the Entity's output
	 * stream on the specified channel. If the output driver trogdord is
	 * configured to use doesn't support retrieving messages (this is the case
	 * with the redis driver) the returned promise will be rejected.
	 *
	 * @param {String} channel The output channel we should send a message to or retrieve messages from
	 * @param {String} message The message to send to the Entity's output channel (not specifying this second argument will result in a retrieval of existing messages)
	 */
	output(channel, message = false) {

		// We're retrieving messages
		if (false === message) {

			return new Promise((resolve, reject) => {

				this.#game.trogdord.makeRequest({
					method: "get",
					scope: "entity",
					action: "output",
					args: {
						game_id: this.#game.id,
						name: this.#name,
						channel: channel
					}
				}).then(response => {
	
					if (200 != response.status) {
	
						let error = new Error(response.message);
	
						error.status = response.status;
						reject(error);
					}
	
					resolve(response.messages);
	
				}).catch(error => {
					reject(error);
				});
			});
		}

		// We're appending a new message
		else {

			return new Promise((resolve, reject) => {

				this.#game.trogdord.makeRequest({
					method: "post",
					scope: "entity",
					action: "output",
					args: {
						game_id: this.#game.id,
						name: this.#name,
						channel: channel,
						message: message
					}
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
	}
};

module.exports = Entity;