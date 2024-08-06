#include "Game.h"

// Should be updated to the correct paths when first used
// Windows

#define FONT_PATH "C:\\Projekty\\Cpp_Game_Engine\\Assignment2\\src\\resources\\fonts\\"
#define CONFIG_PATH "C:\\Projekty\\Cpp_Game_Engine\\Assignment2\\src\\resources\\config.txt"

// Mac
//#define FONT_PATH "/Users/wiktorziebka/Projects/Cpp_Game_Engine/Assignment2/src/resources/fonts/"
//#define CONFIG_PATH "/Users/wiktorziebka/Projects/Cpp_Game_Engine/Assignment2/src/resources/config.txt"


int main()
{
    Game g{CONFIG_PATH, FONT_PATH};
    g.run();
}