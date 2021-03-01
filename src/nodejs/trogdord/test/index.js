const fs = require('fs');
const path = require('path');

const casesPath = path.dirname(__filename) + path.sep + 'cases';
const casesDir = fs.opendirSync(casesPath);

// Number of tests that failed
let failed = 0;

// Each test is executed asynchronously as a Promise
let tests = [];

console.log("Running nodejs/trogdord unit tests:\n");

for (let entry = casesDir.readSync(); entry != null; entry = casesDir.readSync()) {

	if (entry.isFile() && 'js' == entry.name.split('.').slice(1).join()) {

		let test = require(casesPath + path.sep + entry.name);
		let instance = new test();

		tests.push(
			new Promise((resolve, reject) => {
				instance.skip().then(() => {
					console.log(entry.name + ': SKIPPED');
					resolve();
				}).catch(e => {
					instance.init().then(() => {return instance.run();}).then(result => {
						console.log(entry.name + ': PASSED');
						resolve();
					}).catch(error => {
						console.log(entry.name + ': FAILED');
						console.error("\t" + ("string" == typeof error ? error : entry.name + ':' + error.stack.split("\n")[1].split(':')[1] + ': ' + error.message));
						failed++;
						resolve();
					});
				});
			})
		);
	}
}

Promise.allSettled(tests).then(result => {
	casesDir.closeSync();
	process.exit(failed);
}).catch(error => {
	casesDir.closeSync();
	console.error(error);
	process.exit(1);
});
