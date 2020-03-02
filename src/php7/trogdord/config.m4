# You must have the Boost Asio library installed in a place where the compiler
# can find them. Otherwise, the build will fail.

PHP_ARG_ENABLE(trogdord,
	[Whether to enable trogdord support],
	[  --enable-trogdord         Enable trogdord support], yes)

if test $PHP_TROGDORD != "no"; then
	PHP_REQUIRE_CXX()
	PHP_SUBST(TROGDORD_SHARED_LIBADD)
	PHP_ADD_LIBRARY(stdc++, 1, TROGDORD_SHARED_LIBADD)
	PHP_ADD_LIBRARY(boost_system, 1, TROGDORD_SHARED_LIBADD)
	PHP_NEW_EXTENSION(trogdord, *.cpp, $ext_shared, , -std=c++17, cxx)
fi
