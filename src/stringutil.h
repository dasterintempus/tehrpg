#pragma once
#include <string>
#include <sstream>
#include <vector>

namespace teh
{
	template <class T>
	bool is_numeric(const std::string& in)
	{
		std::stringstream conv;
		conv << in;
		T n;
		conv >> n;
		return !conv.fail();
	}
	
	template <class T>
	T to_numeric(const std::string& in)
	{
		std::stringstream conv;
		conv << in;
		T n;
		conv >> n;
		return n;
	}
	
	std::string stringjoin(const std::vector<std::string>& list, const std::string& sep = " ");
	std::vector<std::string> stringsplit(const std::string& str, const std::string& sep = " ");
	
	std::string hash_sha512(std::string input);
	std::string to_hex(std::string input);
}