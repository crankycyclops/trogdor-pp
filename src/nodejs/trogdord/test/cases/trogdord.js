'use strict';

const ConnectionRequired = require('./lib/connectionrequired');

class Trogdord extends ConnectionRequired {

	run() {

		return new Promise((resolve, reject) => {
			console.log("Trogdord.js: TODO");
			resolve();
		});
	}
};

module.exports = Trogdord;
