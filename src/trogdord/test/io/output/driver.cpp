#include <doctest.h>
#include <trogdord/io/output/local.h>
#include <trogdord/exception/outputdrivernotfound.h>

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
}
