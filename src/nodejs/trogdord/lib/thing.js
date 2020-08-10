'use strict';

const Tangible = require('./tangible');

class Thing extends Tangible {

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