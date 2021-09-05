-- Runs basic checks that ensure instances of LuaState are in a sane and
-- consistent state.
function sanityCheck()

    -- All the types that should be defined by the engine. I have to enclose
    -- each type in another table because otherwise, any nil values that should
    -- be picked up as a failure to define a required type will be skipped.
    trogTypes = {
        ['Game']={Game},
        ['Entity']={Entity},
        ['Resource']={Resource},
        ['Tangible']={Tangible},
        ['Place']={Place},
        ['Room']={Room},
        ['Thing']={Thing},
        ['Object']={Object},
        ['Being']={Being},
        ['Creature']={Creature},
        ['Player']={Player}
    }

    for i, type in pairs(trogTypes) do
        if nil == type[1] then
            error("Lua type \"" .. i .. "\" should be defined but isn't")
            return false
        end
    end

    return true
end