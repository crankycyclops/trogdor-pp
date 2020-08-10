'use strict';

const Tangible = require('./tangible');

class Place extends Tangible {

	/**
	 * Create a new object representing a place inside of a trogdord game.
	 *
	 * @param {Object} game The game the place belongs to
	 * @param {String} name The place's name
	 */
	constructor(game, name) {

		super(game, name);
	}
};

module.exports = Place;