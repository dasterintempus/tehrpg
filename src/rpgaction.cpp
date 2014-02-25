#include "rpgaction.h"
#include "rpggame.h"
#include "rpgobject.h"

namespace teh
{
	namespace RPG
	{
		Action::Action(Object* source, const std::string& verb, const std::vector<Object*>& targets, Game* game)
			: _source(source), _verb(verb), _targets(targets), _game(game)
		{
			
		}
		
		Action::Action(Object* source, const std::string& verb, const std::vector<Object*>& targets, const std::vector<VariantData>& data, Game* game)
			: _source(source), _verb(verb), _targets(targets), _data(data), _game(game)
		{
			
		}
		
		Object* Action::source() const
		{
			return _source;
		}
		
		std::string Action::verb() const
		{
			return _verb;
		}
		
		bool Action::is_target(Object* other) const
		{
			return target_position(other) != -1;
		}
		
		int Action::target_position(Object* other) const
		{
			int n = -1;
			for (auto i = _targets.cbegin();i != _targets.cend();i++)
			{
				n++;
				if (other == *i)
					return n;
			}
			return -1;
		}
		
		std::vector<Object*> Action::targets() const
		{
			return _targets;
		}
		
		std::vector<VariantData> Action::data() const
		{
			return _data;
		}
	}
}