'use strict';

const Dump = require('./dump');

/**
 * Class representing a game dump slot that exists inside an instance of trogdord.
 */
class Slot {

    // The slot number
    #slot;

    // UNIX timestamp when the slot was created
    #timestamp;

    // Instance of Dump representing the dump the slot belongs to
    #dump;


	/**
	 * Create a new object representing a game dump slot inside an instance of trogdord.
	 *
	 * @param {Integer} slot The slot number
	 * @param {Integer} timestamp UNIX timestamp representing when the dump slot was created
	 * @param {Object} dump Object representing the dump the slot belongs to
	 */
	constructor(slot, timestamp, dump) {

		this.#slot = slot;
		this.#timestamp = timestamp;
		this.#dump = dump;
	}};

module.exports = Slot;
