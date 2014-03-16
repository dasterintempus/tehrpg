#pragma once

#include <json/json.h>
#include <string>

namespace teh
{
	Json::Value ReadJSONFile(const std::string& jsonfilename);
	void WriteJSONFile(const std::string& jsonfilename, const Json::Value& json);
}