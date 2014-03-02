#pragma once

#include <vector>
#include <string>
#include <lua.hpp>

namespace teh
{
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
}