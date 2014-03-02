#pragma once

#include <string>

namespace teh
{
	namespace RPG
	{
		class Entity;
		class Engine;
		
		Entity* constructTile(Engine* engine, long int xpos, long int ypos, const std::string& summary, const std::string& description);
		Entity* constructPlayerCharacter(Engine* engine, long int xpos, long int ypos, const std::string& name, unsigned int userid);
		
		Entity* findTile(Engine* engine, long int xpos, long int ypos);
		Entity* findCharacter(Engine* engine, const std::string& name);
	}
	
}