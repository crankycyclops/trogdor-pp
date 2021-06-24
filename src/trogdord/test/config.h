#ifndef TEST_CONFIG_H
#define TEST_CONFIG_H


#include <fstream>

#include <trogdord/config.h>
#include <trogdord/filesystem.h>


inline void initIniFile(std::string iniFilename, std::unordered_map<std::string, std::string> values) {

	for (auto const &value: values) {

		std::ofstream iniFile (iniFilename, std::ofstream::out);

		iniFile << '[' << value.first.substr(0, value.first.find('.')) << ']' << std::endl;
		iniFile << value.first.substr(value.first.find('.') + 1) << "=" << value.second << std::endl;
		iniFile.close();
	}

	Config::get()->load(iniFilename);
}


#endif