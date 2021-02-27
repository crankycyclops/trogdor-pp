const fs = require('fs');
const path = require('path');

const casesPath = path.dirname(__filename) + path.sep + 'cases';
const casesDir = fs.opendirSync(casesPath);

// Number of tests that failed
let failed = 0;

// Each skip determination and test (if it's found that the test shouldn't be
// skipped) is executed asynchronously as a Promise
let jobs = [];

console.log("Running nodejs/trogdord unit tests:\n");

for (let entry = casesDir.readSync(); entry != null; entry = casesDir.readSync()) {

	if (entry.isFile() && 'js' == entry.name.split('.').slice(1).join()) {

		let test = require(casesPath + path.sep + entry.name);
		let instance = new test();

		jobs.push(
			instance.skip().then(() => {
				console.log(entry.name + ': SKIPPED');
			}).catch(e => {
				jobs.push(
					instance.run().then(result => {
						console.log(entry.name + ': PASSED');
					}).catch(error => {
						console.log(entry.name + ': FAILED');
						console.error("\t" + entry.name + ':' + e.lineNumber + ': ' + e.message);
						failed++;
					})
				);
			})
		);
	}
}

Promise.allSettled(jobs).then(result => {
	casesDir.closeSync();
	process.exit(failed);
}).catch(error => {
	casesDir.closeSync();
	console.error(error);
});
