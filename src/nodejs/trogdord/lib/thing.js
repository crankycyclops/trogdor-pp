'use strict';

const Entity = require('./entity');

class Thing extends Entity {

	/**
	 * Create a new object representing a thing inside of a trogdord game.
	 *
	 * @param {Object} game The game the thing belongs to
	 * @param {String} name The thing's name
	 */
	constructor(game, name) {

		super(game, name);
	}
};

module.exports = Thing;