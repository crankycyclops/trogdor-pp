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
};

module.exports = Player;