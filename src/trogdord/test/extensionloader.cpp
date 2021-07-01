#include <doctest.h>
#include <trogdord/extensionloader.h>

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

			const char *extNoUsefulSym = "test_trogdord_nousefulsymbols.so";
			std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";
			std::string extPath = STD_FILESYSTEM::temp_directory_path().string() +
				STD_FILESYSTEM::path::preferred_separator + "extensions";

			initIniFile(iniFilename, {{Config::CONFIG_KEY_EXTENSIONS_PATH, "/tmp/extensions"}});

			STD_FILESYSTEM::create_directory(extPath);
			STD_FILESYSTEM::copy(
				extNoUsefulSym,
				extPath + STD_FILESYSTEM::path::preferred_separator + extNoUsefulSym
			);

			// TODO: I need some way to inspect the log to verify that this
			// fails due to the extension not exporting useful symbols
			CHECK(false == ExtensionLoader::get()->load(extNoUsefulSym));

			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(extPath);

		#endif
	}
}
