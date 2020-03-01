PHP_ARG_ENABLE(trogdord,
	[Whether to enable trogdord support],
	[  --enable-trogdord         Enable trogdord support], yes)

PHP_ARG_WITH(lua_version,
	[Specifies the version of Lua libtrogdor was compiled against],
	[  --with-lua-version         Lua version (default 5.1)], 5.1)

if test $PHP_TROGDORD != "no"; then
	PHP_REQUIRE_CXX()
	PHP_SUBST(TROGDOR_SHARED_LIBADD)
	PHP_ADD_LIBRARY(stdc++, 1, TROGDOR_SHARED_LIBADD)
	PHP_ADD_LIBRARY(trogdor, 1, TROGDOR_SHARED_LIBADD)
	PHP_NEW_EXTENSION(trogdord, *.cpp, $ext_shared, , -std=c++17, cxx)
fi
