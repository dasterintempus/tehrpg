function splitstring(str, sep)
        local sep, fields = sep or " ", {}
        local pattern = string.format("([^%s]+)", sep)
        gsub(str, pattern, function(c) fields[#fields+1] = c end)
        return fields
end

function oppositedirname(dirname)
	if dirname == "north" then return "south" end
	if dirname == "south" then return "north" end
	if dirname == "west" then return "east" end
	if dirname == "east" then return "west" end
end