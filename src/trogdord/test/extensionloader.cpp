#include <doctest.h>
#include <trogdord/json.h>
#include <trogdord/dispatcher.h>
#include <trogdord/extensionloader.h>
#include <trogdord/exception/outputdrivernotfound.h>

#include "config.h"


TEST_SUITE("ExtensionLoader (extensionloader.cpp)") {

	TEST_CASE("ExtensionLoader (extensionloader.cpp): Extension doesn't exist") {

		#ifdef LIBDL

			const char *extension = "doesnt_exist.so";
			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";

			initIniFile(iniFilename, {{Config::CONFIG_KEY_EXTENSIONS_PATH, "/tmp/extensions"}});

			// TODO: I need some way to inspect the log to verify that this
			// fails due to the file not existing
			CHECK(false == ExtensionLoader::get()->load(extension));

			initIniFile(iniFilename, {{}});

		#endif
	}

	TEST_CASE("ExtensionLoader (extensionloader.cpp): Trying to load something that isn't an extension") {

		#ifdef LIBDL

			// I'm going to try to load the unit test binary as an extension.
			// This should most certainly fail ;)
			const char *extension = "test_trogdord";
			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::string extPath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "extensions";

			initIniFile(iniFilename, {{Config::CONFIG_KEY_EXTENSIONS_PATH, "/tmp/extensions"}});

			STD_FILESYSTEM::create_directory(extPath);
			STD_FILESYSTEM::copy(
				extension,
				extPath + STD_FILESYSTEM::path::preferred_separator + extension
			);

			// TODO: I need some way to inspect the log to verify that this
			// fails due to an incorrect file format and not some other issue
			CHECK(false == ExtensionLoader::get()->load(extension));

			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(extPath);

		#endif
	}

	TEST_CASE("ExtensionLoader (extensionloader.cpp): Loading extension with no useful symbols") {

		#ifdef LIBDL

			// This extension is built as a dependency to the test_trogdord build target
			const char *extension = "test_trogdord_nousefulsymbols.so";
			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::string extPath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "extensions";

			initIniFile(iniFilename, {{Config::CONFIG_KEY_EXTENSIONS_PATH, "/tmp/extensions"}});

			STD_FILESYSTEM::create_directory(extPath);
			STD_FILESYSTEM::copy(
				extension,
				extPath + STD_FILESYSTEM::path::preferred_separator + extension
			);

			// TODO: I need some way to inspect the log to verify that this
			// fails due to the extension not exporting useful symbols
			CHECK(false == ExtensionLoader::get()->load(extension));

			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(extPath);

		#endif
	}

	TEST_CASE("ExtensionLoader (extensionloader.cpp): Loading extension that tries to overwrite builtin scope") {

		#ifdef LIBDL

			// This extension is built as a dependency to the test_trogdord build target
			const char *extension = "test_trogdord_duplicatescope.so";
			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::string extPath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "extensions";

			initIniFile(iniFilename, {{Config::CONFIG_KEY_EXTENSIONS_PATH, "/tmp/extensions"}});

			STD_FILESYSTEM::create_directory(extPath);
			STD_FILESYSTEM::copy(
				extension,
				extPath + STD_FILESYSTEM::path::preferred_separator + extension
			);

			// TODO: I need some way to inspect the log to verify that this
			// fails due to an exported duplicate scope
			CHECK(false == ExtensionLoader::get()->load(extension));

			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(extPath);

		#endif
	}

	TEST_CASE("ExtensionLoader (extensionloader.cpp): Loading extension that tries to overwrite builtin output driver") {

		#ifdef LIBDL

			// This extension is built as a dependency to the test_trogdord build target
			const char *extension = "test_trogdord_duplicateoutputdriver.so";
			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::string extPath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "extensions";

			initIniFile(iniFilename, {{Config::CONFIG_KEY_EXTENSIONS_PATH, "/tmp/extensions"}});

			STD_FILESYSTEM::create_directory(extPath);
			STD_FILESYSTEM::copy(
				extension,
				extPath + STD_FILESYSTEM::path::preferred_separator + extension
			);

			// Getting for the first time triggers Driver to instantiate its
			// built-in drivers
			output::Driver::get("local");

			// TODO: I need some way to inspect the log to verify that this
			// fails due to an exported duplicate output driver
			CHECK(false == ExtensionLoader::get()->load(extension));

			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(extPath);

		#endif
	}

	TEST_CASE("ExtensionLoader (extensionloader.cpp): Loading extension that exports a custom output driver") {

		#ifdef LIBDL

			// This extension is built as a dependency to the test_trogdord build target
			const char *extension = "test_trogdord_newoutputdriver.so";
			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::string extPath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "extensions";

			initIniFile(iniFilename, {{Config::CONFIG_KEY_EXTENSIONS_PATH, "/tmp/extensions"}});

			STD_FILESYSTEM::create_directory(extPath);
			STD_FILESYSTEM::copy(
				extension,
				extPath + STD_FILESYSTEM::path::preferred_separator + extension
			);

			// Getting for the first time triggers Driver to instantiate its
			// built-in drivers
			output::Driver::get("local");

			CHECK(true == ExtensionLoader::get()->load(extension));

			try {
				output::Driver::get("test");
			} catch (const OutputDriverNotFound &e) {
				FAIL("Output driver 'test' should have been loaded but was not");
			}

			ExtensionLoader::get()->unload(extension);

			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(extPath);

		#endif
	}

	TEST_CASE("ExtensionLoader (extensionloader.cpp): Loading extension that exports a custom scope, then unloading it") {

		#ifdef LIBDL

			std::shared_ptr<TCPConnection> dummyConnection = nullptr;

			// This extension is built as a dependency to the test_trogdord build target
			const char *extension = "test_trogdord_hello.so";
			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::string extPath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "extensions";

			initIniFile(iniFilename, {{Config::CONFIG_KEY_EXTENSIONS_PATH, "/tmp/extensions"}});

			STD_FILESYSTEM::create_directory(extPath);
			STD_FILESYSTEM::copy(
				extension,
				extPath + STD_FILESYSTEM::path::preferred_separator + extension
			);

			// Calling this once ensures that I've loaded 
			Dispatcher::get();

			rapidjson::Document response;

			// Demonstrate that the extension's exported scope doesn't exist yet
			response.Parse(Dispatcher::get()->dispatch(
				dummyConnection,
				"{\"method\":\"get\",\"scope\":\"hello\"}"
			).c_str());

			CHECK(response.HasMember("status"));
			CHECK(404 == response["status"].GetInt());

			CHECK(true == ExtensionLoader::get()->load(extension));

			// Now, the scope should exist and the dispatcher should find it
			response.Parse(Dispatcher::get()->dispatch(
				dummyConnection,
				"{\"method\":\"get\",\"scope\":\"hello\"}"
			).c_str());

			CHECK(response.HasMember("status"));
			CHECK(200 == response["status"].GetInt());
			CHECK(response.HasMember("message"));
			CHECK(0 == std::string("Hello, world!").compare(response["message"].GetString()));

			ExtensionLoader::get()->unload(extension);

			// Finally, after unloading the extension, show that the scope is
			// no longer found
			response.Parse(Dispatcher::get()->dispatch(
				dummyConnection,
				"{\"method\":\"get\",\"scope\":\"hello\"}"
			).c_str());

			CHECK(response.HasMember("status"));
			CHECK(404 == response["status"].GetInt());

			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(extPath);

		#endif
	}

	TEST_CASE("ExtensionLoader (extensionloader.cpp): Unloading extension that was never loaded") {

		#ifdef LIBDL

			// This extension is built as a dependency to the test_trogdord build target
			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::string extPath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "extensions";

			initIniFile(iniFilename, {});

			STD_FILESYSTEM::create_directory(extPath);

			// TODO: I have to check the error log to ensure we got a warning
			ExtensionLoader::get()->unload("NotAnExtension.so");

			STD_FILESYSTEM::remove_all(extPath);

		#endif
	}

	TEST_CASE("ExtensionLoader (extensionloader.cpp): Unloading extension that was loaded and exported an output driver") {

		#ifdef LIBDL

			// This extension is built as a dependency to the test_trogdord build target
			const char *extension = "test_trogdord_newoutputdriver.so";
			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::string extPath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "extensions";

			initIniFile(iniFilename, {{Config::CONFIG_KEY_EXTENSIONS_PATH, "/tmp/extensions"}});

			STD_FILESYSTEM::create_directory(extPath);
			STD_FILESYSTEM::copy(
				extension,
				extPath + STD_FILESYSTEM::path::preferred_separator + extension
			);

			// Getting for the first time triggers Driver to instantiate its
			// built-in drivers
			output::Driver::get("local");

			CHECK(true == ExtensionLoader::get()->load(extension));

			try {
				output::Driver::get("test");
			} catch (const OutputDriverNotFound &e) {
				FAIL("Output driver 'test' should have been loaded but was not");
			}

			ExtensionLoader::get()->unload(extension);

			try {
				output::Driver::get("test");
				FAIL("Output driver 'test' should have been unloaded but was not");
			} catch (const OutputDriverNotFound &e) {}

			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(extPath);

		#endif
	}
}
