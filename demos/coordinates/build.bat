@echo off

mkdir ..\..\build
pushd ..\..\build
cl ..\demos\coordinates\coordinates.cpp ..\src\glad.cpp glfw3.lib opengl32.lib user32.lib gdi32.lib shell32.lib /I..\include /IC:/Users/Sam/source/glfw-3.4/include /IC:/Users/Sam/source/glm/include /std:c++20 /EHsc /MDd /W4 /Zi /link /LIBPATH:C:\Users\Sam\source\glfw-3.4\build\src\Debug
popd
