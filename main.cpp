#include "CandyCrushGame.h"

#ifdef UNIX
#include <termios.h>
#include <unistd.h>
#include <locale>
#endif

int main()
{
    #ifdef UNIX
    std::locale::global(std::locale("en_US.UTF-8"));
    system("clear");

    termios oldt{};
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;

    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    newt.c_cc[VMIN]  = 1;
    newt.c_cc[VTIME] = 0;
    #else
    system("cls");
    #endif

    CandyCrushGame game(16, 16);
    game.run();

    #ifdef UNIX
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    #endif
}
