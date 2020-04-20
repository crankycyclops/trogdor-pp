'use strict';

const Net = require('net');
const EventEmitter = require('events');

const {
	TCP_DELIMITER,
	ENOTCONN,
	ETIMEDOUT
} = require ('./constants');

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

	// Underlying socket connection
	#connection;

	/**
	 * Create a new connection to an instance of trogdord.
	 *
	 * @param {String} hostname Hostname pointing to the box trogdord is running on
	 * @param {Integer} port Port trogdord is listening on
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
		let onConnectionError = (error) => {

			clearTimeout(connectTimer);
			this.emit('error', error);
		};

		this.#connection.once('connect', () => {

			const dataChunks = [];

			// Error callback should only be used when attempt to connect
			// fails and shouldn't ever be called as the result of future
			// errors.
			this.#connection.removeListener('error', onConnectionError);

			let onData = (data) => {

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

			let onError = (error) => {
				this.#connection.setTimeout(0);
				reject(error);
			};

			let onData = (data) => {

				dataChunks.push(data);
				let dataStr = Buffer.concat(dataChunks).toString().trim();

				// Keep appending data to the buffer until we reach the end
				if (dataStr.indexOf(TCP_DELIMITER) != -1) {

					this.#connection.removeListener('data', onData);
					this.#connection.removeListener('error', onError);

					this.#connection.setTimeout(0);
					resolve(JSON.parse(dataStr.slice(0, -1)));
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
