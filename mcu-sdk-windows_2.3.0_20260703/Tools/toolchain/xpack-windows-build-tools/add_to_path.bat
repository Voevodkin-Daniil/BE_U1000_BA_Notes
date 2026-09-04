@echo off
setlocal

set "NEWPATH=%~dp0bin"

powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "$p=[Environment]::GetEnvironmentVariable('Path','Machine'); if (($p -split ';') -notcontains $env:NEWPATH) { [Environment]::SetEnvironmentVariable('Path',($p.TrimEnd(';')+';'+$env:NEWPATH),'Machine'); Write-Host 'Added to system PATH:' $env:NEWPATH } else { Write-Host 'Already in system PATH:' $env:NEWPATH }"

pause
