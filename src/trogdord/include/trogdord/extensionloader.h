#ifndef EXTENSION_LOADER_H
#define EXTENSION_LOADER_H

#include <memory>
#include <vector>
#include <unordered_map>


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
