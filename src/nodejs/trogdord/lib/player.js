'use strict';

const Being = require('./being');

class Player extends Being {

	/**
	 * Create a new object representing a player inside of a trogdord game.
	 *
	 * @param {Object} game The game the player belongs to
	 * @param {String} name The player's name
	 */
	constructor(game, name) {

		super(game, name);
	}

	/**
	 * Sends a command to a player's input stream and returns a promise that
	 * resolves into a success status.
	 *
	 * @param {String} command Command to send to the entity's input stream
	 */
	input(command) {

		return new Promise((resolve, reject) => {

			this.game.trogdord.makeRequest({
				method: "post",
				scope: "player",
				action: "input",
				args: {
					game_id: this.game.id,
					name: this.name,
					command: command
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

	destroy() {

		return new Promise((resolve, reject) => {

			this.game.trogdord.makeRequest({
				method: "delete",
				scope: "player",
				args: {
					game_id: this.game.id,
					name: this.name
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
};

module.exports = Player;