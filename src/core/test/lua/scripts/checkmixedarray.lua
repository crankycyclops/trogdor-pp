function checkMixedArray(arr)

    if "table" ~= type(arr) then
        error("Array argument should have table type but doesn't.")
        return false
    end

    if 5 ~= #arr then
        error("Array should have five elements but has " .. #arr .. " elements instead.")
        return false
    end

    if "number" ~= type(arr[1]) then
        error("First element should be a number but is " .. type(arr[1]) .. " instead.")
        return false
    elseif 1.0 ~= arr[1] then
        error("First element should equal 1.0 but is " .. arr[1] .. " instead.")
        return false
    end

    if "boolean" ~= type(arr[2]) then
        error("Second element should be boolean but is " .. type(arr[2]) .. " instead.")
        return false
    elseif true ~= arr[2] then
        error("Second element should equal true but is false instead.")
        return false
    end

    if "string" ~= type(arr[3]) then
        error("Third element should be a string but is " .. type(arr[3]) .. " instead.")
        return false
    elseif "wee!" ~= arr[3] then
        error("Third element should be \"wee!\" but is \"" .. arr[3] .. "\" instead.")
        return false
    end

    if "table" ~= type(arr[4]) then
        error("Fourth element should be an array but is " .. type(arr[4]) .. " instead.")
        return false
    elseif 1 ~= #arr[4] then
        error("Fourth element should have only one element but has " .. #arr[4] .. " instead.")
        return false
    elseif "number" ~= type(arr[4][1]) then
        error("Fourth element's only element should be a number but is a " .. type(arr[4][1]) .. " instead.")
        return false
    elseif 2.0 ~= arr[4][1] then
        error("Fourth element's only element should equal 2.0 but is " .. arr[4][1] .. " instead.")
        return false
    end

    if "table" ~= type(arr[5]) then
        error("Fifth element should be a table but is " .. type(arr[5]) .. " instead.")
        return false
    elseif "string" ~= type(arr[5]["string"]) then
        error("String entry of fifth element should be a string but is " .. type(arr[5]["string"]) .. " instead.")
        return false
    elseif "string" ~= arr[5]["string"] then
        error("String entry of fifth element should equal \"string\" but is " .. arr[5]["string"] .. " instead.")
        return false
    elseif "number" ~= type(arr[5]["number"]) then
        error("Numeric entry of fifth element should be a number but is " .. type(arr[5]["number"]) .. " instead.")
        return false
    elseif 1.0 ~= arr[5]["number"] then
        error("Numeric entry of fifth element should equal 1.0 but is " .. arr[5]["number"] .. " instead.")
        return false
    end

    return true

end