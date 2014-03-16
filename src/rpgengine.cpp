#include "rpgengine.h"
#include "application.h"
#include "gameserver.h"
#include "mysql.h"
#include "rpgentity.h"
#include "rpgcomponent.h"
#include "rpgsystem.h"
#include "rpgentityfactory.h"
#include "rpgaction.h"
#include "rpgworld.h"
#include "exceptions.h"
#include "luacommandhandler.h"
#include "rpgworldbuilder.h"

#include <iostream>

namespace teh
{
	namespace RPG
	{
		Engine::Engine(Application* parent, const std::string& module, GameServer* server)
			: _parent(parent), _module(module), _server(server), _done(false), _world(new World(this))
		{
			_moduleconfig = ReadJSONFile(get_module_path() + "module.json");
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
			if (_world)
				delete _world;
		}
		
		
		void Engine::init()
		{
			std::string startupstr = _moduleconfig.get("startup", Json::Value("")).asString();
			
			unsigned int pos = startupstr.find(':');
			if (pos != std::string::npos)
			{
				std::string startupname = startupstr.substr(0, pos);
				std::string startupfunction = startupstr.substr(pos+1);
				System* startup = get_system(startupname);
				startup->startup(startupfunction);
			}
			
			Json::Value parsers = _moduleconfig.get("parsers", Json::Value());
			std::vector<std::string> membernames = parsers.getMemberNames();
			for (unsigned int n = 0;n < membernames.size();n++)
			{
				std::string prefixstr = membernames[n];
				std::string parser = parsers[prefixstr].asString();
				char prefix = prefixstr[0];
				_parent->parser()->add_handler(new LuaCommandHandler(this, parser, prefix));
			}
			
			_actionhandler = _moduleconfig.get("actionhandler", Json::Value("")).asString();
			
			get_world()->load();
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

		std::string Engine::get_etc(const std::string& key)
		{
			if (_etc.count(key) == 0)
			{
				sql::Connection* conn = sql()->connect();
				sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `value` FROM `etc` WHERE `key` = ?");
				prep_stmt->setString(1, key.c_str());
				sql::ResultSet* res = prep_stmt->executeQuery();
				if (res->rowsCount() == 0)
				{
					delete res;
					delete prep_stmt;
					delete conn;
					return std::string();
				}
				res->next();
				_etc[key] = res->getString(1);
				delete res;
				delete prep_stmt;
				delete conn;
			}
			return _etc[key];
		}
		
		void Engine::set_etc(const std::string& key, const std::string& value)
		{
			_etc[key] = value;
			sql::Connection* conn = sql()->connect();
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("INSERT INTO `etc` (`key`, `value`) VALUES (?, ?) ON DUPLICATE KEY UPDATE `value` = `value`");
			prep_stmt->setString(1, key.c_str());
			prep_stmt->setString(2, value.c_str());
			prep_stmt->execute();
			delete prep_stmt;
			delete conn;
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
		
		bool Engine::activate_entity(Entity* entity)
		{
			if (_entities.count(entity->id()) == 0)
			{
				_entities[entity->id()] = entity;
				return true;
			}
			return false;
		}
		
		void Engine::delete_entity(Entity* entity)
		{
			if (_entities.count(entity->id()) != 0)
				_entities.erase(entity->id());
			delete entity;
		}
		
		World* Engine::get_world()
		{
			return _world;
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
				//catch (teh::Exceptions::SystemLuaError& e)
				//{
				//	std::cout << e.what() << std::endl;
				//	return 0;
				//}
				_systems[name] = system;
			}
			return _systems[name];
		}
		
		std::string Engine::get_module_path() const
		{
			return _module + "/";
		}
			
		const Json::Value& Engine::get_module_config() const
		{
			return _moduleconfig;
		}
		
		void	Engine::firstinit()
		{
			sql::Connection* conn = sql()->connect();
			sql::Statement* stmt = conn->createStatement();
			
			//Build core tables
			stmt->execute("CREATE TABLE `Users` (`id` INT UNSIGNED AUTO_INCREMENT PRIMARY KEY, `username` VARCHAR(255) UNIQUE NOT NULL, `hashedpasswd` BINARY(64) NOT NULL, `permissions` SMALLINT UNSIGNED NOT NULL, INDEX (`username`)) ENGINE=InnoDB");
			
			stmt->execute("CREATE TABLE `Entities` (`id` INT UNSIGNED AUTO_INCREMENT PRIMARY KEY, `name` VARCHAR(255) NULL DEFAULT NULL,`type` VARCHAR(255) NOT NULL, `user_id` INT UNSIGNED NULL, FOREIGN KEY (`user_id`) REFERENCES `Users` (`id`) ON DELETE CASCADE ON UPDATE CASCADE) ENGINE=InnoDB");
			
			stmt->execute("CREATE TABLE `etc` (`key` VARCHAR(255) PRIMARY KEY, `value` VARCHAR(255) NOT NULL) ENGINE=InnoDB");
			
			stmt->execute("CREATE TABLE `WorldFlags` (`map` VARCHAR(255) NOT NULL, `x` INT NOT NULL, `y` INT NOT NULL, `flag` VARCHAR(255) NOT NULL, PRIMARY KEY `WorldFlagsLocation` (`map`, `x`, `y`)) ENGINE=InnoDB");
			
			stmt->execute("CREATE TABLE `WorldValues` (`map` VARCHAR(255) NOT NULL, `x` INT NOT NULL, `y` INT NOT NULL, `value` FLOAT NOT NULL, PRIMARY KEY `WorldValuesLocation` (`map`, `x`, `y`)) ENGINE=InnoDB");

			//Build component tables
			Json::Value components = _moduleconfig.get("components", Json::Value());
			for (unsigned int n = 0;n < components.size(); n++)
			{
				Json::Value componentdef = ReadJSONFile(get_module_path() + "components/" + components[n].asString() + ".json");
				std::string querystr = "CREATE TABLE `" + components[n].asString() + "Components` (`id` INT UNSIGNED AUTO_INCREMENT PRIMARY KEY, ";
				std::vector<std::string> membernames = componentdef.getMemberNames();
				for (unsigned int namen = 0;namen < membernames.size();namen++)
				{
					std::string name = membernames[namen];
					std::string type = componentdef[name].asString();
					std::string definition = "";
					if (type == "Uint")
						definition = "INT UNSIGNED NOT NULL";
					else if (type == "Int")
						definition = "INT NOT NULL";
					else if (type == "String")
						definition = "VARCHAR(255) NOT NULL";
					else if (type == "Bool")
						definition = "BOOLEAN NOT NULL";
					else
						throw teh::Exceptions::InvalidComponentDefinition(components[n].asString(), name, type);
					
					querystr += "`" + name + "` " + definition + ", ";
				}
				querystr += "`entity_id` INT UNSIGNED UNIQUE NOT NULL, FOREIGN KEY (`entity_id`) REFERENCES `Entities` (`id`) ON DELETE CASCADE ON UPDATE CASCADE) ENGINE=InnoDB;";
				
				stmt->execute(querystr);
			}
			
			delete stmt;
			delete conn;
			
			std::string firstinitstr = _moduleconfig.get("firstinit", Json::Value("")).asString();
			
			unsigned int pos = firstinitstr.find(':');
			if (pos != std::string::npos)
			{
				std::string firstinitname = firstinitstr.substr(0, pos);
				std::string firstinitfunction = firstinitstr.substr(pos+1);
				System* firstinit = get_system(firstinitname);
				firstinit->startup(firstinitfunction);
			}
			
			//World* world = get_world();
			//world->save();
		}
		
		std::pair<long int, long int> Engine::globalspawnpoint()
		{
			return std::make_pair((long int)-1, (long int)-1);
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
		
		clientid Engine::get_player(Entity* entity)
		{
			for (auto i = _client2pc.begin(); i != _client2pc.end(); i++)
			{
				if ((*i).second == entity->id())
					return (*i).first;
			}
			return -1;
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
			Entity* character = findCharacter(name);
			if (!character)
				return false;
			
			return is_pc_active(character->id());
		}
		
		bool Engine::set_pc_played_by(unsigned int entityid, const clientid& client)
		{
			if (get_pc(client) || is_pc_active(entityid))
				return false;
			
			unsigned int userid = get_client(client)->userid();
			
			sql::Connection* conn = sql()->connect();
			
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `id` FROM `Entities` WHERE `id` = ? AND `user_id` = ?");
			prep_stmt->setUInt(1, entityid);
			prep_stmt->setUInt(2, userid);
			
			sql::ResultSet* res = prep_stmt->executeQuery();
			
			if (res->rowsCount() == 0)
			{
				delete res;
				delete prep_stmt;
				delete conn;
				return false;
			}
			
			_client2pc[client] = entityid;
			
			delete res;
			delete prep_stmt;
			delete conn;
			return true;
		}
		
		bool Engine::set_pc_name_played_by(const std::string& name, const clientid& client)
		{
			if (get_pc(client))
				return false;
			
			Entity* character = findCharacter(name);
			if (!character)
				return false;
			
			return set_pc_played_by(character->id(), client);
		}
		
		std::vector<std::string> Engine::get_character_names_of_user(unsigned int userid)
		{
			std::vector<std::string> out;
			
			sql::Connection* conn = sql()->connect();
			
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `name` FROM `Entities` WHERE `type` = 'player.character.entity' AND `user_id` = ?");
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
		
		void Engine::message_entity(Entity* entity, const std::string& msg)
		{
			clientid player = get_player(entity);
			if (player != -1)
			{
				message_client(player, msg);
			}
		}
		
		Entity* Engine::findCharacter(const std::string& name)
		{
			sql::Connection* conn = sql()->connect();
			
			//Find it
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `id` FROM `Entities` WHERE `type` LIKE '%.character.entity' AND `name` = ?");
			prep_stmt->setString(1, name);
			
			sql::ResultSet* res = prep_stmt->executeQuery();
			
			if (res->rowsCount() == 0)
			{
				delete res;
				delete prep_stmt;
				delete conn;
				return 0;
			}
			res->next();
			unsigned int entityid = res->getUInt(1);
			
			delete res;
			delete prep_stmt;
			
			delete conn;
			
			return get_entity(entityid);
		}
		
		void Engine::queue_action(unsigned int entityid, Action* action)
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
			if (_actionhandler != "")
			{
				unsigned int pos = _actionhandler.find(':');
				std::string handlersystem = _actionhandler.substr(0, pos);
				std::string handlerfunction = _actionhandler.substr(pos+1);
				
				System* actionhandler = get_system(handlersystem);
				for (auto i = _entityactions.begin(); i != _entityactions.end();i++)
				{
					Entity* entity = get_entity((*i).first);
					if (!entity)
						continue;
					if ((*i).second.empty())
						continue;
					Action* action = (*i).second.front();
					(*i).second.pop();
					
					actionhandler->process_action(action, handlerfunction);
					delete action;
				}
			}			
			else
			{
				std::cerr << "Tried to tick, but no defined action handler..." << std::endl;
			}
		}
		
		void Engine::cleanup()
		{
			//do nothing for now
		}
	}
}