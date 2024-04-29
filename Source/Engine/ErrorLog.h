#pragma once
#include "windows.h"
#include "StringConverter.h"
class ErrorLog
{
public:
	static void Log(std::string message);
	static void Log(HRESULT hr, std::string message);
	static void Log(HRESULT hr, std::wstring message);
};