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

			let game;

			const gameName = "My Game";
			const definition = "game.xml";

			const connection = new Trogdord();

			connection.on('connect', () => {

				connection.newGame(gameName, definition).then(newGame => {

					game = newGame;
					return newGame.statistics();
				}).then(response => {

					const validKeys = ['created', 'players', 'current_time', 'is_running'];

					let responseKeys = Object.keys(response);
					let keysFound = [];

					if (200 != connection.status) {
						reject(new Error("Request should have returned 200 status but did not"));
					}

					if ('object' != typeof response) {
						reject(new Error("Response should be of type 'object' but isn't"));
					}

					for (let i = 0; i < responseKeys.length; i++) {
						if (!validKeys.includes(responseKeys[i])) {
							reject(new Error("Found unexpected key '" + responseKeys[i] + "' in response. Likely, the unit tests need to be updated."));
						} else {
							keysFound.push(responseKeys[i]);
						}
					}

					let difference = validKeys.filter(value => !responseKeys.includes(value));

					if (difference.length) {
						reject(new Error("Response is missing required keys: " + difference.toString()));
					}

					// TOOD: this should be a UNIX timestamp
					if (isNaN(Date.parse(response.created))) {
						reject(new Error('response.created should be a string that can be parsed into a timestamp'));
					}

					if (!Number.isInteger(response.players)) {
						reject(new Error("response.players should be an integer but isn't"));
					}

					if (!Number.isInteger(response.current_time)) {
						reject(new Error("response.current_time should be an integer but isn't"));
					}

					if (response.current_time < 0) {
						reject(new Error("response.current_time should be unsigned but isn't"));
					}

					if ('boolean' != typeof(response.is_running)) {
						reject(new Error("response.is_running should be a boolean value but isn't"));
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
