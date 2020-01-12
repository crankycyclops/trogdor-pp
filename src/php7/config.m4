PHP_ARG_ENABLE(trogdor,
	[Whether to enable libtrogdor (Trogdor-pp) support],
	[  --enable-trogdor         Enable libtrogdor (Trogdor-pp) support])

if test $PHP_TROGDOR != "no"; then
	PHP_REQUIRE_CXX()
	PHP_SUBST(TROGDOR_SHARED_LIBADD)
	PHP_ADD_LIBRARY(stdc++, 1, TROGDOR_SHARED_LIBADD)
	PHP_ADD_LIBRARY(trogdor, 1, TROGDOR_SHARED_LIBADD)
	PHP_NEW_EXTENSION(trogdor, module.cpp, $ext_shared)
fi
