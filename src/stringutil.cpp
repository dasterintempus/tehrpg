#include "stringutil.h"
#include <sha.h>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace teh
{
	std::string stringjoin(const std::vector<std::string>& list, const std::string& sep)
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
	
	std::vector<std::string> stringsplit(const std::string& str, const std::string& sep)
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
			else
				out.push_back(str.substr(pos, (str.size()-pos2)+sep.size()));
		}
		return out;
	}
	
	std::string hash_sha512(std::string input)
	{
		CryptoPP::SHA512 sha;
		unsigned char* buffer = new unsigned char[sha.DigestSize()];
		unsigned char* inputbytes = new unsigned char[input.size()];
		memcpy(inputbytes, input.c_str(), input.size());
		sha.CalculateDigest(buffer, inputbytes, input.size());
		char* hashedbytes = new char[sha.DigestSize()];
		memcpy(hashedbytes, buffer, sha.DigestSize());
		std::string hashed(hashedbytes, sha.DigestSize());
		delete[] buffer;
		delete[] inputbytes;
		delete[] hashedbytes;
		return hashed;
	}
	
	std::string to_hex(std::string input)
	{
		std::stringstream conv;
		
		conv << std::hex << std::setfill('0');
		
		for (std::size_t i = 0; i != input.size(); ++i)
		{
			unsigned int current_byte_number = static_cast<unsigned int>(static_cast<unsigned char>(input[i]));
			conv << std::setw(2) << current_byte_number;
		}
		
		return conv.str();
	}
}