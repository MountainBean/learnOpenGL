cmake_minimum_required(VERSION 3.10)
project(LearnOpenGl)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_COMPILER "C:\\msys64\\ucrt64\\bin\\g++")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(glfw3 STATIC IMPORTED GLOBAL)
set_property(TARGET glfw3 PROPERTY
	     IMPORTED_LOCATION "$ENV{HOME}/source/glfw-3.4/build/src/libglfw3.a")

add_executable(${PROJECT_NAME} hellowindow.cpp glad.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE 
                      glfw3
                      -lopengl32
                      )
target_include_directories(${PROJECT_NAME} PUBLIC 
                          "include" ,
			  "$ENV{HOME}/source/glfw-3.4/include"
                          )
