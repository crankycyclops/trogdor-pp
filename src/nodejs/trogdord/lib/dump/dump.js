'use strict';

const Trogdord = require('../trogdord');

/**
 * Class representing a game dump that exists inside an instance of trogdord.
 */
class Dump {

	// The dump's numeric id
	#id;

	// The dumped game's name
	#name;

	// The dumped game's definition file
	#definition;

    // UNIX timestamp representing when the dump was created
    #created;

	// Instance of Trogdord that spawned this instance of Dump
	#trogdord;

	/**
	 * Create a new object representing a game dump inside an instance of trogdord.
	 *
	 * @param {Integer} id The dumped game's id
	 * @param {String} name The dumped game's name
	 * @param {String} definition The dumped game's definition filename
	 * @param {Integer} created UNIX timestamp representing when the dump was created
	 * @param {Object} trogdord Connection to the instance of trogdord the dump belongs to
	 */
	constructor(id, name, definition, created, trogdord) {

		this.#id = id;
		this.#name = name;
		this.#definition = definition;
		this.#created = created;
		this.#trogdord = trogdord;
	}
};

module.exports = Dump;
