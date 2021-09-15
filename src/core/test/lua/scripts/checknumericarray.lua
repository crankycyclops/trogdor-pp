function checkNumericArray(arr)

    if "table" ~= type(arr) then
        error("Array argument should have table type but doesn't.")
        return false
    end

    if 5 ~= #arr then
        error("Array should have five elements but has " .. #arr .. " elements instead.")
        return false
    end

    num = 1.0

    for i, number in pairs(arr) do

        if "number" ~= type(number) then
            error("One or more elements in the array doesn't have a number type when it should.")
            return false
        end

        if number ~= num then
            error("One or more elements in the array don't match what was pushed.")
            return false
        end

        num = num + 1
    end

    return true

end