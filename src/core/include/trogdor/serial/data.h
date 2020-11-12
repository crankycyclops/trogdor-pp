#ifndef SERIAL_DATA_H
#define SERIAL_DATA_H


#include <variant>
#include <string>
#include <memory>

namespace trogdor::serial {


   /*
      Instead of creating a wrapper class around an unordered_map, I wanted to
      use the STL container directly. But the typedefs don't work because it
      ends up being recursive. If someone can tell me how to define these two
      containers so that I don't need to implement Serializable as a wrapper
      class around std::unordered_map, I would be most grateful.
   */
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