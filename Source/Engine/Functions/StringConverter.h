#pragma once
#include <string>

class StringConverter
{
public:
	static std::wstring StringToWide(std::string aString);
	static const std::string FormatForJSONPath(std::string aRemoveBefore, std::string aPhrase);
};