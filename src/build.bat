@echo off

set CCFLAGS=-std:c++17 -Od -nologo -MT -GR- -EHa- -Oi -W4 -wd4530 -FC -Z7
set CLFLAGS=opengl32.lib glfw3dll.lib glew32.lib

if not exist ..\build mkdir ..\build
pushd ..\build

cl %CCFLAGS% %CLFLAGS% ..\src\main.cpp
popd
