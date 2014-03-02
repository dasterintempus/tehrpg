function move(origin, dx, dy)
	if (dx ~= 0 and dy ~= 0) or (dx == 0 and dy == 0) then
		return false, "Invalid move."
	end
	
	location = origin.component.location
	nx = location.xpos + dx
	ny = location.ypos + dy
	
	destination = engine.map(nx, ny)
	if destination ~= nil then
		origin.act("move", "destination", destination)
		return true, "Moving..."
	else
		return false, "Unable to move that direction."
	end
end

function handle_move(origin, command)
	usagestr = "Invalid usage of move command.\nUsage: move {direction}"

	if #command ~= 2 then
		return false, usagestr
	end
	if command[2] == "north" or command[2] == "n" then
		return move(origin, 0, -1)
	elseif command[2] == "south" or command[2] == "s" then
		return move(origin, 0, 1)
	elseif command[2] == "west" or command[2] == "w" then
		return move(origin, -1, 0)
	elseif command[2] == "east" or command[2] == "e" then
		return move(origin, 1, 0)
	else
		return false, usagestr
	end
end

function can_handle(origin, command)
	--print(command[1], command[2])
	if command[1] == "move" then
		return "handle_move"
	else
		return nil
	end
end