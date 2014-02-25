#pragma once

#include "exceptions.h"

#include <vector>
#include <string>
#include <lua.hpp>

namespace teh
{
	namespace RPG
	{
		class Game;
		class Action;
		
		class VariantData
		{
			public:
				enum type
				{
					TypeInt,
					TypeUInt,
					TypeFloat,
					TypeString,
					TypeNull,
				};
				
				static VariantData pop_from_lua(lua_State* L);
			
				VariantData();
				VariantData(int val);
				VariantData(unsigned int val);
				VariantData(float val);
				VariantData(const std::string& val);
				VariantData(const std::vector<int>& val);
				VariantData(const std::vector<unsigned int>& val);
				VariantData(const std::vector<float>& val);
				VariantData(const std::vector<std::string>& val);
			
				~VariantData();
				
				unsigned int count();
				
				type get_type();
				bool is_type(type t);
				
				int* as_int();
				unsigned int* as_uint();
				float* as_float();
				std::string* as_string();
				
				void push_to_lua(lua_State* L);
			private:
				unsigned int _count;
				union
				{
					int* intptr;
					unsigned int* uintptr;
					float* floatptr;
					std::string* stringptr;
				} _ptr;
				type _t;
		};
		
		class Object
		{
			public:
				Object(unsigned int objectid, Game* game, Object* parent=0);
				virtual ~Object();
				unsigned int objectid() const;
				Object* parent() const;
			
				virtual std::vector<std::string> available_verbs();
			
				virtual std::vector<std::string> available_requests();
				virtual VariantData request(const std::string& name, Object* sender, VariantData data=VariantData());
				virtual std::vector<std::string> available_messages();
				virtual void message(const std::string& name, Object* sender, VariantData data=VariantData());
			
				virtual void act(Action* action);
			protected:
				void init(const std::string& scriptname);
				virtual void setup_lua(lua_State* L);
				Game* game() const;
			private:
				unsigned int _objectid;
				Game* _game;
				unsigned int _parentid;
				lua_State* _L;
		};
	}
}
