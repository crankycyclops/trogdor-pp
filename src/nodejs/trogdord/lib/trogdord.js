'use strict';

const Net = require('net');
const EventEmitter = require('events');

const {
	TCP_DELIMITER,
	ENOTCONN,
	ETIMEDOUT
} = require('./constants');

// Default connection settings
const {
	DEFAULT_HOST,
	DEFAULT_PORT,
	DEFAULT_CONNECT_TIMEOUT_MS,
	DEFAULT_REQUEST_TIMEOUT_MS
} = require('./defaults');

/**
 * Class representing a connection to trogdord.
 */
class Trogdord extends EventEmitter {

	// Game objects represent games that exist inside the trogdord instance.
	// Only the Trogdord class should be able to instantiate this class, which
	// is why I've imported it as a private field.
	#Game = require('./game');

	// Game dump object represent dumps that exist inside the trogdord instance.
	// Like Game, only Trogdord should be able to instantiate this class.
	#Dump = require('./dump/dump');

	// Underlying socket connection
	#connection;

	// Stores the status of the last request
	#status = 0;

	/**
	 * Create a new connection to an instance of trogdord.
	 *
	 * @param {String} hostname Hostname pointing to the box trogdord is running on
	 * @param {Integer} port Port trogdord is listening on
	 * @param {Object} options connection options (optional)
	 */
	constructor(hostname = DEFAULT_HOST, port = DEFAULT_PORT, options = {}) {

		super();

		// This is how long we should spend attempting to establish a
		// connection before giving up
		let connectTimeout = options.connectTimeout ?
			options.connectTimeout : DEFAULT_CONNECT_TIMEOUT_MS;

		// Create socket and attempt to connect
		this.#connection = Net.connect({
			port: port,
			host: hostname
		});

		// If we don't receive our ready status from trogdord before this
		// timer runs out, we consider the connection timed out and report
		// the error
		let connectTimer = setTimeout(() => {
			this.#connection.destroy();
			this.emit('error', new Error(ETIMEDOUT));
		}, connectTimeout);

		// Fires when there's a problem connecting to trogdord
		let onConnectionError = error => {

			clearTimeout(connectTimer);
			this.emit('error', error);
		};

		this.#connection.once('connect', () => {

			const dataChunks = [];

			// Error callback should only be used when attempt to connect
			// fails and shouldn't ever be called as the result of future
			// errors.
			this.#connection.removeListener('error', onConnectionError);

			let onData = data => {

				dataChunks.push(data);
				let dataStr = Buffer.concat(dataChunks).toString().trim();

				// Keep appending data to the buffer until we reach the end
				if (dataStr.indexOf(TCP_DELIMITER) != -1) {

					this.#connection.removeListener('data', onData);

					// We've successfully connected and received data back from
					// whatever's listening on the specified host and port, so
					// we should clear the connection timeout callback.
					clearTimeout(connectTimer);

					try {

						// If I don't chop off that last null character at
						// the end of the stream, JSON.parse chokes.
						let status = JSON.parse(dataStr.slice(0, -1));

						if (!status || !status.status || status.status != 'ready') {
							socket.destroy();
							this.emit('error', new Error(ENOTCONN));
						}

						else {

							this.emit('connect');

							this.#connection.on('close', () => {
								this.emit('close');
							});
						}
					}

					catch (e) {
						this.#connection.destroy();
						this.emit('error', new Error(ENOTCONN));
					}
				}
			}
			// Make sure we received the appropriate response from trogdord
			// before considering the connection a success.
			this.#connection.on('data', onData);
		});

		this.#connection.once('error', onConnectionError);
	}

	/**
	 * Returns true if we're connected to a trogdord instance and false if not.
	 */
	get connected() {

		return !this.#connection.destroyed && !this.#connection.connecting ? true : false;
	}

	/**
	 * Returns the status of the last request.
	 */
	get status() {

		return this.#status;
	}

	/**
	 * Returns a promise that resolves to a JSON object detailing trogdord's
	 * configuration. Excludes sensitive settings that should not be made
	 * public.
	 */
	config() {

		return new Promise((resolve, reject) => {

			this.makeRequest({
				method: "get",
				scope: "global",
				action: "config"
			}).then(response => {

				if (200 != response.status) {

					let error = new Error(response.message);

					error.status = response.status;
					reject(error);
				}

				delete response.status;
				resolve(response.config);

			}).catch(error => {
				reject(error);
			});
		});
	}

	/**
	 * Returns a promise that resolves to a JSON object detailing useful
	 * statistical data.
	 */
	statistics() {

		return new Promise((resolve, reject) => {

			this.makeRequest({
				method: "get",
				scope: "global",
				action: "statistics"
			}).then(response => {

				if (200 != response.status) {

					let error = new Error(response.message);

					error.status = response.status;
					reject(error);
				}

				delete response.status;
				resolve(response);

			}).catch(error => {
				reject(error);
			});
		});
	}

	/**
	 * Returns a promise that resolves to a JSON object containing all
	 * dumped games that currently exist on the server.
	 */
	dumped() {

		return new Promise((resolve, reject) => {

			let data = {
				method: "get",
				scope: "game",
				action: "dumplist"
			};

			this.makeRequest(data).then(response => {

				if (200 != response.status) {

					let error = new Error(response.message);

					error.status = response.status;
					reject(error);
				}

				try {

					let games = [];

					response.games.forEach((game, index) => {
						games.push(new this.#Dump(game.id, game.name, game.definition, game.created, this));
					});

					resolve(games);
				}

				catch (e) {
					reject(e);
				}

			}).catch(error => {
				reject(error);
			});
		});
	}

	/**
	 * Returns a promise that resolves to an instance of the dump referenced by
	 * the given id.
	 * 
	 * @param {Integer} id The dumped game's id
	 */
	getDump(id) {

		return new Promise((resolve, reject) => {

			if (!id && 0 != id) {
				reject(new Error('passed invalid or undefined game id'));
			}

			this.makeRequest({
				method: "get",
				scope: "game",
				action: "dump",
				args: {id: id}
			}).then(response => {

				if (200 != response.status) {

					let error = new Error(response.message);

					error.status = response.status;
					reject(error);
				}

				resolve(new this.#Dump(response.id, response.name, response.definition, response.created, this));

			}).catch(error => {
				reject(error);
			});
		});
	}

	/**
	 * Dumps the server's state to disk, including all games.
	 */
	dump() {

		return new Promise((resolve, reject) => {

			this.makeRequest({
				method: "post",
				scope: "global",
				action: "dump"
			}).then(response => {

				if (200 != response.status) {

					let error = new Error(response.message);

					error.status = response.status;
					reject(error);
				}

				delete response.status;
				resolve(response);

			}).catch(error => {
				reject(error);
			});
		});
	}

	/**
	 * Restores the server's state from disk, including all games.
	 */
	restore() {

		return new Promise((resolve, reject) => {

			this.makeRequest({
				method: "post",
				scope: "global",
				action: "restore"
			}).then(response => {

				if (200 != response.status && 206 != response.status) {

					let error = new Error(response.message);

					error.status = response.status;
					reject(error);
				}

				delete response.status;
				resolve(response);

			}).catch(error => {
				reject(error);
			});
		});
	}

	/**
	 * Returns a promise that resolves to a JSON object containing a list of
	 * all available game definitions.
	 */
	definitions() {

		return new Promise((resolve, reject) => {

			this.makeRequest({
				method: "get",
				scope: "game",
				action: "definitions"
			}).then(response => {

				if (200 != response.status) {

					let error = new Error(response.message);

					error.status = response.status;
					reject(error);
				}

				delete response.status;
				resolve(response.definitions);

			}).catch(error => {
				reject(error);
			});
		});
	}

	/**
	 * Returns a promise that resolves to a JSON object containing all games
	 * that currently exist on the server.
	 * 
	 * @param {Object} filters A filter group or union used to return games matching specific criteria (optional)
	 */
	games(filters = null) {

		return new Promise((resolve, reject) => {

			let data = {
				method: "get",
				scope: "game",
				action: "list"
			};

			if (filters) {
				data.args = {};
				data.args.filters = filters;
			}

			this.makeRequest(data).then(response => {

				if (200 != response.status) {

					let error = new Error(response.message);

					error.status = response.status;
					reject(error);
				}

				try {

					let games = [];

					response.games.forEach((game, index) => {
						games.push(new this.#Game(
							game.id,
							game.name,
							game.definition,
							game.created,
							this
						));
					});

					resolve(games);
				}

				catch (e) {
					reject(e);
				}

			}).catch(error => {
				reject(error);
			});
		});
	}

	/**
	 * Returns a promise that resolves to an instance of the game referenced by
	 * the given id.
	 * 
	 * @param {Integer} id The game's id
	 */
	getGame(id) {

		return new Promise((resolve, reject) => {

			if (!id && 0 != id) {
				reject(new Error('passed invalid or undefined game id'));
			}

			this.makeRequest({
				method: "get",
				scope: "game",
				args: {id: id}
			}).then(response => {

				if (200 != response.status) {

					let error = new Error(response.message);

					error.status = response.status;
					reject(error);
				}

				resolve(new this.#Game(
					response.id,
					response.name,
					response.definition,
					response.created,
					this
				));

			}).catch(error => {
				reject(error);
			});
		});
	}

	/**
	 * Returns a promise that resolves to an instance of the new game.
	 * 
	 * @param {String} name The game's name
	 * @param {String} definition The game's definition file
	 * @param {Object} meta Any game meta data that should be set (optional)
	 */
	newGame(name, definition, meta = {}) {

		return new Promise((resolve, reject) => {

			name = name.trim();
			definition = definition.trim();

			if (!name) {
				reject(new Error('invalid or undefined name'));
			}

			else if (!definition) {
				reject(new Error('invalid or undefined game definition filename'));
			}

			let request = {
				method: "post",
				scope: "game",
				args: {
					name: name,
					definition: definition
				}
			};

			Object.keys(meta).forEach((key, index) => {
				request.args[key] = meta[key];
			});

			this.makeRequest(request)
			.then(response => {

				if (200 != response.status) {

					let error = new Error(response.message);

					error.status = response.status;
					reject(error);
				}

				resolve(new this.#Game(
					response.id,
					name,
					definition,
					response.created,
					this
				));

			}).catch(error => {
				reject(error);
			});
		});
	}

	/**
	 * Sends a request and returns a promise that resolves to a JSON response
	 * object.
	 * 
	 * @param {Object} request A JSON object representing a trogdord request.
	 * @param {Integer} timeout Number of milliseconds to wait before timing out (optional)
	 */
	makeRequest(request, timeout = DEFAULT_REQUEST_TIMEOUT_MS) {

		if (!this.connected) {
			throw new Error(ENOTCONN);
		}

		return new Promise((resolve, reject) => {

			const dataChunks = [];

			let onError = error => {
				this.#connection.setTimeout(0);
				reject(error);
			};

			let onData = data => {

				dataChunks.push(data);
				let dataStr = Buffer.concat(dataChunks).toString().trim();

				// Keep appending data to the buffer until we reach the end
				if (dataStr.indexOf(TCP_DELIMITER) != -1) {

					this.#connection.removeListener('data', onData);
					this.#connection.removeListener('error', onError);
					this.#connection.setTimeout(0);

					let jsonObj = JSON.parse(dataStr.slice(0, -1));

					this.#status = jsonObj.status;
					resolve(jsonObj);
				}
			};

			this.#connection.on('data', onData);
			this.#connection.once('error', onError);

			this.#connection.setTimeout(timeout);
			this.#connection.write(JSON.stringify(request) + TCP_DELIMITER);
		});
	}

	/**
	 * Closes the connection.
	 */
	close() {

		this.#connection.destroy();
	}
};

module.exports = Trogdord;
