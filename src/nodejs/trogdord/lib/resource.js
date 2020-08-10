'use strict';

const Entity = require('./entity');

class Resource extends Entity {

	/**
	 * Create a new object representing a resource inside of a trogdord game.
	 *
	 * @param {Object} game The game the resource belongs to
	 * @param {String} name The resource's name
	 */
	constructor(game, name) {

		super(game, name);
	}
};

module.exports = Resource;