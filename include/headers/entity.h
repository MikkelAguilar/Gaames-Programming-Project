#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <memory>
#include <cmath>

#include <headers/component.h>
#include <headers/entityManager.h>
#include <headers/utility.h>

#include <sdl/SDL.h>
#include <sdl/SDL_image.h>
#include <sdl/SDL_mixer.h>
#include <sdl/SDL_ttf.h>

using namespace std;

class Entity {
    protected:
        string _currentDirection = "down";
        SDL_Rect _rect;
        SDL_Rect _position;
        SDL_Texture* _currentTexture;
        SDL_Renderer* _renderer;
        SDL_RendererFlip _horizontalFlip = SDL_FLIP_HORIZONTAL;
        SDL_RendererFlip _verticalFlip = SDL_FLIP_VERTICAL;

        int _textureWidth;
        int _textureHeight;
        int _frameWidth;
        int _frameHeight;
        int _screenWidth;
        int _screenHeight;
        int _speed;
        int _damage;
        int _armor;
        int _fps;

        vector<unique_ptr<Component>> _components;
        void addComponent(unique_ptr<Component> component);
        void updateComponents(SDL_Renderer* renderer);

        void draw(SDL_Renderer* renderer);
        void handleEvents();
        void powerUpBoost(PowerUp* powerUp);

        friend class PlayerControlledMovement;
        friend class RandomMovement;
        friend class Animation;
        friend class Buffable;
        friend class RangedWeapon;
        friend class HealthBar;
        friend class CoinCollector;
    public:
        int startingHealth = 100;
        int health = 100;
        bool shooting = false;

        SDL_Event currentEvent;

        bool keys[323] = {false};
        SDL_Keycode lastKeyPressed;
        SDL_Keycode lastKeyReleased;
        
        bool collision(SDL_Rect otherRect);
        
        void moveUp();
        void moveDown();
        void moveLeft();
        void moveRight();

        void setRandomLocation();
        void resetTextureRect(int x = 0, int y = 0);

        virtual void update() = 0;
};

class Player : public Entity {
    private:
        int _numberOfSprites;
        int _animationSpeed;
    public:
        Player()=default;
        Player(SDL_Renderer* renderer, int x, int y, int w, int h, int fps, int screenWidth, int screenHeight);

        void update();
        void centerPlayerToScreen();
        void printStats();
};

class Enemy : public Entity {
    private:
        int _numberOfSprites;
        int _animationSpeed;
    public:
        Enemy()=default;
        Enemy(SDL_Renderer* renderer, int x, int y, int w, int h, int fps, int screenWidth, int screenHeight);

        void update();
        void printStats();
};

class PowerUp : public Entity {
    private:
        int _numberOfSprites;
        int _animationSpeed;
    public:
        string boostType;
        int damageBoost = 15;
        int armorBoost = 15;
        int speedBoost = 4;

        PowerUp()=default;
        PowerUp(SDL_Renderer* renderer, int x, int y, int w, int h, int fps, int screenWidth, int screenHeight, string type = "damage");

        void update();
};

class Coin : public Entity {
    private:
        int _numberOfSprites;
        int _animationSpeed;
        int coinsWorth = 1;
    public:
        Coin()=default;
        Coin(SDL_Renderer* renderer, int x, int y, int w, int h, int fps, int screenWidth, int screenHeight);

        void update();
};

class Projectile : public Entity {
    private:
        float _angle;
        float _xVel;
        float _yVel;
        string _projectileType;
    public:
        Projectile()=default;
        Projectile(SDL_Renderer* renderer, int x, int y, int w, int h, int screenWidth, int screenHeight, int target_x, int target_y, string type = "bullet");

        void update();
};