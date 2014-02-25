#pragma once

#include <string>
#include <vector>

namespace teh
{
	namespace RPG
	{
		class Game;
		class Object;
		class VariantData;
		
		class Action
		{
			public:
				Action(Object* source, const std::string& verb, const std::vector<Object*>& targets, Game* game);
				Action(Object* source, const std::string& verb, const std::vector<Object*>& targets, const std::vector<VariantData>& data, Game* game);
			
				Object* source() const;
				std::string verb() const;
				bool is_target(Object* other) const;
				int target_position(Object* other) const;
				std::vector<Object*> targets() const;
				std::vector<VariantData> data() const;
			private:
				Game* _game;
				Object* _source;
				std::string _verb;
				std::vector<Object*> _targets;
				std::vector<VariantData> _data;
		};
	}
}