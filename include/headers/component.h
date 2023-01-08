#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <algorithm>

#include <headers/command.h>
#include <headers/entityManager.h>
#include <headers/utility.h>

#include <sdl/SDL.h>
#include <sdl/SDL_image.h>
#include <sdl/SDL_mixer.h>
#include <sdl/SDL_ttf.h>

using namespace std;

class Entity;
class Player;
class PowerUp;
class Projectile;

class Component {
    protected:
        SDL_Renderer* _renderer;
    public:
        Component() {};
        virtual ~Component() {};
        virtual void update(Entity* e) = 0;
};

class PlayerControlledMovement : public Component {
    private:
        InputHandler _inputHandler;
    public:
        PlayerControlledMovement() {};;
        PlayerControlledMovement(SDL_Renderer* renderer) { _renderer = renderer; }
        ~PlayerControlledMovement() {};

        void update(Entity* e);
};

class RandomMovement : public Component {
    private:
        int moveTimer = 0;

        void changeDirection(Entity* e);
    public:
        RandomMovement() {};
        RandomMovement(SDL_Renderer* renderer) { _renderer = renderer; }
        ~RandomMovement() {};

        void update(Entity* e);
};

class RangedWeapon : public Component {
    private:
        int reloadTimer = 60;
        int reloadThreshold = 60;
        int delayTimer = 0;
        int delayThreshold = 40;
        string _type;

        vector<Projectile*> _projectiles;
        SDL_Texture* _texture;
        Mix_Chunk* _shootSound;

        void shoot(Entity* entity);
        void updateProjectile(Entity* entity);
        void destroyProjectile(Projectile* projectile);
    public:
        RangedWeapon() {};
        RangedWeapon(SDL_Renderer* renderer, string type = "gun");
        ~RangedWeapon() {};

        void update(Entity* entity);
        void handleInput();
};

class Animation : public Component {
    private:
        int _frameTime = 0;
        int _fps;
        int _animationPerSecond;
        int _type;
        
        vector<SDL_KeyCode> movementKeys = {SDLK_w, SDLK_a, SDLK_s, SDLK_d};

        map<string, SDL_Texture*> _texture1; //Base
        map<string, SDL_Texture*> _texture2; //Moving (If Exists)
        map<string, SDL_Texture*> _texture3; //Action (If Exists)

        bool movementKeysNotActivated(bool keysPressed[]);
        bool onlyMovementActivated(SDL_Keycode code, bool keysPressed[]);
        void handlePlayerAnimation(Entity* entity);
        void handleEnemyAnimation(Entity* entity);
    public:
        Animation() {};
        Animation(SDL_Renderer* renderer, int fps, int animationPerSecond, int type, vector<SDL_Texture*> texture1, vector<SDL_Texture*> texture2 = {}, vector<SDL_Texture*> texture3 = {});
        ~Animation() {};

        void update(Entity* e);
};

class Buffable : public Component {
    private:
        bool _damageBoosted = false;
        bool _armorBoosted = false;
        bool _speedBoosted = false;

        int _damageBoostTimer = 0;
        int _armorBoostTimer = 0;
        int _speedBoostTimer = 0;

        pair<int, int> _damage;
        pair<int, int> _armor;
        pair<int, int> _speed;

        SDL_Texture* _damageIndicator = nullptr;
        SDL_Texture* _armorIndicator = nullptr;
        SDL_Texture* _speedIndicator = nullptr;

        SDL_Rect _damageIPosition;
        SDL_Rect _armorIPosition;
        SDL_Rect _speedIPosition;

        Mix_Chunk* _damageBoostSound;
        Mix_Chunk* _armorBoostSound;
        Mix_Chunk* _speedBoostSound;
        Mix_Chunk* _coinCollectedSound;

        void handleBoosts(Entity* e);
        void drawIndicators(Entity* e);
    public:
        Buffable() {};
        Buffable(SDL_Renderer* renderer, pair<int, int> damage, pair<int, int> armor, pair<int, int> speed);
        ~Buffable() {};

        void update(Entity* e);
        
};

class CoinCollector : public Component {
    private:
        Mix_Chunk* _coinCollectedSound;                
    public:
        CoinCollector() {};
        CoinCollector(SDL_Renderer* renderer);
        ~CoinCollector() {};

        void update(Entity* e);
};

class HealthBar : public Component {
    private:
        SDL_Rect _healthBarPosition;
        vector<SDL_Texture*> _textures;
        SDL_Texture* _currentTexture;

        void drawBar(Entity* e);
    public:
        HealthBar() {};
        HealthBar(SDL_Renderer* renderer);
        ~HealthBar() {};

        void update(Entity* e);
};