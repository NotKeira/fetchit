@echo off

:: Install FetchIt for Windows

set "INSTALL_DIR=%USERPROFILE%\AppData\Local\fetchit"
set "PATH_TO_ADD=%INSTALL_DIR%"

if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%"
if exist "..\fetchit.exe" (
  copy "..\fetchit.exe" "%INSTALL_DIR%\fetchit.exe"
  ) else if exist "..\fetchit" (
  copy "..\fetchit" "%INSTALL_DIR%\fetchit.exe"
  ) else (
  echo Error: Could not find fetchit executable
  exit /b 1
)

for /f "tokens=2*" %%a in ('reg query "HKCU\Environment" /v PATH 2^>nul') do set "CURRENT_PATH=%%b"

echo %CURRENT_PATH% | findstr /C:"%PATH_TO_ADD%" >nul
if errorlevel 1 (
  setx PATH "%PATH_TO_ADD%;%CURRENT_PATH%"
  echo Added fetchit to PATH. Please restart your terminal for changes to take effect.
  ) else (
  echo fetchit is already in PATH.
)

echo Installation complete. You can now run 'fetchit' from anywhere.
pause
