#ifndef PHP_UTILITY_H
#define PHP_UTILITY_H


// Replace all instance of substr with replacement in str. Returns true if one
// or more matches were found and false if not.
extern bool strReplace(std::string &str, const std::string &substr, const std::string &replacement);


#endif
