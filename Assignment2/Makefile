CXX := g++
OUTPUT := sfmlgame
# if you need to manually specify your SFML install dir, do so here
SFML_DIR := /opt/homebrew/Cellar/sfml/2.6.1

# compiler and linker flags
CXX_FLAGS := -std=c++20 -Wno-unused-result
INCLUDES  := -I./src -I ./src/imgui -I$(SFML_DIR)/include
LDFLAGS   := -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -framework OpenGL -L$(SFML_DIR)/lib

# the source files for the ecs game engine
SRC_FILES := $(wildcard src/*.cpp src/imgui/*.cpp)
OBJ_FILES := $(SRC_FILES:.cpp=.o)

# all of these targets will be made if you just type make
all:$(OUTPUT)

# define the main executable requirements / command
$(OUTPUT):$(OBJ_FILES) Makefile
		$(CXX) $(OBJ_FILES) $(LDFLAGS) -o ./bin/$@
# specifies how the object files are compiled from cpp files
.cpp.o:
	$(CXX) -c $(CXX_FLAGS) $(INCLUDES) $< -o $@

# typing 'make clean' will remove all intermediate build files
clean: 
	rm -f $(OBJ_FILES) ./bin/sfmlgame

# typing 'make run' will compile and run the program
run: $(OUTPUT)
	cd bin && ./sfmlgame && cd ..
