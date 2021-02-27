'use strict';

const Case = require('./case');
const Trogdord = require('../../../lib/trogdord');


// Any unit tests that inherit from this class will be skipped if a connection
// to an instance of trogdord cannot be established.
class ConnectionRequired extends Case {

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
};

module.exports = ConnectionRequired;