'use strict';

const Place = require('./place');

class Room extends Place {

	/**
	 * Create a new object representing a room inside of a trogdord game.
	 *
	 * @param {Object} game The game the room belongs to
	 * @param {String} name The room's name
	 */
	constructor(game, name) {

		super(game, name);
	}
};

module.exports = Room;