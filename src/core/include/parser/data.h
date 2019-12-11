#ifndef DATA_H
#define DATA_H


// these words are reserved and cannot be used as names for user-defined classes
extern const char *g_reservedClassNames[];

/*
   Determines whether or not a class name is reserved.

   Input:
      class name (std::string)

   Output:
      true if the word is reserved and false if not
*/
extern bool isClassNameReserved(std::string name);


#endif
