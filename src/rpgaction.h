#pragma once

#include <string>
#include <map>
#include <vector>
#include <utility>

namespace teh
{
	namespace RPG
	{
		class Entity;
		
		class Action
		{
			public:
				typedef std::pair<std::string, Entity*> Actor;
				
				Action(const Actor& origin);
				Action(const Actor& origin, const Actor& target);
				Action(const Actor& origin, const std::vector<Actor>& targets);
			
				Actor origin();
				std::vector<Actor> targets();
			private:
				Actor _origin;
				std::vector<Actor> _targets;
		};
	}
}