function testLualCheckudataex()

    entityTypes = {"entity", "resource", "tangible", "place", "room", "thing", "object", "being", "creature", "player"}

    resourceTypes = {"entity", "resource"}
    notResourceTypes = {"place", "room", "thing", "object", "being", "creature", "player"}

    roomTypes = {"entity", "place", "room"}
    notRoomTypes = {"resource", "thing", "object", "being", "creature", "player"}

    objectTypes = {"entity", "thing", "object"}
    notObjectTypes = {"resource", "place", "room", "being", "creature", "player"}

    creatureTypes = {"entity", "thing", "being", "creature"}
    notCreatureTypes = {"resource", "place", "room", "object", "player"}

    playerTypes = {"entity", "thing", "being", "player"}
    notPlayerTypes = {"resource", "place", "room", "object", "creature"}

    room = Room.new("testroom")
    object = Object.new("testobject")
    creature = Creature.new("testcreature")
    -- player = Player.new("testplayer")
    --resource = Resurce.new("testresource")

    -- Make sure each instantiable type can be retrieved by any of its
    -- inherited types
    for _, type in ipairs(roomTypes) do
        if not Test.luaTestCheckudataex(room, type) then
            error("Failed to get Room by type " .. type)
            return false
        end
    end

    for _, type in ipairs(objectTypes) do
        if not Test.luaTestCheckudataex(object, type) then
            error("Failed to get Object by type " .. type)
            return false
        end
    end

    for _, type in ipairs(creatureTypes) do
        if not Test.luaTestCheckudataex(creature, type) then
            error("Failed to get Creature by type " .. type)
            return false
        end
    end

    -- Make sure each instantiable type CANNOT be retrieved by any type it
    -- doesn't inherit
    for _, type in ipairs(notRoomTypes) do
        if pcall(function () Test.luaTestCheckudataex(room, type) end) then
            error("Was able to retrieve Room by non-inherited type " .. type)
        end
    end

    for _, type in ipairs(notObjectTypes) do
        if pcall(function () Test.luaTestCheckudataex(object, type) end) then
            error("Was able to retrieve Object by non-inherited type " .. type)
        end
    end

    for _, type in ipairs(notCreatureTypes) do
        if pcall(function () Test.luaTestCheckudataex(creature, type) end) then
            error("Was able to retrieve Creature by non-inherited type " .. type)
        end
    end

    -- For the sake of completeness, I'll also test with basic data types
    for _, type in ipairs(entityTypes) do
        if pcall(function () Test.luaTestCheckudataex(1, type) end) then
            error("Was able to retrieve " .. type .. " with basic data type")
        end
    end

    for _, type in ipairs(entityTypes) do
        if pcall(function () Test.luaTestCheckudataex(3.3, type) end) then
            error("Was able to retrieve " .. type .. " with basic data type")
        end
    end

    for _, type in ipairs(entityTypes) do
        if pcall(function () Test.luaTestCheckudataex(true, type) end) then
            error("Was able to retrieve " .. type .. " with basic data type")
        end
    end

    for _, type in ipairs(entityTypes) do
        if pcall(function () Test.luaTestCheckudataex(false, type) end) then
            error("Was able to retrieve " .. type .. " with basic data type")
        end
    end

    for _, type in ipairs(entityTypes) do
        if pcall(function () Test.luaTestCheckudataex({}, type) end) then
            error("Was able to retrieve " .. type .. " with basic data type")
        end
    end

    return true

end
