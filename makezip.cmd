@echo off

set BUILD_64=%1
set BUILD_32=%2

if "%BUILD_64%" == "" (
    echo "Usage: makezip.cmd <path to 64-bit build> <path to 32-bit build>"
    exit /b 1
)
if "%BUILD_32%" == "" (
    echo "Usage: makezip.cmd <path to 64-bit build> <path to 32-bit build>"
    exit /b 1
)

md build-zip
md build-zip\x86
md build-zip\x64

copy kpisrla.txt build-zip\x86
copy kpisrla.txt build-zip\x64
copy LICENSE build-zip\x86
copy LICENSE build-zip\x64

copy %BUILD_32%\kpisrla\Release\kpisrla.kpi build-zip\x86
copy %BUILD_64%\kpisrla\Release\kpisrla.kpi build-zip\x64

cd build-zip

zip -9r ../kpisrla.zip .
