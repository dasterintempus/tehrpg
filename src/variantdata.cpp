#include "variantdata.h"

namespace teh
{
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
}