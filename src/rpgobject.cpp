#include "rpgobject.h"
#include "rpggame.h"
#include "mysql.h"

namespace teh
{
	namespace RPG
	{
		//
		// VariantData
		//
		VariantData VariantData::pop_from_lua(lua_State* L)
		{
			if (lua_istable(L, 1))
			{
				std::vector<float> fval;
				std::vector<std::string> sval;
				lua_pushnil(L);
				while (lua_next(L, 1) != 0)
				{
					//uses 'key' (at index -2) and 'value' (at index -1)
					if (lua_isnumber(L, -1))
					{
						if (fval.size() > 0)
							break;
						sval.push_back(lua_tostring(L, -1));
					}
					else if (lua_isstring(L, -1))
					{
						if (sval.size() > 0)
							break;
						fval.push_back((float)lua_tonumber(L, -1));
					}
					/* removes 'value'; keeps 'key' for next iteration */
					lua_pop(L, 1);
				}
				if (fval.size() > 0)
				{
					return VariantData(fval);
				}
				else if (sval.size() > 0)
				{
					return VariantData(sval);
				}
			}
			else
			{
				if (lua_isnumber(L, 1))
				{
					float val = (float)lua_tonumber(L, 1);
					lua_pop(L, 1);
					return VariantData(val);
				}
				else if (lua_isstring(L, 1))
				{
					std::string val = lua_tostring(L, 1);
					lua_pop(L, 1);
					return VariantData(val);
				}
			}
			return VariantData();
		}
		
		VariantData::VariantData()
			: _count(0), _t(TypeNull)
		{
			_ptr.intptr = 0;
		}
		
		VariantData::VariantData(int val)
			: _count(1), _t(TypeInt)
		{
			_ptr.intptr = new int;
			*_ptr.intptr = val;
		}
		
		VariantData::VariantData(unsigned int val)
			: _count(1), _t(TypeUInt)
		{
			_ptr.uintptr = new unsigned int;
			*_ptr.intptr = val;
		}
		
		VariantData::VariantData(float val)
			: _count(1), _t(TypeUInt)
		{
			_ptr.floatptr = new float;
			*_ptr.floatptr = val;
		}

		VariantData::VariantData(const std::string& val)
			: _count(1), _t(TypeString)
		{
			_ptr.stringptr = new std::string;
			*_ptr.stringptr = val;
		}
		
		VariantData::VariantData(const std::vector<int>& val)
			: _count(val.size()), _t(TypeInt)
		{
			_ptr.intptr = new int[_count];
			for (unsigned int n = 0;n < val.size();n++)
			{
				_ptr.intptr[n] = val[n];
			}
		}
		
		VariantData::VariantData(const std::vector<unsigned int>& val)
			: _count(val.size()), _t(TypeUInt)
		{
			_ptr.uintptr = new unsigned int[_count];
			for (unsigned int n = 0;n < val.size();n++)
			{
				_ptr.uintptr[n] = val[n];
			}
		}
		VariantData::VariantData(const std::vector<float>& val)
			: _count(val.size()), _t(TypeFloat)
		{
			_ptr.floatptr = new float[_count];
			for (unsigned int n = 0;n < val.size();n++)
			{
				_ptr.floatptr[n] = val[n];
			}
		}
		
		VariantData::VariantData(const std::vector<std::string>& val)
			: _count(val.size()), _t(TypeString)
		{
			_ptr.stringptr = new std::string[_count];
			for (unsigned int n = 0;n < val.size();n++)
			{
				_ptr.stringptr[n] = val[n];
			}
		}
	
		VariantData::~VariantData()
		{
			switch (_t)
			{
				case TypeInt:
					if (_count > 1)
						delete[] _ptr.intptr;
					else
						delete _ptr.intptr;
					break;
				case TypeUInt:
					if (_count > 1)
						delete[] _ptr.uintptr;
					else
						delete _ptr.uintptr;
					break;
				case TypeFloat:
					if (_count > 1)
						delete[] _ptr.floatptr;
					else
						delete _ptr.floatptr;
					break;
				case TypeString:
					if (_count > 1)
						delete[] _ptr.stringptr;
					else
						delete _ptr.stringptr;
					break;
				case TypeNull:
					break;
			}
		}
		
		unsigned int VariantData::count()
		{
			return _count;
		}
		
		VariantData::type VariantData::get_type()
		{
			return _t;
		}
		
		bool VariantData::is_type(VariantData::type t)
		{
			return _t == t;
		}
		
		int* VariantData::as_int()
		{
			return _ptr.intptr;
		}
		
		unsigned int* VariantData::as_uint()
		{
			return _ptr.uintptr;
		}
		
		float* VariantData::as_float()
		{
			return _ptr.floatptr;
		}
		
		std::string* VariantData::as_string()
		{
			return _ptr.stringptr;
		}
		
		void VariantData::push_to_lua(lua_State* L)
		{
			if (_count > 1)
			{
				lua_newtable(L);
				for (unsigned int n=0;n<_count;n++)
				{
					lua_pushnumber(L, n+1);
					switch (_t)
					{
						case TypeInt:
							lua_pushnumber(L, as_int()[n]);
							break;
						case TypeUInt:
							lua_pushnumber(L, as_uint()[n]);
							break;
						case TypeFloat:
							lua_pushnumber(L, as_float()[n]);
							break;
						case TypeString:
							lua_pushstring(L, as_string()[n].c_str());
							break;
						case TypeNull:
							break;
					}
					lua_settable(L, -3);
				}
			}
			else if (_count == 1)
			{
				switch (_t)
				{
					case TypeInt:
						lua_pushnumber(L, *as_int());
						break;
					case TypeUInt:
						lua_pushnumber(L, *as_uint());
						break;
					case TypeFloat:
						lua_pushnumber(L, *as_float());
						break;
					case TypeString:
						lua_pushstring(L, as_string()->c_str());
						break;
					case TypeNull:
						break;
				}
			}
			else
			{
				lua_pushnil(L);
			}
		}
		
		//
		// Object
		//
		
		Object::Object(unsigned int objectid, Game* game, Object* parent)
			: _objectid(objectid), _game(game), _parentid(parent->objectid()), _L(0)
		{
			sql::Connection* conn = _game->sql()->connect();
			sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT * FROM `Objects` WHERE `id` = ?");
			prep_stmt->setUInt(1, _objectid);
			sql::ResultSet* res = prep_stmt->executeQuery();
			
			if (res->rowsCount() != 1)
			{
				throw teh::Exceptions::InvalidObject(_objectid);
			}
			
			delete res;
			delete prep_stmt;
			delete conn;
		}
		
		Object::~Object()
		{
			if (_L)
				lua_close(_L);
		}
		
		unsigned int Object::objectid() const
		{
			return _objectid;
		}
		
		Object* Object::parent() const
		{
			return game()->get_object(_parentid);
		}
		
		std::vector<std::string> Object::available_verbs()
		{
			lua_getglobal(_L, "verbs");
			if (lua_isnil(_L, -1))
			{
				lua_pop(_L, 1);
				return std::vector<std::string>();
			}

			lua_pushlightuserdata(_L, this);

			lua_call(_L, 1, LUA_MULTRET);

			std::vector<std::string> out;
			for (unsigned int n=0;n<lua_gettop(_L);n++)
			{
				out.push_back(lua_tostring(_L, -1));
				lua_pop(_L, 1);
			}
			
			return out;
		}
		
		std::vector<std::string> Object::available_requests()
		{
			lua_getglobal(_L, "requests");
			if (lua_isnil(_L, -1))
			{
				lua_pop(_L, 1);
				return std::vector<std::string>();
			}

			lua_pushlightuserdata(_L, this);

			lua_call(_L, 1, LUA_MULTRET);

			std::vector<std::string> out;
			for (unsigned int n=0;n<lua_gettop(_L);n++)
			{
				out.push_back(lua_tostring(_L, -1));
				lua_pop(_L, 1);
			}
			
			return out;
		}
		
		VariantData Object::request(const std::string& name, Object* sender, VariantData data)
		{
			lua_getglobal(_L, name.c_str());
			if (lua_isnil(_L, -1))
			{
				lua_pop(_L, 1);
				return VariantData();
			}

			lua_pushlightuserdata(_L, this);
			lua_pushlightuserdata(_L, sender);
			data.push_to_lua(_L);

			lua_call(_L, 3, 1);

			VariantData out = VariantData::pop_from_lua(_L);
			
			return out;
		}
		
		std::vector<std::string> Object::available_messages()
		{
			lua_getglobal(_L, "messages");
			if (lua_isnil(_L, -1))
			{
				lua_pop(_L, 1);
				return std::vector<std::string>();
			}

			lua_pushlightuserdata(_L, this);

			lua_call(_L, 1, LUA_MULTRET);

			std::vector<std::string> out;
			for (unsigned int n=0;n<lua_gettop(_L);n++)
			{
				out.push_back(lua_tostring(_L, -1));
				lua_pop(_L, 1);
			}
			
			return out;
		}
		
		void Object::message(const std::string& name, Object* sender, VariantData data)
		{
			lua_getglobal(_L, name.c_str());
			if (lua_isnil(_L, -1))
			{
				lua_pop(_L, 1);
				return;
			}

			lua_pushlightuserdata(_L, this);
			lua_pushlightuserdata(_L, sender);
			data.push_to_lua(_L);

			lua_call(_L, 3, 0);
		}
		
		void Object::act(Action* action)
		{
			lua_getglobal(_L, "act");
			if (lua_isnil(_L, -1))
			{
				lua_pop(_L, 1);
				return;
			}
			
			lua_pushlightuserdata(_L, this);
			lua_pushlightuserdata(_L, action);
			
			lua_call(_L, 2, 0);
		}
		
		void Object::init(const std::string& scriptname)
		{
			std::string luafilename = "./lua/" + scriptname + ".lua";
			_L = luaL_newstate();
			luaL_openlibs(_L);
			setup_lua(_L);
			luaL_dofile(_L, luafilename.c_str());
		}
		
		void Object::setup_lua(lua_State* L)
		{
			//pass
		}
		
		Game* Object::game() const
		{
			return _game;
		}
	}	
}
