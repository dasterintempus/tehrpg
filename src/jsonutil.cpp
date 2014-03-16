#include "jsonutil.h"
#include <iostream>
#include <fstream>

namespace teh
{
	Json::Value ReadJSONFile(const std::string& jsonfilename)
	{
		std::ifstream file(jsonfilename);
		Json::Reader r;
		Json::Value root;
		r.parse(file, root);
		return root;
	}

	void WriteJSONFile(const std::string& jsonfilename, const Json::Value& json)
	{
		std::ofstream file(jsonfilename);
		Json::StyledStreamWriter w;
		w.write(file, json);
	}
}