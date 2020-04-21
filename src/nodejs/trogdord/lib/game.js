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

	// Instance of Trogdord that spawned this instance of Game
	#trogdord;

	/**
	 * Create a new object representing a game inside an instance of trogdord.
	 *
	 * @param {Integer} id The game's id
	 * @param {String} name The game's name
	 * @param {Object} trogdord Connection to the instance of trogdord the game belongs to
	 */
	constructor(id, name, trogdord) {

		this.#id = id;
		this.#name = name;
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
	 * Returns the connection to trogdord associated with the game.
	 */
	get trogdord() {

		return this.#trogdord;
	}
};

module.exports = Game;