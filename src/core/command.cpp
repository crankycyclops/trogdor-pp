#include "include/command.h"

using namespace std;

namespace core {


   Command::Command() {

      verb = "";
      directObject = "";
      indirectObject = "";
      preposition = "";

      // command is invalid until we successfully parse a command
      invalid = true;
   }


   void Command::execute() {
      // TODO: print error message if error == true
   }


   // static method
   Command *Command::read(istream &in, ostream &out) {

      string commandStr;
      Command *command;

      out << "> ";
      in >> commandStr;

      command = new Command();

      // TODO
      return command;
   }
}

