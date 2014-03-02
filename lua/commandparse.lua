function process_command(origin, command)
	basiccommands = engine.system.basiccommands
	handlername = basiccommands.can_handle(origin, command)
	if handlername ~= nil then
		return basiccommands[handlername](origin, command)
	end
	return false, ""
end