#include <trogdor/lua/luastate.h>


class TestLuaState: public trogdor::LuaState {

    public:

        using trogdor::LuaState::LuaState;

        static void luaL_register_wrapper(TestLuaState &L,
        const char *libname, const luaL_Reg *l) {

            return trogdor::LuaState::luaL_register_wrapper(L.L, libname, l);
        }

        // Grants access to underlying instance of lua_State
        lua_State *getRealState() {return L;}

        // Manually increments the number of arguments passed into a function
        // that's being called
        void incNargs() {nArgs++;}
};
