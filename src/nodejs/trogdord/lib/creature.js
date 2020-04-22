'use strict';

const Being = require('./being');

class Creature extends Being {

	/**
	 * Create a new object representing a thing inside of a trogdord game.
	 *
	 * @param {Object} game The game the creature belongs to
	 * @param {String} name The creature's name
	 */
	constructor(game, name) {

		super(game, name);
	}
};

module.exports = Creature;