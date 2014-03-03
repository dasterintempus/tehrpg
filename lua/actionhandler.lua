function process_action(action)
	origin = action.origin
	if origin.verb == "move" then
		dirname = action.targets[1].verb
		charsatcurtile = engine.characters_at(origin.entity.component.location.xpos, origin.entity.component.location.ypos)

		for n=1, #charsatcurtile do
			if charsatcurtile[n] ~= origin.entity then
				charsatcurtile[n].msg(origin.entity.name .. " has left the area, heading " .. dirname .. ".")
			end
		end
		
		desttile = action.targets[1].entity
		destlocation = desttile.component.location
		
		--Message at arrival location
		oppositedirname = engine.system.util.oppositedirname(dirname)
		charsatdesttile = engine.characters_at(destlocation.xpos, destlocation.ypos)
		for n=1, #charsatdesttile do
			charsatdesttile[n].msg(origin.entity.name .. " has entered the area, arriving from the " .. oppositedirname .. ".")
		end
		
		--update posiiton
		location = origin.entity.component.location
		location.xpos = destlocation.xpos
		location.ypos = destlocation.ypos
		
		--return ok (unused)
		return 1
	end
	return 0
end