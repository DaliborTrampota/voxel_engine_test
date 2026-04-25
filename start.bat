@echo off

if not exist build (
    mkdir build
)

if "%1" == "-cc" (
    cmake --preset ninja
    move /y .\build-ninja\compile_commands.json .\build\compile_commands.json
    exit
)

if "%1" == "-vs" (
    start build/engine_test.slnx
    exit
)

if "%1" == "-conf" (
    cmake --preset vs2022
    exit
)

cmake --preset vs2022
start build/engine_test.slnx