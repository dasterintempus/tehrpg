#include "rpgaction.h"
#include "rpgentity.h"
#include "rpgengine.h"

#include <iostream>

namespace teh
{
	namespace RPG
	{
		Action::Action(const Actor& origin)
			: _origin(origin)
		{
			
		}
		
		Action::Action(const Action::Actor& origin, const Action::Actor& target)
			: _origin(origin)
		{
			_targets.push_back(target);
		}
		
		Action::Action(const Action::Actor& origin, const std::vector<Action::Actor>& targets)
			: _origin(origin), _targets(targets)
		{
			std::cerr << "Action created with " << _targets.size() << " # of targets" << std::endl;
		}
	
		Action::Actor Action::origin()
		{
			return _origin;
		}
		
		std::vector<Action::Actor> Action::targets()
		{
			return _targets;
		}
	}
}