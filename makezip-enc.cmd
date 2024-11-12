@echo off

set BUILD_64=%1
set BUILD_32=%2

if "%BUILD_64%" == "" (
    echo "Usage: makezip.cmd <path to 64-bit build> <path to 32-bit build>"
    exit /b 1
)

zip -9j ../srlaenc_x64.zip %BUILD_64%\srlaenc\Release\srlaenc.exe
