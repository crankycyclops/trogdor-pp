#include <doctest.h>
#include <trogdord/extensionloader.h>

#include "config.h"


TEST_SUITE("ExtensionLoader (extensionloader.cpp)") {

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

			// TODO: write an output stream class I can use for testing that will
			// allow me to examine log messages. This will let me make sure the
			// right message is being logged about the extension being ignored.
			CHECK(false == ExtensionLoader::get()->load(extNoUsefulSym));

			initIniFile(iniFilename, {{}});
			STD_FILESYSTEM::remove_all(extPath);

		#endif
	}
}
