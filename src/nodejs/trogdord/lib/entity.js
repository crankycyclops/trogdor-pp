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
};

module.exports = Entity;