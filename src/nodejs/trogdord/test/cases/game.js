'use strict';

const ConnectionRequired = require('./lib/connectionrequired');

const Trogdord = require('../../lib/trogdord');
const Game = require('../../lib/game');

class GameTest extends ConnectionRequired {

    #testGetters = function () {

        return new Promise((resolve, reject) => {

            // TODO
            resolve();
        });
    }

	/**
	 * Initialize tests.
	 */
	init() {

		return new Promise((resolve, reject) => {

			this.addTest("Game Getters for id, name, definition, created, and trogdord", this.#testGetters);
			resolve();
		});
	}
};

module.exports = GameTest;
