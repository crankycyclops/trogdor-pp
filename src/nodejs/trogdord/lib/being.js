'use strict';

const Thing = require('./thing');

class Being extends Thing {

	/**
	 * Create a new object representing a thing inside of a trogdord game.
	 *
	 * @param {Object} game The game the being belongs to
	 * @param {String} name The being's name
	 */
	constructor(game, name) {

		super(game, name);
	}
};

module.exports = Being;