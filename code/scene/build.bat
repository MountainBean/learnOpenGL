@echo off

if [%1]==[] goto fail

:usage
SET CODEDIR="%cd%"
mkdir ..\..\build
pushd ..\..\build
rm %1.*
cl %CODEDIR%/%1.cpp %HOME%\OpenGL\src\glad.cpp glfw3.lib opengl32.lib user32.lib gdi32.lib shell32.lib /I..\include /I%HOME%/OpenGL/include /std:c++20 /EHsc /MDd /W4 /Zi /link /LIBPATH:%HOME%\OpenGL\src
popd
goto eof

:fail
@echo ERROR::BUILD::BAT: NO_FILE
@echo Usage: ./shdc filename (do not inlcude extention)

:eof
