@echo off
setlocal
powershell.exe -NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass -File "%~dp0CopyClient.ps1" %*
exit /b %errorlevel%
