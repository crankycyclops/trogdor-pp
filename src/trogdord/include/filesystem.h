#ifndef FILESYSTEM_H
#define FILESYSTEM_H


#ifdef __cpp_lib_filesystem
	#include <filesystem>
	#define STD_FILESYSTEM std::filesystem
#else
	#include <experimental/filesystem>
	#define STD_FILESYSTEM std::experimental::filesystem
#endif


#endif
