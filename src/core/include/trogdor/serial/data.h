#ifndef SERIAL_DATA_H
#define SERIAL_DATA_H


#include <variant>
#include <string>
#include <memory>

namespace trogdor::serial {


	class Serializable;

	typedef std::variant<
		size_t,
		int,
		double,
		bool,
		std::string,
		std::shared_ptr<Serializable>
	> Value;
}


#endif // SERIAL_DATA_H