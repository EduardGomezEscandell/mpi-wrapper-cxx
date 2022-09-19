
:: Settings
if not defined BUILD_TYPE set BUILD_TYPE=Release

:: Change this path to your installation of Visual Studio"
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" x64

cmake                                   ^
    -G"Ninja"                           ^
    -DCMAKE_BUILD_TYPE=%BUILD_TYPE%     ^
    -DPROJECT_ROOT=%cd%                 ^
    -B"build/%BUILD_TYPE%"              ^
    -DMPI_ENABLED="false"

cmake --build "build/%BUILD_TYPE%"
