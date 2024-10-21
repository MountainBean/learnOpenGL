CC = clang++

ifeq ($(OS),Windows_NT)
	detected_OS = Windows
	LINKS = C:/Users/Sam/source/glfw-3.4/build/src/libglfw3.a
	ARGS = -I C:/Users/Sam/source/glfw-3.4/include -v
	LIBS = -lgdi32
	TARGET = hellowindow.exe
	RM = del
else
	detected_OS := $(shell uname)  # same as "uname -s"
	ARGS = -I /opt/glfw-3.4/include /opt/glfw-3.4/build/src/libglfw3.a -lopengl32 -lGL -lglfw3 -lX11 -lpthread -lXrandr -lXi -ldl -v
	TARGET = hellowindow
	RM = rm
endif

SRCS = hellowindow.cpp glad.cpp


all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) -o $(TARGET) $(SRCS) $(ARGS) $(LINKS) $(LIBS)

clean:
	$(RM) -f *.o $(TARGET)

