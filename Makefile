LIBS = -L ./lib -lGL -lglfw3 -lGLEW -lpthread -lX11 -ldl -lXcursor -lXinerama -lXxf86vm -lXrandr
INCLUDE = -I ./include/
SRC = ./src/
DEPS = $(SRC)Shader.cpp $(SRC)VertexBuffer.cpp $(SRC)VertexArray.cpp $(SRC)IndexBuffer.cpp $(SRC)Renderer.cpp
BUILD = ./bin/

run: main
	$(BUILD)main

main: 
	g++ -std=c++11 -o $(BUILD)main $(INCLUDE) $(SRC)main.cpp $(DEPS) $(LIBS)
