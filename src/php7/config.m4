PHP_ARG_ENABLE(trogdor,
	[Whether to enable libtrogdor (Trogdor-pp) support],
	[  --enable-trogdor         Enable libtrogdor (Trogdor-pp) support], yes)

PHP_ARG_WITH(lua_version,
	[Specifies the version of Lua libtrogdor was compiled against],
	[  --with-lua-version         Lua version (default 5.1)], 5.1)

if test $PHP_TROGDOR != "no"; then
	PHP_REQUIRE_CXX()
	PHP_SUBST(TROGDOR_SHARED_LIBADD)
	PHP_ADD_LIBRARY(stdc++, 1, TROGDOR_SHARED_LIBADD)
	PHP_ADD_LIBRARY(trogdor, 1, TROGDOR_SHARED_LIBADD)
	PHP_ADD_INCLUDE(`pkg-config --cflags-only-I lua-$php_with_lua_version | sed -r 's/^-I//'`)
	PHP_ADD_INCLUDE(`pkg-config --cflags-only-I libxml-2.0 | sed -r 's/^-I//'`)
	PHP_NEW_EXTENSION(trogdor, *.cpp entities/*.cpp, $ext_shared, , -std=c++17, cxx)
fi
