'use strict';

const Case = require('./case');
const Trogdord = require('../../../lib/trogdord');


/**
 * Any unit tests that inherit from this class will be skipped if a connection
 * to an instance of trogdord cannot be established.
 */
class ConnectionRequired extends Case {

	/**
	 * The unit test is skipped if we can't connect to an instance of trogdord.
	 */
	skip() {

		return new Promise((resolve, reject) => {

			let connection = new Trogdord('localhost', 1040, {connectTimeout: 1000});

			connection.on('connect', () => {
				connection.close();
				reject('Connected');
			});

			connection.on('error', () => {
				resolve('Connection failed');
			});

			setTimeout(() => {
				resolve('Connection timed out');
			}, 1000);
		});
	}

	/**
	 * Remove any games or dumps left behind by a test case.
	 */
	cleanup() {

		return new Promise((resolve, reject) => {

			let promises = Promise.resolve();
			let connection = new Trogdord('localhost', 1040, {connectTimeout: 1000});

			connection.on('connect', () => {

				promises = connection.games().then(games => {

					games.forEach(game => {

						promises.then(() => {
							return game.destroy(true);
						}).then(() => {
							if (200 != connection.status) {
								reject("Failed to cleanup game id " + game.id);
							}
						});
					});

					promises.catch(error => {
						reject(error.message);
					});

					return connection.config();
				}).then(response => {

					if (response['state.enabled']) {

						return connection.dumped().then(dumped => {

							dumped.forEach(dump => {

								promises.then(() => {
									return dump.destroy();
								}).then(() => {
									if (200 != connection.status) {
										reject("Failed to cleanup dump id " + dump.id);
									}
								}).catch(error => {
									reject(error);
								});
							});
						});
					} else {
						return Promise.resolve();
					}
				}).catch(error => {

					reject(error);
				});

				promises.then(() => {
					resolve();
				});
			});

			connection.on('error', () => {
				reject('Connection failed');
			});
		});
	}

	/**
	 * Returns a promise that resolves to true if trogdord's configuration has
	 * state features turned on and false if not.
	 */
	 isStateEnabled() {

		return new Promise((resolve, reject) => {

			let connection = new Trogdord('localhost', 1040, {connectTimeout: 1000});

			connection.on('connect', () => {

				connection.config().then(response => {

					connection.close();
					resolve(response['state.enabled']);
				}).catch(error => {

					reject(error);
				});
			});

			connection.on('error', () => {
				reject('Connection failed');
			});
		});
	}
};

module.exports = ConnectionRequired;
