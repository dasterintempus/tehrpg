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
				Entity(const std::string& name, const std::string& type, Engine* engine);
				Entity(unsigned int id, Engine* engine);
				~Entity();
				unsigned int id();
				std::string name();
				std::string type();
				Component* component(const std::string& componenttype);
			
				void destroy();
				void takeownership(Component* component);
				void detach(const std::string& componenttype);
				unsigned int userid();
				void userid(unsigned int id);
			protected:
				void build();
			
				unsigned int _id;
				Engine* _engine;
				std::string _name;
				std::string _type;
				unsigned int _userid;
				std::map<std::string, Component*> _components;
		};
	}
}
