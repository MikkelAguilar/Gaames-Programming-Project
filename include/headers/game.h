#pragma once

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <random>

#include <sdl/SDL.h>
#include <sdl/SDL_image.h>
#include <sdl/SDL_mixer.h>
#include <sdl/SDL_ttf.h>

#include "utility.h"
#include "entityManager.h"
#include "entity.h"
#include "command.h"

using namespace std;

class Game {
    private:
        const int _fps = 60;
        const int _frameDelay = 1000 / _fps;
        Uint32 _frameStart;
        int _frameTime;

        int _screenWidth;
        int _screenHeight;
        int _speed = 5;
        SDL_Color black = {0, 0, 0};

        SDL_Window* _window;
        SDL_Renderer* _renderer;
        bool running = true;
        vector<SDL_KeyCode> movementKeys = {SDLK_w, SDLK_a, SDLK_s, SDLK_d};

        int powerUpTimer = 0;
        int powerUpThreshold = 3 * _fps;

        int enemyTimer = 0;
        int enemyThreshold = 5 * _fps;

        TTF_Font* font;
        SDL_Surface* surfaceMessage;
        SDL_Texture* message;
        SDL_Rect messagePosition;

        SDL_Texture* coin;
        SDL_Rect coinPosition;

        Mix_Music* backgroundMusic;
        bool musicPlaying = false;

        void gameLoop();
        void handleEvents();
        void handleSpawning();
        void handleUI();

        void spawnPowerUp(int type = 0);
        void spawnEnemy(int type = 0);

        bool collision(SDL_Rect a, SDL_Rect b);
        void display();
        void clear();
        void cleanUp();
    public:
        bool keys[323] = {false};
        SDL_Keycode lastKeyPressed;
        SDL_Keycode lastKeyReleased;

        Game(const char* title, int x, int y, int w, int h, Uint32 flags);
        ~Game() {};
        void run();
};