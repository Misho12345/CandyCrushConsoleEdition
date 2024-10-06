#include "CandyCrushGame.h"

#include <iostream>
#include <locale>

#if defined(__linux__) || defined(__APPLE__)
#include <termios.h>
#include <unistd.h>
#endif

#if !defined(_WIN32) && !defined(__linux__) && !defined(__APPLE__)
#error "Unsupported platform"
#endif


int main()
{
    std::locale::global(std::locale("en_US.UTF-8"));

    #ifdef _WIN32
    system("cls");
    #elif defined(__linux__) || defined(__APPLE__)

    system("clear");
    termios oldt{};
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;

    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    newt.c_cc[VMIN]  = 1;
    newt.c_cc[VTIME] = 0;
    #endif


    CandyCrushGame game(16, 16);
    game.run();

    #if defined(__linux__) || defined(__APPLE__)
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    #endif
}
