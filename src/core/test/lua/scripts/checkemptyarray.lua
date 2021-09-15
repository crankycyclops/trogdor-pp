function checkEmptyArray(arr)

    if "table" ~= type(arr) then
        error("Array argument should have table type but doesn't.")
        return false
    end

    if 0 ~= #arr then
        error("Array should be empty but has " .. #arr .. " elements.")
        return false
    end

    return true

end
