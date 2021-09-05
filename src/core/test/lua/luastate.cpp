#include <doctest.h>

#include <trogdor/game.h>
#include <trogdor/lua/luastate.h>
#include <trogdor/iostream/nullerr.h>


// Lua function that confirms sanity of the underlying Lua state
const std::string sanityCheck = " \
    function sanityCheck()\n\
        \n\
        -- All the types that should be defined by the engine\n\
        trogTypes = {\n\
            ['Entity']={Entity},\n\
            ['Resource']={Resource},\n\
            ['Tangible']={Tangible},\n\
            ['Place']={Place},\n\
            ['Room']={Room},\n\
            ['Thing']={Thing},\n\
            ['Object']={Object},\n\
            ['Being']={Being},\n\
            ['Creature']={Creature},\n\
            ['Player']={Player}\n\
        }\n\
        \n\
        for i, type in pairs(trogTypes) do\n\
            if nil == type[1] then\n\
                error(\"Lua type \" .. i .. \" should be defined but isn't\")\n\
                return false\n\
            end\n\
        end\n\
        \n\
        return true\n\
    end\
";


TEST_SUITE("LuaState (luastate.cpp)") {

	TEST_CASE("LuaState (luastate.cpp): LuaState(Game *)") {

        std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
            std::make_unique<trogdor::NullErr>()
        );

        trogdor::LuaState L(game.get());

        // Load into the Lua state a simple script that will be used as a
        // sanity check that returns 1 if anything is wrong and 0 if not.
        L.loadScriptFromString(sanityCheck);

        // Because I'm only executing Lua states in a single thread and because
        // locking would make things more complicated, I'm not going to call
        // L.lock().
        L.call("sanityCheck");
        L.execute(1);

        if (!L.getBoolean(0)) {
            FAIL(L.getLastErrorMsg());
        }

        CHECK(true);
    }
}
