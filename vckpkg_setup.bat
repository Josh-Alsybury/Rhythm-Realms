@echo off
git submodule update --init --recursive
cd vcpkg
call bootstrap-vcpkg.bat
vcpkg integrate install
vcpkg install cpr
pause
