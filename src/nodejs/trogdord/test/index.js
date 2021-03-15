const fs = require('fs');
const path = require('path');

const casesPath = path.dirname(__filename) + path.sep + 'cases';
const casesDir = fs.opendirSync(casesPath);

// Number of tests that failed
let failed = 0;

// When set to true, this signals that the tests are finished running
let finished = false;

// Promise chain that will execute our tests one by one
let run = new Promise((resolve, reject) => {

	console.log("Running nodejs/trogdord unit tests:\n");
	resolve();
});

for (let entry = casesDir.readSync(); entry != null; entry = casesDir.readSync()) {

	if (entry.isFile() && 'js' == entry.name.split('.').slice(1).join()) {

		let test = require(casesPath + path.sep + entry.name);
		let instance = new test();

		run = run.then(() => {

			return new Promise((resolve, reject) => {

				instance.skip().then(() => {
					console.log(entry.name + ': SKIPPED');
					resolve();
				}).catch(e => {
					instance.init().then(() => {
						return instance.run();
					}).then(result => {
						console.log(entry.name + ': PASSED');
					}).catch(error => {
						console.log(entry.name + ': FAILED');
						console.error(
							"\t While testing " + error.test + ": " + (
								"string" == typeof error.error ?
								error.error :
								entry.name + ':' + error.error.stack.split("\n")[1].split(':')[1] + ': ' + error.error.message
							)
						);
						failed++;
					}).then(() => {
						return instance.cleanup();
					}).then(() => {
						resolve();
					}).catch(error => {
						console.log("Cleanup failed due to error: " + error + ". Aborting remaining tests.");
						process.exit(1);
					});
				});
			});
		});
	}
}

run.finally(() => {
	finished = true;
});

// Wait for tests to finish
(function wait () {

	if (!finished) {
		setTimeout(wait, 250);
		return;
	}

	casesDir.closeSync();
	process.exit(failed);
})();
