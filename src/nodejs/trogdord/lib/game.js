'use strict';

const Trogdord = require('./trogdord');

/**
 * Class representing a game that exists inside an instance of trogdord.
 */
class Game {

	// Private class definitions for each entity type
	#EntityTypes = {
		room: require('./room'),
		object: require('./object'),
		creature: require('./creature'),
		player: require('./player')
	};

	// The game's numeric id
	#id;

	// The game's name
	#name;

	// The game's definition file
	#definition;

	// Instance of Trogdord that spawned this instance of Game
	#trogdord;

	/**
	 * Create a new object representing a game inside an instance of trogdord.
	 *
	 * @param {Integer} id The game's id
	 * @param {String} name The game's name
	 * @param {Object} trogdord Connection to the instance of trogdord the game belongs to
	 */
	constructor(id, name, definition, trogdord) {

		this.#id = id;
		this.#name = name;
		this.#definition = definition;
		this.#trogdord = trogdord;
	}

	/**
	 * Returns the game's id.
	 */
	get id() {

		return this.#id;
	}

	/**
	 * Returns the game's name.
	 */
	get name() {

		return this.#name;
	}

	/**
	 * Returns the game's definition.
	 */
	get definition() {

		return this.#definition;
	}

	/**
	 * Returns the connection to trogdord associated with the game.
	 */
	get trogdord() {

		return this.#trogdord;
	}

	/**
	 * Returns a promise that resolves to true if the game is running and false
	 * if it's not.
	 */
	isRunning() {

		return new Promise((resolve, reject) => {

			this.#trogdord.makeRequest({
				method: "get",
				scope: "game",
				action: "is_running",
				args: {id: this.#id}
			}).then((response) => {

				if (200 != response.status) {

					let error = new Error(response.message);

					error.status = response.status;
					reject(error);
				}

				resolve(response.is_running);

			}).catch((error) => {
				reject(error);
			});
		});
	}

	/**
	 * Returns a promise that resolves to the current time in the game.
	 */
	getTime() {

		return new Promise((resolve, reject) => {

			this.#trogdord.makeRequest({
				method: "get",
				scope: "game",
				action: "time",
				args: {id: this.#id}
			}).then((response) => {

				if (200 != response.status) {

					let error = new Error(response.message);

					error.status = response.status;
					reject(error);
				}

				resolve(response.current_time);

			}).catch((error) => {
				reject(error);
			});
		});
	}

	/**
	 * Starts the game.
	 */
	start() {

		return new Promise((resolve, reject) => {

			this.#trogdord.makeRequest({
				method: "set",
				scope: "game",
				action: "start",
				args: {id: this.#id}
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

	/**
	 * Stops the game.
	 */
	stop() {

		return new Promise((resolve, reject) => {

			this.#trogdord.makeRequest({
				method: "set",
				scope: "game",
				action: "stop",
				args: {id: this.#id}
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

	/**
	 * Destroys the game. From this point forward, the Game object will be
	 * invalid and network requests that originate from it will result in a
	 * 404 game not found error.
	 */
	destroy() {

		return new Promise((resolve, reject) => {

			this.#trogdord.makeRequest({
				method: "delete",
				scope: "game",
				args: {id: this.#id}
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

module.exports = Game;