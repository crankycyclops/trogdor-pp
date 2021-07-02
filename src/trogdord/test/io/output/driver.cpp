#include <doctest.h>

#include <trogdord/config.h>
#include <trogdord/filesystem.h>
#include <trogdord/io/output/local.h>
#include <trogdord/exception/outputdrivernotfound.h>

#include "../../config.h"
#include "../../mock/mockoutputdriver.h"


TEST_SUITE("output::Driver (io/output/driver.cpp)") {

	TEST_CASE("output::Driver (io/output/driver.cpp): get(), registerDriver(), and unregisterDriver()") {

		// Verify that trying to unregister a non-existent driver doesn't work
		CHECK(false == output::Driver::unregisterDriver("NotADriver"));

		// Verify that trying to unregister a built-in driver doesn't work
		CHECK(false == output::Driver::unregisterDriver(output::Local::DRIVER_NAME));

		try {
			output::Driver::get(output::MockOutput::DRIVER_NAME);
			FAIL("Mock output driver shouldn't have been registered before call to output::Driver::registerDriver()");
		} catch (const OutputDriverNotFound &e) {}

		// Register the driver and then verify that it has, in fact, been registered
		output::Driver::registerDriver(output::MockOutput::get().get());

		try {
			CHECK(0 == std::string(output::Driver::get(output::MockOutput::DRIVER_NAME)->getName()).compare(output::MockOutput::DRIVER_NAME));
		} catch (const OutputDriverNotFound &e) {
			FAIL("Mock output driver should have been registered but wasn't");
		}

		// Show that unregistering the mock driver is successful (and that it can't be
		// unregistered a second time)
		CHECK(true == output::Driver::unregisterDriver(output::MockOutput::DRIVER_NAME));
		CHECK(false == output::Driver::unregisterDriver(output::MockOutput::DRIVER_NAME));
	}

	TEST_CASE("output::Driver (io/output/driver.cpp): unregisterDriver() when the output driver was configured to be used in trogdord.ini") {

		// Configure trogdord.ini to use the mock output driver
		std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
		initIniFile(iniFilename, {{Config::CONFIG_KEY_OUTPUT_DRIVER, "mock"}});

		try {
			output::Driver::get(output::MockOutput::DRIVER_NAME);
			FAIL("Mock output driver shouldn't have been registered before call to output::Driver::registerDriver()");
		} catch (const OutputDriverNotFound &e) {}

		// Register the driver and then verify that it has, in fact, been registered
		output::Driver::registerDriver(output::MockOutput::get().get());

		try {
			CHECK(0 == std::string(output::Driver::get(output::MockOutput::DRIVER_NAME)->getName()).compare(output::MockOutput::DRIVER_NAME));
		} catch (const OutputDriverNotFound &e) {
			FAIL("Mock output driver should have been registered but wasn't");
		}

		// Unregister the driver and confirm that it resets the value in
		// trogdord.ini back to its default
		CHECK(true == output::Driver::unregisterDriver(output::MockOutput::DRIVER_NAME));
		CHECK(0 == Config::DEFAULTS.find(Config::CONFIG_KEY_OUTPUT_DRIVER)->second.compare(Config::get()->getString(Config::CONFIG_KEY_OUTPUT_DRIVER)));

		initIniFile(iniFilename, {{}});
		STD_FILESYSTEM::remove_all(iniFilename);
	}
}
