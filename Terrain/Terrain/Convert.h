#if !defined(__CONVERT_H__)
#define __CONVERT_H__

#include <string>
#include <sstream>
class Convert
{
public:
	static std::wstring str2Wstr(std::string str);
	static std::string Int2Str(int val)
	{
		std::string str;
		std::stringstream ss;
		ss << val;
		ss >> str;
		return str;
	}
};
#endif