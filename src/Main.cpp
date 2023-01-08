#include <headers/game.h>

using namespace std;

int main(int argc, char* argv[]) {
    Game game("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800, SDL_WINDOW_SHOWN);
    game.run();

    return 0;
}