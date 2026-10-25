@echo off
setlocal enabledelayedexpansion

echo ========================================================
echo   Building Quick Kinetic Framework (QKF) Toolchain
echo ========================================================

if not exist "bin" mkdir bin
if not exist "build" mkdir build

echo [1/2] Compiling with CMake and Ninja/MinGW...
cd build
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
if %ERRORLEVEL% NEQ 0 (
    echo [CMake Error] Falling back to direct g++ compilation...
    cd ..
    g++ -std=c++20 -O2 -Iinclude src/compiler/*.cpp src/config/*.cpp src/cloner/*.cpp src/main_qkf.cpp -o bin/qkf.exe
    g++ -std=c++20 -O2 -Iinclude src/compiler/*.cpp src/config/*.cpp src/cloner/*.cpp src/main_cloner.cpp -o bin/qkf-cloner.exe
    goto check_bin
)

cmake --build . --config Release
cd ..

:check_bin
if exist "bin\qkf.exe" (
    echo.
    echo [SUCCESS] qkf.exe built successfully in bin\
)
if exist "bin\qkf-cloner.exe" (
    echo [SUCCESS] qkf-cloner.exe built successfully in bin\
)
echo ========================================================
