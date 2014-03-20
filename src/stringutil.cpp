#include "stringutil.h"

namespace teh
{
	std::string stringjoin(const stringvector& list, const std::string& sep)
	{
		std::stringstream out;
		for (unsigned int n=0;n<list.size();n++)
		{
			if (n == list.size() - 1)
			{
				out << list[n];
			}
			else
			{
				out << list[n] << sep;
			}
		}
		return out.str();
	}
	
	std::vector<std::string> stringsplit(const std::string& str, const std::string& sep = " ")
	{
		std::vector<std::string> out;
		unsigned int pos = 0;
		while (true)
		{
			pos = str.find(sep, pos);
			if (pos == std::string::npos)
				break;
			unsigned int pos2 = str.find(sep, pos);
			if (pos2 == std::string::npos)
				out.push_back(str.substr(pos));
			else:
				out.push_back(str.substr(pos, (str.size()-pos2)+sep.size()));
		}
		return out;
	}
}