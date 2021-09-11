#include <trogdor/lua/luastate.h>


// Lua function that returns true.
extern int luaReturnTrue(lua_State *L);

// Tests luaL_checkudata_ex() by taking as input an entity and a type and
// returning true if an argument of that type could be read from the stack and
// false if not.
extern int luaTestCheckudataex(lua_State *L);