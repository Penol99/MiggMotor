@echo off
:: Main Directory
@RD /S /Q "./Local"
@RD /S /Q "./.vs"
@RD /S /Q "./Temp"
@RD /S /Q "./Lib"
@RD /S /Q "./JSON"
@DEL "./*.sln" /Q

:: Bin
cd "./Bin"
@RD /S /Q "./settings"
@DEL "./*.dll" /Q
@DEL "./*.exe" /Q
@DEL "./*.pdb" /Q
@DEL "./imgui.ini" /Q
@DEL "./DebugLogs.txt" /Q
cd ".."

:: VS Project Files
@DEL "./*vcxproj*" /S /Q

