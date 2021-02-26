const fs = require('fs');
const path = require('path');

const casesPath = path.dirname(__filename) + path.sep + 'cases';
const casesDir = fs.opendirSync(casesPath);

// Number of tests that failed
let failed = 0;

console.log("Running nodejs/trogdord unit tests:\n");

for (let entry = casesDir.readSync(); entry != null; entry = casesDir.readSync()) {

	if (entry.isFile() && 'js' == entry.name.split('.').slice(1).join()) {

		let test = require(casesPath + path.sep + entry.name);
		let instance = new test();

		if (instance.skip()) {
			console.log(entry.name + ': SKIPPED');
		}

		else {

			try {
				instance.run();
				console.log(entry.name + ': PASSED');
			}

			catch (e) {
				console.log(entry.name + ': FAILED');
				console.error("\t" + e.message);
				failed++;
			}
		}
	}
}

casesDir.closeSync();
process.exit(failed);
