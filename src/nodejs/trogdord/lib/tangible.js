'use strict';

const Entity = require('./entity');

class Tangible extends Entity {

	/**
	 * Create a new object representing a tangible inside of a trogdord game.
	 *
	 * @param {Object} game The game the tangible belongs to
	 * @param {String} name The tangible's name
	 */
	constructor(game, name) {

		super(game, name);
	}
};

module.exports = Tangible;