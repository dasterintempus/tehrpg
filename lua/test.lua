function process(entity)
	print(entity.name)
	result = engine.system.test2("process", entity)
	return result
end