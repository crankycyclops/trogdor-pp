#include <trogdor/serial/driver.h>
#include <trogdor/serial/serializable.h>

#include <trogdor/exception/undefinedexception.h>


namespace trogdor::serial {

   Driver::~Driver() {}

   /**************************************************************************/

   void Driver::serializeSizeT(std::any data, std::string key, const size_t &value) {

      throw UndefinedException("You must implement serializeSizeT() in order to call Driver::doSerialize()");
   }

   /**************************************************************************/

   void Driver::serializeInt(std::any data, std::string key, int const &value) {

      throw UndefinedException("You must implement serializeInt() in order to call Driver::doSerialize()");
   }

   /**************************************************************************/

   void Driver::serializeDouble(std::any data, std::string key, const double &value) {

      throw UndefinedException("You must implement serializeDouble() in order to call Driver::doSerialize()");
   }

   /**************************************************************************/

   void Driver::serializeBool(std::any data, std::string key, const bool &value) {

      throw UndefinedException("You must implement serializeBool() in order to call Driver::doSerialize()");
   }

   /**************************************************************************/

   void Driver::serializeString(std::any data, std::string key, const std::string &value) {

      throw UndefinedException("You must implement serializeString() in order to call Driver::doSerialize()");
   }

   /**************************************************************************/

   void Driver::serializeSerializable(std::any data, std::string key, const std::shared_ptr<Serializable> &value) {

      throw UndefinedException("You must implement serializeSerializable() in order to call Driver::doSerialize()");
   }

   /**************************************************************************/

   void Driver::serializeStringVector(std::any data, std::string key, const std::vector<std::string> &value) {

      throw UndefinedException("You must implement serializeStringVector() in order to call Driver::doSerialize()");
   }

   /**************************************************************************/

   void Driver::serializeSerializableVector(std::any data, std::string key, const std::vector<std::shared_ptr<Serializable>> &value) {

      throw UndefinedException("You must implement serializeSerializableVector() in order to call Driver::doSerialize()");
   }

   /**************************************************************************/

   std::any Driver::initSerializedChild() {

      throw UndefinedException("You must implement initSerializedChild() in order to call Driver::doSerialize()");
   }

   /**************************************************************************/

   std::any Driver::doSerialize(
      const std::shared_ptr<Serializable> &data,
      std::any output
   ) {

		if (!output.has_value()) {
			output = initSerializedChild();
		}

		for (const auto &value: data->getAll()) {

			std::visit([&](auto &&arg) {

				using T = std::decay_t<decltype(arg)>;

				if constexpr (std::is_same_v<T, size_t>) {
					serializeSizeT(output, value.first, arg);
				}

				else if constexpr (std::is_same_v<T, int>) {
					serializeInt(output, value.first, arg);
				}

				else if constexpr (std::is_same_v<T, double>) {
					serializeDouble(output, value.first, arg);
				}

				else if constexpr (std::is_same_v<T, bool>) {
					serializeBool(output, value.first, arg);
				}

				else if constexpr (std::is_same_v<T, std::string>){
					serializeString(output, value.first, arg);
				}

				else if constexpr (std::is_same_v<T, std::shared_ptr<Serializable>>) {
					serializeSerializable(output, value.first, arg);
				}

				else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
					serializeStringVector(output, value.first, arg);
				}

				else if constexpr (std::is_same_v<T, std::vector<std::shared_ptr<Serializable>>>) {
					serializeSerializableVector(output, value.first, arg);
				}

				else {
					throw UndefinedException("Invalid type encountered in instance of serial::Serializable");
				}
			}, value.second);
		}

		return output;
   }
}
