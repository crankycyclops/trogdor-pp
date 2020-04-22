'use strict';

const Thing = require('./thing');

// The 'T' in the name stands for Trogdord. I tried naming this class Object,
// but that didn't go so well :)
class TObject extends Thing {

	/**
	 * Create a new object representing an object inside of a trogdord game.
	 *
	 * @param {Object} game The game the object belongs to
	 * @param {String} name The object's name
	 */
	constructor(game, name) {

		super(game, name);
	}
};

module.exports = TObject;