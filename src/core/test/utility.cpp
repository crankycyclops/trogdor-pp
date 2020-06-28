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

	TEST_CASE("Utility functions (utility.cpp): replaceAll()") {

		std::string str1 = "apple";
		std::string str2 = "appleapple";
		std::string str3 = "apple orange apple";
		std::string str4 = "test test";

		CHECK(0 == trogdor::replaceAll(str1, "apple", "pear").compare("pear"));
		CHECK(0 == trogdor::replaceAll(str2, "apple", "pear").compare("pearpear"));
		CHECK(0 == trogdor::replaceAll(str3, "apple", "pear").compare("pear orange pear"));
		CHECK(0 == trogdor::replaceAll(str4, "apple", "pear").compare("test test"));
	}

	TEST_CASE("Utility functions (utility.cpp): vectorToStr()") {

		std::vector<std::string> empty;
		std::vector<std::string> oneStr = {"one"};
		std::vector<std::string> twoStr = {"one", "two"};
		std::vector<std::string> threeStr = {"one", "two", "three"};

		CHECK(0 == trogdor::vectorToStr(empty).compare(""));
		CHECK(0 == trogdor::vectorToStr(oneStr).compare("one"));
		CHECK(0 == trogdor::vectorToStr(twoStr).compare("one and two"));
		CHECK(0 == trogdor::vectorToStr(threeStr).compare("one, two and three"));

		CHECK(0 == trogdor::vectorToStr(empty, "or").compare(""));
		CHECK(0 == trogdor::vectorToStr(oneStr, "or").compare("one"));
		CHECK(0 == trogdor::vectorToStr(twoStr, "or").compare("one or two"));
		CHECK(0 == trogdor::vectorToStr(threeStr, "or").compare("one, two or three"));
	}

	TEST_CASE("Utility functions (utility.cpp): isValidInteger()") {

		CHECK(!trogdor::isValidInteger(""));
		CHECK(!trogdor::isValidInteger("a"));
		CHECK(!trogdor::isValidInteger("1a"));
		CHECK(!trogdor::isValidInteger("1.1"));
		CHECK(!trogdor::isValidInteger(".1"));
		CHECK(!trogdor::isValidInteger("-1.1"));
		CHECK(!trogdor::isValidInteger("01"));
		CHECK(trogdor::isValidInteger("0"));
		CHECK(trogdor::isValidInteger("-1"));
		CHECK(trogdor::isValidInteger("10"));
		CHECK(trogdor::isValidInteger("-10"));
		CHECK(trogdor::isValidInteger("12"));
		CHECK(trogdor::isValidInteger("-12"));
	}

	TEST_CASE("Utility functions (utility.cpp): isValidDouble()") {

		CHECK(!trogdor::isValidDouble(""));
		CHECK(!trogdor::isValidDouble("a"));
		CHECK(!trogdor::isValidDouble("1a"));
		CHECK(!trogdor::isValidDouble("01"));
		CHECK(!trogdor::isValidDouble("-01"));
		CHECK(trogdor::isValidDouble("0"));
		CHECK(trogdor::isValidDouble("1"));
		CHECK(trogdor::isValidDouble("10"));
		CHECK(trogdor::isValidDouble("-10"));
		CHECK(trogdor::isValidDouble("21"));
		CHECK(trogdor::isValidDouble("-21"));
		CHECK(trogdor::isValidDouble("0.1"));
		CHECK(trogdor::isValidDouble("-0.1"));
		CHECK(trogdor::isValidDouble("-.1"));
		CHECK(trogdor::isValidDouble(".1"));
		CHECK(trogdor::isValidDouble(".12"));
		CHECK(trogdor::isValidDouble("-.12"));
		CHECK(trogdor::isValidDouble("12.12"));
		CHECK(trogdor::isValidDouble("-12.12"));

		// Right now, this passes, and I think that's okay, because it means
		// the C++ conversion functions can also handle it. But if that ever
		// causes an error later, be sure to fix the utility function and this
		// test to enforce correct behavior.
		CHECK(trogdor::isValidDouble("  1.1"));
	}
}
