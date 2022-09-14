@echo off
mkdir build
mkdir bin

cd build

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" x64
cl.exe                          ^
    /EHsc                       ^
    /W4 /WX                     ^
    /std:c++20                  ^
    "../src/main.cpp"           ^
    /link                       ^
    /out:"../bin/mpidemo.exe"