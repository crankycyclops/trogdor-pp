#include <doctest.h>
#include <trogdor/utility.h>


TEST_SUITE("Utility functions (utility.cpp)") {

	TEST_CASE("Utility functions (utility.cpp): strToLower()") {

		std::string allCaps = "TEST";
		std::string allLower = "test";
		std::string oneCap = "tEst";
		std::string mixOfChars = "  \t \n  &# tEST!";

		CHECK(0 == trogdor::strToLower(allCaps).compare("test"));
		CHECK(0 == trogdor::strToLower(allLower).compare("test"));
		CHECK(0 == trogdor::strToLower(oneCap).compare("test"));
		CHECK(0 == trogdor::strToLower(mixOfChars).compare("  \t \n  &# test!"));
	}

	TEST_CASE("Utility functions (utility.cpp): ltrim()") {

		std::string noSpace = "Test";
		std::string leftSpace = "\t\n  Test";
		std::string rightSpace = "Test \t\n  ";
		std::string leftAndRightSpace = "\t\n  Test\t\n  ";

		trogdor::ltrim(noSpace);
		trogdor::ltrim(leftSpace);
		trogdor::ltrim(rightSpace);
		trogdor::ltrim(leftAndRightSpace);

		CHECK(0 == noSpace.compare("Test"));
		CHECK(0 == leftSpace.compare("Test"));
		CHECK(0 == rightSpace.compare("Test \t\n  "));
		CHECK(0 == leftAndRightSpace.compare("Test\t\n  "));
	}

	TEST_CASE("Utility functions (utility.cpp): rtrim()") {

		std::string noSpace = "Test";
		std::string leftSpace = "\t\n  Test";
		std::string rightSpace = "Test \t\n  ";
		std::string leftAndRightSpace = "\t\n  Test\t\n  ";

		trogdor::rtrim(noSpace);
		trogdor::rtrim(leftSpace);
		trogdor::rtrim(rightSpace);
		trogdor::rtrim(leftAndRightSpace);

		CHECK(0 == noSpace.compare("Test"));
		CHECK(0 == leftSpace.compare("\t\n  Test"));
		CHECK(0 == rightSpace.compare("Test"));
		CHECK(0 == leftAndRightSpace.compare("\t\n  Test"));
	}

	TEST_CASE("Utility functions (utility.cpp): trim()") {

		std::string noSpace = "Test";
		std::string leftSpace = "\t\n  Test";
		std::string rightSpace = "Test \t\n  ";
		std::string leftAndRightSpace = "\t\n  Test\t\n  ";

		trogdor::trim(noSpace);
		trogdor::trim(leftSpace);
		trogdor::trim(rightSpace);
		trogdor::trim(leftAndRightSpace);

		CHECK(0 == noSpace.compare("Test"));
		CHECK(0 == leftSpace.compare("Test"));
		CHECK(0 == rightSpace.compare("Test"));
		CHECK(0 == leftAndRightSpace.compare("Test"));
	}
}
