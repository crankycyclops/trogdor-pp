'use strict';

const ConnectionRequired = require('./lib/connectionrequired');

const Trogdord = require('../../lib/trogdord');
const Game = require('../../lib/game');

class GameTest extends ConnectionRequired {

	/**
	 * Tests Game.statistics().
	 */
	#testStatistics = function () {

		return new Promise((resolve, reject) => {

			const gameName = "My Game";
			const definition = "game.xml";

			const connection = new Trogdord();

			connection.on('connect', () => {

				// TODO
				resolve();
			});

			connection.on('error', (e) => {
				reject(new Error(e.message));
			});
		});
	}

	/**
	 * Tests various getters for the Game class.
	 */
	#testGetters = function () {

		return new Promise((resolve, reject) => {

			const gameName = "My Game";
			const definition = "game.xml";

			const connection = new Trogdord();

			connection.on('connect', () => {

				connection.newGame(gameName, definition).then(game => {

					if (!Number.isInteger(game.id) || game.id < 0) {
						reject(new Error('Value of Game.id is invalid'));
					}

					if (gameName != game.name) {
						reject(new Error('Value of Game.name is invalid'));
					}

					if (definition != game.definition) {
						reject(new Error('Value of Game.definition is invalid'));
					}

					if (!Number.isInteger(game.created) || game.created < 0) {
						reject(new Error('Value of Game.created is invalid'));
					}

					if (connection != game.trogdord) {
						reject(new Error('Value of Game.trogdord is invalid'));
					}

					// Clean up
					return game.destroy();
				}).then(response => {

					resolve();
				}).catch(error => {

					reject(error);
				});
			});

			connection.on('error', (e) => {
				reject(new Error(e.message));
			});
		});
	}

	/**
	 * Initialize tests.
	 */
	init() {

		return new Promise((resolve, reject) => {

			this.addTest("Game.statistics()", this.#testStatistics);
			this.addTest("Game Getters for id, name, definition, created, and trogdord", this.#testGetters);
			resolve();
		});
	}
};

module.exports = GameTest;
