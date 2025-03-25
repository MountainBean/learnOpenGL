@echo off

mkdir ..\..\build
pushd ..\..\build
cl ..\demos\colours\colours.cpp %HOME%\OpenGL\src\glad.cpp glfw3.lib opengl32.lib user32.lib gdi32.lib shell32.lib /I..\include /I%HOME%/OpenGL/include /std:c++20 /EHsc /MDd /W4 /Zi /link /LIBPATH:%HOME%\OpenGL\src
popd
