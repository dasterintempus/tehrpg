#include "rpgentityfactory.h"
#include "rpgentity.h"
#include "rpgengine.h"
#include "rpgcomponent.h"
#include "mysql.h"

namespace teh
{
	namespace RPG
	{
		Entity* constructTile(Engine* engine, long int xpos, long int ypos, const std::string& summary, const std::string& description)
		{
			sql::Connection* conn = engine->sql()->connect();
			
			//Validate that we don't already have a tile matching these parameters (type and location x/y)
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `Entities`.`id` FROM `Entities` JOIN `locationComponents` WHERE `Entities`.`id` = `locationComponents`.`entity_id` AND `Entities`.`type` = 'tile.map.entity' AND `locationComponents`.`xpos` = ? AND `locationComponents`.`ypos` = ?");
			prep_stmt->setInt(1, xpos);
			prep_stmt->setInt(2, ypos);
			
			sql::ResultSet* res = prep_stmt->executeQuery();
			
			if (res->rowsCount() > 0)
			{
				delete res;
				delete prep_stmt;
				delete conn;
				return 0;
			}
			delete res;
			delete prep_stmt;
			
			//Make an entity
			prep_stmt = conn->prepareStatement("INSERT INTO `Entities` VALUES (NULL, DEFAULT, 'tile.map.entity')");
			try
			{
				prep_stmt->execute();
			}
			catch (sql::SQLException &e)
			{
				delete prep_stmt;
				delete conn;
				return 0;
			}
			
			delete prep_stmt;
			
			//get the entity id
			prep_stmt = conn->prepareStatement("SELECT LAST_INSERT_ID()");
			res = prep_stmt->executeQuery();
			res->next();
			unsigned int entityid = res->getUInt(1);
			delete res;
			delete prep_stmt;
			
			//description Component
			prep_stmt = conn->prepareStatement("INSERT INTO `descriptionComponents` VALUES (NULL, ?, ?, ?)");
			prep_stmt->setString(1, summary);
			prep_stmt->setString(2, description);
			prep_stmt->setUInt(3, entityid);
			
			prep_stmt->execute();
			delete prep_stmt;
			
			//location Component
			prep_stmt = conn->prepareStatement("INSERT INTO `locationComponents` VALUES (NULL, ?, ?, ?)");
			prep_stmt->setInt(1, xpos);
			prep_stmt->setInt(2, ypos);
			prep_stmt->setUInt(3, entityid);
			
			prep_stmt->execute();
			delete prep_stmt;
			
			delete conn;
			
			return engine->get_entity(entityid);
		}
		
		Entity* constructPlayerCharacter(Engine* engine, long int xpos, long int ypos, const std::string& name, unsigned int userid)
		{
			sql::Connection* conn = engine->sql()->connect();
			
			//Validate that we don't already have a character matching these parameters (name and type)
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `id` FROM `Entities` WHERE `type` LIKE '%.character.entity' AND `name` = ?");
			prep_stmt->setString(1, name);
			
			sql::ResultSet* res = prep_stmt->executeQuery();
			
			if (res->rowsCount() > 0)
			{
				delete res;
				delete prep_stmt;
				delete conn;
				return 0;
			}
			delete res;
			delete prep_stmt;
			
			//Validate that the user exists
			prep_stmt = conn->prepareStatement("SELECT `id` FROM `Users` WHERE `id` = ?");
			prep_stmt->setUInt(1, userid);
			
			res = prep_stmt->executeQuery();
			
			if (res->rowsCount() == 0)
			{
				delete res;
				delete prep_stmt;
				delete conn;
				return 0;
			}
			delete res;
			delete prep_stmt;
			
			//Make an entity
			prep_stmt = conn->prepareStatement("INSERT INTO `Entities` VALUES (NULL, ?, 'player.character.entity')");
			prep_stmt->setString(1, name);
			try
			{
				prep_stmt->execute();
			}
			catch (sql::SQLException &e)
			{
				delete prep_stmt;
				delete conn;
				return 0;
			}
			
			//get the entity id
			prep_stmt = conn->prepareStatement("SELECT LAST_INSERT_ID()");
			res = prep_stmt->executeQuery();
			res->next();
			unsigned int entityid = res->getUInt(1);
			delete res;
			delete prep_stmt;
			
			//userownership Component
			prep_stmt = conn->prepareStatement("INSERT INTO `userownershipComponents` VALUES (NULL, ?, ?)");
			prep_stmt->setUInt(1, userid);
			prep_stmt->setUInt(2, entityid);
			
			prep_stmt->execute();
			delete prep_stmt;
			
			//location Component
			prep_stmt = conn->prepareStatement("INSERT INTO `locationComponents` VALUES (NULL, ?, ?, ?)");
			prep_stmt->setInt(1, xpos);
			prep_stmt->setInt(2, ypos);
			prep_stmt->setUInt(3, entityid);
			
			prep_stmt->execute();
			delete prep_stmt;
			
			//statistics Component
			prep_stmt = conn->prepareStatement("INSERT INTO `statisticsComponents` VALUES (NULL, ?, ?, ?, ?, ?, ?, ?)");
			prep_stmt->setInt(1, 3);
			prep_stmt->setInt(2, 3);
			prep_stmt->setInt(3, 3);
			prep_stmt->setInt(4, 3);
			prep_stmt->setInt(5, 3);
			prep_stmt->setInt(6, 3);
			prep_stmt->setUInt(7, entityid);
			
			prep_stmt->execute();
			delete prep_stmt;
			
			//itemcontainer Component
			prep_stmt = conn->prepareStatement("INSERT INTO `itemcontainerComponents` VALUES (NULL, ?, ?)");
			prep_stmt->setUInt(1, 100);
			prep_stmt->setUInt(2, entityid);
			
			prep_stmt->execute();
			delete prep_stmt;
			
			delete conn;
			
			return engine->get_entity(entityid);
		}
		
		Entity* findTile(Engine* engine, long int xpos, long int ypos)
		{
			sql::Connection* conn = engine->sql()->connect();
			
			//Find it
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `Entities`.`id` FROM `Entities` JOIN `locationComponents` WHERE `Entities`.`id` = `locationComponents`.`entity_id` AND `Entities`.`type` = 'tile.map.entity' AND `locationComponents`.`xpos` = ? AND `locationComponents`.`ypos` = ?");
			prep_stmt->setInt(1, xpos);
			prep_stmt->setInt(2, ypos);
			
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
			
			return engine->get_entity(entityid);
		}
		
		Entity* findCharacter(Engine* engine, const std::string& name)
		{
			sql::Connection* conn = engine->sql()->connect();
			
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
			
			return engine->get_entity(entityid);
		}
		
		std::vector<Entity*> findCharactersAt(Engine* engine, long int xpos, long int ypos)
		{
			std::vector<Entity*> out;
			
			sql::Connection* conn = engine->sql()->connect();
			
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT `Entities`.`id` FROM `Entities` JOIN `locationComponents` WHERE `Entities`.`id` = `locationComponents`.`entity_id` AND `Entities`.`type` LIKE '%.character.entity' AND `locationComponents`.`xpos` = ? AND `locationComponents`.`ypos` = ?");
			prep_stmt->setInt(1, xpos);
			prep_stmt->setInt(2, ypos);
			
			sql::ResultSet* res = prep_stmt->executeQuery();
			
			while (res->next())
			{
				Entity* character = engine->get_entity(res->getUInt(1));
				if (engine->is_pc_active(character->id()))
				{
					out.push_back(character);
				}
			}
			
			delete res;
			delete prep_stmt;
			delete conn;
			
			return out;
		}
	}
}
