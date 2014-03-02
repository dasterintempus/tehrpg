function splitstring(str, sep)
        local sep, fields = sep or " ", {}
        local pattern = string.format("([^%s]+)", sep)
        gsub(str, pattern, function(c) fields[#fields+1] = c end)
        return fields
end