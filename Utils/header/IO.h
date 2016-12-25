#pragma once

#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using std::wstring;
using std::vector;

inline auto ReadStringListFromFileW(LPCWSTR filePath)
{
	vector<wstring> wstr;

	std::wifstream ifs(filePath, std::ios::in);
	ifs.imbue(std::locale("ko-kr"));

	if (!ifs.is_open())
	{
		_DEBUG_ERROR("부적절한 경로이거나 잘못된 파일입니다!");
		ifs.close();
		return wstr;
	}

	while (!ifs.eof())
	{
		wstr.emplace_back();
		ifs >> wstr.back();
	}
	
	while (true)
	{
		if (wstr.back().length() > 0) break;
		wstr.pop_back();
	}

	ifs.close();
	return wstr;
}