#ifndef EXTENSION_LOADER_H
#define EXTENSION_LOADER_H

#include <memory>
#include <vector>
#include <unordered_map>

#include <trogdord/io/output/driver.h>

class ScopeController;


class ExtensionLoader {

	private:

		// Singleton instance of ExtensionLoader
		static std::unique_ptr<ExtensionLoader> instance;

		// Constructor should only be called internally by get(), which will
		// ensure we only ever have a single instance of the class.
		ExtensionLoader();
		ExtensionLoader(const ExtensionLoader &) = delete;

		// Tracks all opened extensions so that we can close them later.
		std::unordered_map<const char *, void *> handles;

		// Tracks which scopes were registered by which extensions so we can
		// remove them when the extension is unloaded.
		std::unordered_map<const char *, std::vector<std::string>> registeredScopes;

		// Tracks which output drivers were registered by which extensions so
		// we can remove them when the extension is unloaded.
		std::unordered_map<const char *, std::vector<std::string>> registeredOutputDrivers;

		// Takes as input the name of the extension, a reference to a bool that
		// gets set to true if any scopes were loaded, and a pointer to an
		// extension's scope loader function and attempts to load those scopes
		// into the singleton instance of Dispatcher. If loading fails, this
		// returns false. Otherwise, it returns true.
		bool loadScopes(
			const char *extension,
			bool &usefulSymbols,
			ScopeController **(*scopeLoader)()
		);

		// Takes as input the name of the extension, a reference to a bool that
		// gets set to true if any output drivers were loaded, and a pointer to
		// an extension's output driver loader function and attempts to register
		// it. If loading fails, this returns false. Otherwise, it returns
		// true.
		bool loadOutputDrivers(
			const char *extension,
			bool &usefulSymbols,
			output::Driver **(*driverLoader)()
		);

	public:

		// Returns singleton instance of ExtensionLoader.
		static std::unique_ptr<ExtensionLoader> &get();

		// Loads an extension. Returns true if load was successful (or if
		// the extension was already loaded) and false if not.
		bool load(const char *extension);

		// Unloads an extension
		void unload(const char *extension);
};


#endif
