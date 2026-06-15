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

		// Out-of-range values must be rejected, since std::stod() would throw
		// std::out_of_range on them
		CHECK(!trogdor::isValidDouble("1e9999"));    // positive overflow
		CHECK(!trogdor::isValidDouble("-1e9999"));   // negative overflow
		CHECK(!trogdor::isValidDouble("1e-400"));    // underflow

		// Sanity check that ordinary values still validate after the fix
		CHECK(trogdor::isValidDouble("1.5"));
		CHECK(trogdor::isValidDouble("-2.3"));
	}

	TEST_CASE("Utility functions (utility.cpp): resolveContainedPath()") {

		// Legitimate relative paths resolve, normalized, inside the base directory
		auto plain = trogdor::resolveContainedPath("/games/mygame", "foo.lua");
		CHECK(plain.has_value());
		CHECK(0 == plain->compare("/games/mygame/foo.lua"));

		auto nested = trogdor::resolveContainedPath("/games/mygame", "scripts/foo.lua");
		CHECK(nested.has_value());
		CHECK(0 == nested->compare("/games/mygame/scripts/foo.lua"));

		// A leading "./" is harmless and gets normalized away
		auto dotSlash = trogdor::resolveContainedPath("/games/mygame", "./foo.lua");
		CHECK(dotSlash.has_value());
		CHECK(0 == dotSlash->compare("/games/mygame/foo.lua"));

		// ".." is fine as long as it doesn't escape the base directory
		auto innerDotDot = trogdor::resolveContainedPath("/games/mygame", "sub/../foo.lua");
		CHECK(innerDotDot.has_value());
		CHECK(0 == innerDotDot->compare("/games/mygame/foo.lua"));

		// An empty base directory is treated as the current working directory
		auto emptyBase = trogdor::resolveContainedPath("", "foo.lua");
		CHECK(emptyBase.has_value());
		CHECK(0 == emptyBase->compare("foo.lua"));

		// An empty path can't be resolved
		CHECK(!trogdor::resolveContainedPath("/games/mygame", "").has_value());

		// Absolute paths are rejected outright
		CHECK(!trogdor::resolveContainedPath("/games/mygame", "/etc/passwd").has_value());

		// "../" traversal that escapes the base directory is rejected
		CHECK(!trogdor::resolveContainedPath("/games/mygame", "../../../../etc/passwd").has_value());
		CHECK(!trogdor::resolveContainedPath("/games/mygame", "../secret.lua").has_value());

		// Even traversal that dips out and back in is rejected
		CHECK(!trogdor::resolveContainedPath("/games/mygame", "sub/../../mygame/foo.lua").has_value());

		// The sibling prefix trick (string prefix containment would wrongly
		// allow this) must be rejected
		CHECK(!trogdor::resolveContainedPath("/games/mygame", "../mygame-evil/foo.lua").has_value());

		// Containment also works with a relative base directory
		auto relBase = trogdor::resolveContainedPath("games/mygame", "scripts/foo.lua");
		CHECK(relBase.has_value());
		CHECK(0 == relBase->compare("games/mygame/scripts/foo.lua"));

		CHECK(!trogdor::resolveContainedPath("games/mygame", "../../etc/passwd").has_value());
	}
}
