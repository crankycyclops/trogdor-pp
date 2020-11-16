#ifndef MESSAGES_H
#define MESSAGES_H


#include <string>
#include <unordered_map>


namespace trogdor {

   class Messages {

      private:

         // Hash table mapping message names to messages
         std::unordered_map<std::string, std::string> messageTable;

      public:

         /*
            Constructor is required because there's a copy constructor, but we
            don't actually need it to do anything.
         */
         inline Messages() {}

         /*
            Copy constructor.
         */
         inline Messages (const Messages &m) {messageTable = m.messageTable;}

         /*
            Overloaded assignment operator.
         */
         inline Messages &operator=(const Messages &m) {

            messageTable = m.messageTable;
            return *this;
         }

         /*
            Returns constant iterators to the internal message table (this
            won't work with for-range loops.)

            Input: (none)
            Output: Constant iterator
         */
         inline auto cbegin() {return messageTable.cbegin();}
         inline auto cend() {return messageTable.cend();}

         /*
            Clears all messages.

            Input: (none)
            Output: (none)
         */
         inline void clear() {messageTable.clear();}

         /*
            Gets a message by name.  If it doesn't exist, an empty string is
            returned.

            Input: string & (name)
            Output: string (message)
         */
         std::string get(const std::string &name) const;

         /*
            Sets a message.  If the message already exists, it will be
            overwritten.

            Input: string (message name), string (message)
            Output: (none)
         */
         void set(std::string name, std::string message);

         /*
            Displays a message.  If it doesn't exist, nothing is displayed.

            Input: string (message name), output stream (ostream)
            Output: (none)
         */
         void display(std::string name, std::ostream &out) const;
   };
}


#endif
