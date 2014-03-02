#pragma once

#include <string>
#include <map>

namespace teh
{
	namespace RPG
	{
		class Engine;
		class Component;
		
		class Entity
		{
			public:
				Entity(unsigned int id, Engine* engine);
				~Entity();
				unsigned int id();
				std::string name();
				std::string type();
				Component* component(const std::string& componenttype);
			private:
				unsigned int _id;
				Engine* _engine;
				std::string _name;
				std::string _type;
				std::map<std::string, Component*> _components;
		};
	}
}
