function process_action(action)
	origin = action.origin
	if origin.verb == "move" then
		destlocation = action.targets[1].entity.component.location
		location = origin.entity.component.location
		location.xpos = destlocation.xpos
		location.ypos = destlocation.ypos
		return 1
	end
	return 0
end