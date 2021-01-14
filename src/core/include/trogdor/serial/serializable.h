#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H


#include <any>
#include <optional>
#include <unordered_map>

#include <trogdor/exception/undefinedexception.h>
#include "driver.h"

namespace trogdor::serial {


   // Represents an intermediate form of data that can serialize to and
   // deserialize from various formats.
   class Serializable {

      private:

         // Maps keys to serializable values
         std::unordered_map<std::string, Value> data;

      public:

         // Constructor
         Serializable() {};
         Serializable(const Serializable &rhs) {*this = rhs;}

         // Assignment operator
         Serializable &operator=(const Serializable &rhs) {

            if (this == &rhs) {
               return *this;
            }

            data = rhs.data;
            return *this;
         }

         /*
            Clears any currently set data.

            Input:
               (none)

            Output:
               (none)
         */
         void clear() {data.clear();}

         /*
            Returns the underlying container's size.

            Input:
               (none)

            Output:
               Container's size (size_t)
         */
         size_t size() const {return data.size();}

         /*
            Sets a value.

            Input:
               key (std::string)
               value (varied)

            Output:
               (none)
         */
         void set(std::string key, Value value) {data[key] = value;}
         void set(std::string key, const char *value) {set(key, std::string(value));}

         /*
            Unsets a value.

            Input:
               key (std::string)

            Output:
               (none)
         */
         void erase(std::string key) {data.erase(key);}

         /*
            If the value identified by the specified key is defined and an array,
            this method returns its size. Otherwise, it throws an instance of
            UndefinedException.

            Input:
               key (std::string)

            Output:
               Array size (size_t)
         */
         size_t arraySize(std::string key) const {

            size_t size = 0;
            auto v = get(key);

            if (!v) {
               throw UndefinedException("'key' is undefined.");
            }

            std::visit([&](auto &&arg) {

               using T = std::decay_t<decltype(arg)>;

               if constexpr (
                  std::is_same_v<T, std::vector<std::string>> ||
                  std::is_same_v<T, std::vector<std::shared_ptr<Serializable>>>
               ) {
                  size = arg.size();
               } else {
                  throw UndefinedException("'key' is not an array.");
               }
            }, *v);

            return size;
         }

         /*
            Returns all serialized values (read-only.)

            Input:
               (none)

            Output:
               const std::unordered_map<std::string, Value> &
         */
         const std::unordered_map<std::string, Value> &getAll() const {return data;}

         /*
            Returns the specified value if it exists or std::nullopt if
            not.

            Input:
               key (std::string)

            Output:
               std::optional<Value>
         */
         std::optional<Value> get(std::string key) const {

            return data.find(key) != data.end() ?
               std::optional<Value>(data.find(key)->second) : std::nullopt;
         }
   };
}


#endif // SERIALIZABLE_H