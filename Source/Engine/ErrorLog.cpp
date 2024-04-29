#include "ErrorLog.h"
#include <comdef.h>
void ErrorLog::Log(std::string message)
{
	std::string errorMessage = "Error: " + message;
	MessageBoxA(NULL, errorMessage.c_str(), "Error", MB_ICONERROR);
}

void ErrorLog::Log(HRESULT hr, std::string message)
{
	_com_error error(hr);
	std::wstring errorMessage = L"Error: " + StringConverter::StringToWide(message) + L"\n" + error.ErrorMessage();
	MessageBoxW(NULL, errorMessage.c_str(), L"Error", MB_ICONERROR);
}

void ErrorLog::Log(HRESULT hr, std::wstring message)
{
	_com_error error(hr);
	std::wstring errorMessage = L"Error: " + message + L"\n" + error.ErrorMessage();
	MessageBoxW(NULL, errorMessage.c_str(), L"Error", MB_ICONERROR);
}
