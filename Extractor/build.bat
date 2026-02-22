@echo off
REM Build GameMaker Extractor

echo Building GameMaker Extractor...
cd /d "%~dp0"

echo.
echo Restoring NuGet packages...
dotnet restore

if errorlevel 1 (
    echo Error: Failed to restore packages
    exit /b 1
)

echo.
echo Building Release version...
dotnet build -c Release

if errorlevel 1 (
    echo Error: Build failed
    exit /b 1
)

echo.
echo Build successful! 
echo.
echo To run:
echo   dotnet run <path_to_data.win> [output_directory]
echo.
echo To publish standalone executable:
echo   dotnet publish -c Release -r win-x64 --self-contained
echo.

pause
