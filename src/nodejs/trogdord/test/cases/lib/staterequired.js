'use strict';

const ConnectionRequired = require('./connectionrequired');
const Trogdord = require('../../../lib/trogdord');


/**
 * Any unit tests that inherit from this class will be skipped if the state
 * feature of the trogdord instance is disabled.
 */
class StateRequired extends ConnectionRequired {

	/**
	 * The unit test is skipped if we can't connect to an instance of trogdord.
	 */
	skip() {

		return new Promise((resolve, reject) => {

			super.skip().then(() => {

				resolve();
			}).catch(() => {

				return this.isStateEnabled().then(enabled => {

					if (enabled) {
						reject();
					} else {
						resolve();
					}
				}).catch(error => {

					// If there's an unexpected error, just skip the test
					resolve();
				});
			});
		});
	}
};

module.exports = StateRequired;
