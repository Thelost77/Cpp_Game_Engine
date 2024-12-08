CXX := clang++
OUTPUT := sfmlgame
OUTPUT_DEBUG := sfmlgame_d
# if you need to manually specify your SFML install dir, do so here
SFML_DIR := /opt/homebrew/Cellar/sfml/2.6.2

# compiler and linker flags
CXX_FLAGS_RELEASE := -std=c++20 -Wno-unused-result
CXX_FLAGS_DEBUG := -g -std=c++20 -Wno-unused-result
INCLUDES  := -I./src -I ./src/imgui -I$(SFML_DIR)/include
LDFLAGS   := -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -framework OpenGL -L$(SFML_DIR)/lib

# the source files for the ecs game engine
SRC_FILES := $(wildcard src/*.cpp src/imgui/*.cpp)
OBJ_FILES := $(SRC_FILES:.cpp=.o)

# all of these targets will be made if you just type make
all:CXX_FLAGS := $(CXX_FLAGS_RELEASE)
all:$(OUTPUT)

# define the main executable requirements / command
$(OUTPUT):$(OBJ_FILES) Makefile
		$(CXX) $(OBJ_FILES) $(LDFLAGS) -o ./bin/$@

# define the main executable requirements / command
$(OUTPUT_DEBUG):$(OBJ_FILES) Makefile
		$(CXX) $(OBJ_FILES) $(LDFLAGS) -o ./bin/$@

# specifies how the object files are compiled from cpp files
.cpp.o:
	$(CXX) -c $(CXX_FLAGS) $(INCLUDES) $< -o $@

# typing 'make clean' will remove all intermediate build files
clean: 
	rm -f $(OBJ_FILES) ./bin/sfmlgame ./bin/sfmlgame_d

# typing 'make run' will compile and run the program
run: CXX_FLAGS := $(CXX_FLAGS_RELEASE)
run: $(OUTPUT)
	cd bin && ./sfmlgame && cd ..

debug: CXX_FLAGS := $(CXX_FLAGS_DEBUG)
debug: $(OUTPUT_DEBUG)
	cd bin && lldb ./sfmlgame_d && cd ..
