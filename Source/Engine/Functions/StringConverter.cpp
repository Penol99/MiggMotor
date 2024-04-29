#include "StringConverter.h"

std::wstring StringConverter::StringToWide(std::string aString)
{
	std::wstring wideString(aString.begin(), aString.end());
	return wideString;
}

const std::string StringConverter::FormatForJSONPath(std::string aRemoveBefore, std::string aPhrase)
{
	size_t stringPos = aPhrase.find(aRemoveBefore);
	aPhrase.erase(0, stringPos);
	aPhrase.insert(0, "..\\");

	return aPhrase;
}
