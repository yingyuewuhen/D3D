#include "Convert.h"
#include <locale.h>
#include <stdlib.h>

std::wstring Convert::str2Wstr(std::string str)
{
	setlocale(LC_ALL, "chs");
	const char *_source = str.c_str();

	size_t _Dsize = str.size() + 1;
	wchar_t *_dest = new wchar_t[_Dsize];
	wmemset(_dest, 0, _Dsize);

	size_t converted = 0;
	mbstowcs_s(&converted, _dest, _Dsize, _source, _TRUNCATE);

	std::wstring result = _dest;
	delete []_dest;
	setlocale(LC_ALL, "C");
	return result;
}
