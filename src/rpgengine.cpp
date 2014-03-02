#include "rpgengine.h"
#include "application.h"
#include "gameserver.h"
#include "mysql.h"
#include "rpgentity.h"
#include "rpgcomponent.h"
#include "rpgsystem.h"
#include "rpgentityfactory.h"
#include "rpgaction.h"
#include "exceptions.h"
#include "rpgcommandhandler.h"

#include <iostream>

namespace teh
{
	namespace RPG
	{
		Engine::Engine(Application* parent, GameServer* server)
			: _parent(parent), _server(server), _done(false)
		{
			//Ensure divine void exists
			
			Entity* tile = findTile(this, -1, -1);
			if (!tile)
			{
				tile = constructTile(this, -1, -1, "A divine void.", "A divine emptiness (where developers hang out and test things).");
			}
			Entity* tile2 = findTile(this, -1, 0);
			if (!tile2)
			{
				tile2 = constructTile(this, -1, 0, "Stairs to Earth.", "The stairway down from the heavens.");
			}
		}
		
		Engine::~Engine()
		{
			for (auto i = _entities.begin(); i != _entities.end(); i++)
			{
				delete (*i).second;
			}
			for (auto i = _systems.begin(); i != _systems.end(); i++)
			{
				delete (*i).second;
			}
		}
		
		
		void Engine::init()
		{
			_parent->parser()->add_handler(new CommandHandler(this));
		}
		
		void Engine::start()
		{
			init();
			
			sf::Clock clock;
			
			_donemutex.lock();
			while (!_done)
			{
				_donemutex.unlock();
				
				while (clock.getElapsedTime() < sf::milliseconds(1500))
				{
					sf::sleep(sf::milliseconds(50));
				}
				sf::Time elapsed = clock.restart();
				
				while (elapsed.asMilliseconds() > 0)
				{
					tick();
					elapsed -= sf::milliseconds(1500);
				}
				
				_donemutex.lock();
			}
			_donemutex.unlock();
		}
		
		void Engine::finish()
		{
			sf::Lock donelock(_donemutex);
			_done = true;
		}

		Entity* Engine::get_entity(unsigned int entityid)
		{
			if (_entities.count(entityid) == 0)
			{
				sql::Connection* conn = sql()->connect();
				sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT * FROM `Entities` WHERE `id` = ?");
				prep_stmt->setUInt(1, entityid);
				sql::ResultSet* res = prep_stmt->executeQuery();
				if (res->rowsCount() == 1)
				{
					_entities[entityid] = new Entity(entityid, this);
					_entityactions[entityid] = std::queue<Action*>(); //Just put something here
				}
				else
				{
					delete res;
					delete prep_stmt;
					delete conn;
					return 0;
				}
				delete res;
				delete prep_stmt;
				delete conn;
			}
			return _entities[entityid];
		}
		
		System* Engine::get_system(const std::string& name)
		{
			if (_systems.count(name) == 0)
			{
				System* system = 0;
				try
				{
					system = new System(name, this);
				}
				catch (teh::Exceptions::InvalidSystem& e)
				{
					std::cout << e.what() << std::endl;
					return 0;
				}
				_systems[name] = system;
			}
			return _systems[name];
		}
		
		MySQL* Engine::sql()
		{
			return _parent->sql();
		}
		
		void Engine::message_client(const clientid& client, const std::string& message)
		{
			GameClient* gc = _server->get_client(client);
			if (gc)
				gc->write_line(message);
		}
		
		Entity* Engine::get_pc(const clientid& client)
		{
			if (_client2pc.count(client) == 0)
				return 0;
			return get_entity(_client2pc[client]);
		}
		
		bool Engine::is_pc_active(unsigned int entityid)
		{
			for (auto i = _client2pc.begin(); i != _client2pc.end(); i++)
			{
				if ((*i).second == entityid)
					return true;
			}
			return false;
		}
		
		bool Engine::is_pc_name_active(const std::string& name)
		{
			Entity* character = findCharacter(this, name);
			if (!character)
				return false;
			
			return is_pc_active(character->id());
		}
		
		bool Engine::set_pc_played_by(unsigned int entityid, const clientid& client)
		{
			if (get_pc(client) || is_pc_active(entityid))
				return false;
			
			unsigned int userid = get_client(client)->userid();
			
			Entity* character = get_entity(entityid);
			Component* userownership = character->component("userownership");
			
			if (userownership->getUInt("user_id") != userid)
				return false;
			
			_client2pc[client] = entityid;
			return true;
		}
		
		bool Engine::set_pc_name_played_by(const std::string& name, const clientid& client)
		{
			if (get_pc(client))
				return false;
			
			Entity* character = findCharacter(this, name);
			if (!character)
				return false;
			
			return set_pc_played_by(character->id(), client);
		}
		
		std::vector<std::string> Engine::get_character_names_of_user(unsigned int userid)
		{
			std::vector<std::string> out;
			
			sql::Connection* conn = sql()->connect();
			
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `Entities`.`name` FROM `Entities` JOIN `userownershipComponents` WHERE `Entities`.`id` = `userownershipComponents`.`entity_id` AND `Entities`.`type` LIKE '%.character.entity' AND `userownershipComponents`.`user_id` = ?");
			prep_stmt->setUInt(1, userid);
			
			sql::ResultSet* res = prep_stmt->executeQuery();
			
			if (res->rowsCount() > 0)
			{
				delete res;
				delete prep_stmt;
				delete conn;
				return out;
			}
			
			while (res->next())
			{
				out.push_back(res->getString(1));
			}
			
			delete res;
			delete prep_stmt;
			delete conn;
			
			return out;
		}
		
		std::vector<std::string> Engine::get_character_names_of_client(const clientid& client)
		{
			unsigned int userid = get_client(client)->userid();
			
			return get_character_names_of_user(userid);
		}
		
		void Engine::queueAction(unsigned int entityid, Action* action)
		{
			_entityactions[entityid].push(action);
		}
		
		void Engine::logout(const clientid& client)
		{
			if (_client2pc.count(client))
				_client2pc.erase(client);
		}
		
		GameClient* Engine::get_client(const clientid& client)
		{
			return _server->get_client(client);
		}
		
		bool Engine::valid_coord(const long int& xpos, const long int& ypos)
		{
			if (xpos < 0)
			{
				return false;
			}
			else if (xpos > Engine::WorldXSize)
			{
				return false;
			}
			if (ypos < 0)
			{
				return false;
			}
			else if (ypos >= Engine::WorldYSize)
			{
				return false;
			}
			return true;
		}
		
		void Engine::tick()
		{
			cleanup();
			System* actionhandler = get_system("actionhandler");
			for (auto i = _entityactions.begin(); i != _entityactions.end();i++)
			{
				Entity* entity = get_entity((*i).first);
				if (!entity)
					continue;
				if ((*i).second.empty())
					continue;
				Action* action = (*i).second.front();
				(*i).second.pop();
				
				actionhandler->process_action(action);
				delete action;
			}
		}
		
		void Engine::cleanup()
		{
			//do nothing for now
		}
	}
}