#include <trogdord/io/output/driver.h>

namespace output {


	class Local: public Driver {

		private:

			// Singleton instance of the Local driver
			static std::unique_ptr<Local> instance;

		protected:

			// Construction should only occur through get()
			Local();
			Local(const Local &) = delete;

		public:

			// String representation of the driver's name.
			static const char *DRIVER_NAME;

			// Returns singleton instance of the Local output driver.
			static std::unique_ptr<Local> &get();

			// Destructor
			virtual ~Local();

			// Returns the driver's name
			virtual const char *getName();

			virtual size_t size(
				size_t gameId,
				std::string entityName,
				std::string channel
			);

			virtual void push(
				size_t gameId,
				std::string entityName,
				std::string channel,
				Message message
			);

			virtual std::optional<Message> pop(
				size_t gameId,
				std::string entityName,
				std::string channel
			);
	};

	/************************************************************************/
	/************************************************************************/

	// String representation of the driver's name
	const char *Local::DRIVER_NAME = "local";

	// Singleton instance of the Local driver
	std::unique_ptr<Local> Local::instance = nullptr;

	/************************************************************************/

	/*
		Fill this in if you need the constructor to do anything.
	*/
	Local::Local() {}

	/************************************************************************/

	/*
		Likewise, if you need the destructor to do anything, fill this in.
	*/
	Local::~Local() {}

	/************************************************************************/

	std::unique_ptr<Local> &Local::get() {

		if (!instance) {
			instance = std::unique_ptr<Local>(new Local());
		}

		return instance;
	}

	/************************************************************************/

	const char *Local::getName() {

		return DRIVER_NAME;
	}

	/************************************************************************/

	size_t Local::size(
		size_t gameId,
		std::string entityName,
		std::string channel
	) {

		return 0;
	}

	/************************************************************************/

	std::optional<Message> Local::pop(
		size_t gameId,
		std::string entityName,
		std::string channel
	) {

		return std::nullopt;
	}

	/************************************************************************/

	void Local::push(
		size_t gameId,
		std::string entityName,
		std::string channel,
		Message message
	) {

		return;
	}
}

/****************************************************************************/
/****************************************************************************/

extern "C" {

	output::Driver **loadOutputDrivers() {

		static output::Driver *drivers[] = {
			output::Local::get().get(),
			nullptr
		};

		return drivers;
	}
}
