
#include <trogdord/config.h>
#include <trogdord/dispatcher.h>
#include <trogdor/filesystem.h>
#include <trogdord/extensionloader.h>

#ifdef LIBDL
#include <dlfcn.h>
#endif


// Singleton instance of ExtensionLoader
std::unique_ptr<ExtensionLoader> ExtensionLoader::instance = nullptr;

/******************************************************************************/

ExtensionLoader::ExtensionLoader() {}

/******************************************************************************/

std::unique_ptr<ExtensionLoader> &ExtensionLoader::get() {

	if (!instance) {
		instance = std::unique_ptr<ExtensionLoader>(new ExtensionLoader());
	}

	return instance;
}

/******************************************************************************/

bool ExtensionLoader::loadScopes(
	const char *extension,
	bool &usefulSymbols,
	ScopeController **(*scopeLoader)()
) {

	if (scopeLoader != NULL) {

		// There was an error (very possibly due to unresolved symbols)
		if (dlerror() != NULL) {
			Config::get()->err(trogdor::Trogerr::ERROR) << "ExtensionLoader::load(): failed to load "
				<< extension << ": " << dlerror() << std::endl;
			return false;
		}

		else {

			ScopeController **scopes = scopeLoader();

			for (ScopeController **scope = scopes; *scope != nullptr; scope++) {

				if (!Dispatcher::get()->registerScope(*scope)) {
					Config::get()->err(trogdor::Trogerr::ERROR) << "ExtensionLoader::load(): failed to load "
						<< extension << " because it tried to register a scope (" << (*scope)->getName()
						<< ") that was already previously registered." << std::endl;
					return false;
				} else {
					usefulSymbols = true;
				}

				registeredScopes[extension].push_back((*scope)->getName());
			}
		}
	}

	return true;
}

/******************************************************************************/

bool ExtensionLoader::loadOutputDrivers(
	const char *extension,
	bool &usefulSymbols,
	output::Driver **(*driverLoader)()
) {

	if (driverLoader != NULL) {

		if (dlerror() != NULL) {
			Config::get()->err(trogdor::Trogerr::ERROR) << "ExtensionLoader::load(): failed to load "
				<< extension << ": " << dlerror() << std::endl;
			return false;
		}

		else {

			output::Driver **drivers = driverLoader();

			for (output::Driver **driver = drivers; *driver != nullptr; driver++) {

				if (!output::Driver::registerDriver(*driver)) {
					Config::get()->err(trogdor::Trogerr::ERROR) << "ExtensionLoader::load(): failed to load "
						<< extension << " because it tried to register an output driver (" << (*driver)->getName()
						<< ") that was already previously registered." << std::endl;
					return false;
				} else {
					usefulSymbols = true;
				}

				registeredOutputDrivers[extension].push_back((*driver)->getName());
			}
		}
	}

	return true;
}

/******************************************************************************/

bool ExtensionLoader::load(const char *extension) {

	std::unique_ptr<Config> &config = Config::get();

	#ifdef LIBDL

		void *handle = nullptr;
		bool usefulSymbols = false;

		if (handles.end() != handles.find(extension)) {
			config->err(trogdor::Trogerr::WARNING) << "ExtensionLoader::load(): "
				<< extension << " has already been loaded. Skipping." << std::endl;
			return true;
		}

		std::string extPath = config->getExtensionsPath() +
			STD_FILESYSTEM::path::preferred_separator + extension;

		if ( (handle = dlopen(extPath.c_str(), RTLD_NOW | RTLD_GLOBAL)) == nullptr) {
			config->err(trogdor::Trogerr::ERROR) << "ExtensionLoader::load(): failed to load "
				<< extension << ": " << dlerror() << std::endl;
			return false;
		}

		handles[extension] = handle;

		// Attempt to load scopes, if any
		if (!loadScopes(extension, usefulSymbols, reinterpret_cast<ScopeController** (*)()>(dlsym(handle, "loadScopes")))) {
			dlclose(handle);
			return false;
		}

		// Attempt to load output drivers, if any
		if (!loadOutputDrivers(extension, usefulSymbols, reinterpret_cast<output::Driver** (*)()>(dlsym(handle, "loadOutputDrivers")))) {
			dlclose(handle);
			return false;
		}

		if (!usefulSymbols) {
			dlclose(handle);
			config->err(trogdor::Trogerr::ERROR) << "ExtensionLoader::load(): "
				<< extension << " exported no useful symbols. Ignoring." << std::endl;
			return false;
		} else {
			config->err(trogdor::Trogerr::INFO) << "ExtensionLoader::load(): "
				<< extension << " loaded." << std::endl;
			return true;
		}

	#else

		config->err(trogdor::Trogerr::ERROR) << "ExtensionLoader::load(): "
			<< extension << " cannot be loaded because extensions aren't supported on your platform."
			<< std::endl;
		return false;

	#endif // LIBDL
}

/******************************************************************************/

void ExtensionLoader::unload(const char *extension) {

	std::unique_ptr<Config> &config = Config::get();

	#ifdef LIBDL

		if (handles.end() == handles.find(extension)) {
			config->err(trogdor::Trogerr::WARNING) << "ExtensionLoader::unload(): "
				<< extension << " was never loaded. Skipping." << std::endl;
			return;
		}

		dlclose(handles[extension]);
		handles.erase(extension);

		// If any scopes were registered by the extension, remove them from the
		// dispatcher before unloading
		if (registeredScopes.end() != registeredScopes.find(extension)) {

			for (auto &scopeName: registeredScopes[extension]) {
				Dispatcher::get()->unregisterScope(scopeName);
			}

			registeredScopes.erase(extension);
		}

		// Also unregister any output drivers that the extension added
		if (registeredOutputDrivers.end() != registeredOutputDrivers.find(extension)) {

			for (auto &driverName: registeredOutputDrivers[extension]) {
				output::Driver::unregisterDriver(driverName);
			}

			registeredOutputDrivers.erase(extension);
		}

		config->err(trogdor::Trogerr::INFO) << "ExtensionLoader::unload(): "
			<< extension << " unloaded." << std::endl;

	#else

		config->err(trogdor::Trogerr::ERROR) << "ExtensionLoader::unload(): "
			<< "Cannot unload extension because extensions aren't supported on your platform."
			<< std::endl;
		return;

	#endif // LIBDL
}