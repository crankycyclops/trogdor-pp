#include <trogdord/io/output/driver.h>

namespace output {


    // Exports an output driver that should be successfully loaded
	class TestOut: public Driver {

		private:

			// Singleton instance of the TestOut driver
			static std::unique_ptr<TestOut> instance;

		protected:

			// Construction should only occur through get()
			TestOut();
			TestOut(const TestOut &) = delete;

		public:

			// String representation of the driver's name.
			static const char *DRIVER_NAME;

			// Returns singleton instance of the TestOut output driver.
			static std::unique_ptr<TestOut> &get();

			// Destructor
			virtual ~TestOut();

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
	const char *TestOut::DRIVER_NAME = "test";

	// Singleton instance of the TestOut driver
	std::unique_ptr<TestOut> TestOut::instance = nullptr;

	/************************************************************************/

	/*
		Fill this in if you need the constructor to do anything.
	*/
	TestOut::TestOut() {}

	/************************************************************************/

	/*
		Likewise, if you need the destructor to do anything, fill this in.
	*/
	TestOut::~TestOut() {}

	/************************************************************************/

	std::unique_ptr<TestOut> &TestOut::get() {

		if (!instance) {
			instance = std::unique_ptr<TestOut>(new TestOut());
		}

		return instance;
	}

	/************************************************************************/

	const char *TestOut::getName() {

		return DRIVER_NAME;
	}

	/************************************************************************/

	size_t TestOut::size(
		size_t gameId,
		std::string entityName,
		std::string channel
	) {

		return 0;
	}

	/************************************************************************/

	std::optional<Message> TestOut::pop(
		size_t gameId,
		std::string entityName,
		std::string channel
	) {

		return std::nullopt;
	}

	/************************************************************************/

	void TestOut::push(
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
			output::TestOut::get().get(),
			nullptr
		};

		return drivers;
	}
}
