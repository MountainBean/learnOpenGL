CC = clang++

ifeq ($(OS),Windows_NT)
	detected_OS := Windows
	ARGS = -I "C:\Users\Sam\source\glfw-3.4\include" "C:\Users\Sam\source\glfw-3.4\build\src\libglfw3.a" -lopengl32
	TARGET = hellowindow.exe
else
	detected_OS := $(shell uname)  # same as "uname -s"
	ARGS = -I /opt/glfw-3.4/include /opt/glfw-3.4/build/src/libglfw3.a -lGL
	TARGET = hellowindow
endif

SRCS = hellowindow.cpp glad.cpp


all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(ARGS) -o $(TARGET) $(SRCS)

clean:
	rm -f *.o $(TARGET)

