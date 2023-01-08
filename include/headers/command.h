#pragma once

#include <iostream>
#include <vector>
#include <algorithm>

#include <headers/entityManager.h>

#include <sdl/SDL.h>
#include <sdl/SDL_image.h>
#include <sdl/SDL_mixer.h>
#include <sdl/SDL_ttf.h>

using namespace std;

class Entity;

class Command {
    public:
        virtual ~Command() {};
        virtual void execute(Entity* player) = 0;
};

class UpCommand : public Command {
    public:
        void execute(Entity* entity);
};

class DownCommand : public Command {
    public:
        void execute(Entity* entity);
};

class LeftCommand : public Command {
    public:
        void execute(Entity* entity);
};

class RightCommand : public Command {
    public:
        void execute(Entity* entity);
};

class InputHandler {
    private:
        UpCommand _wKey;
        DownCommand _sKey;
        LeftCommand _aKey;
        RightCommand _dKey;
        vector<Command*> commandQueue;
    public:
        InputHandler()=default;
        void handleInput(Entity* entity);
        void executeCommands(Entity* entity);
};