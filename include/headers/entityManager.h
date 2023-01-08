#pragma once

#include <iostream>
#include <vector>
#include <algorithm>

#include <sdl/SDL.h>
#include <sdl/SDL_image.h>
#include <sdl/SDL_mixer.h>
#include <sdl/SDL_ttf.h>

using namespace std;

class Entity;
class Player;
class PowerUp;
class Enemy;
class Coin;

class EntityManager {
    private:
        int powerUpTimer = 0;
        int powerUpThreshold = 180; //3 seconds * 60 fps

        int enemyTimer = 0;
        int EnemyThreshold = 600; // 10 seconds * 60 fps

        Player* _player;
        vector<PowerUp*> _powerUps;
        vector<Enemy*> _enemies;
        vector<Coin*> _coins;

        EntityManager() {};
    public:
        int coinsCollected = 0;

        EntityManager(const EntityManager&) = delete;

        static EntityManager& get() {
            static EntityManager instance;
            return instance; 
        }
        
        void initPlayer(Player* player) { _player = player; }

        Player* getPlayer() { return _player; }
        vector<PowerUp*> getPowerUpList() { return _powerUps; }
        vector<Enemy*> getEnemiesList() { return _enemies; }
        vector<Coin*> getCoinsList() { return _coins; }

        void addToPowerUpList(PowerUp* powerUp) { _powerUps.push_back(powerUp) ; }
        void addToEnemiesList(Enemy* enemy) { _enemies.push_back(enemy); }
        void addToCoinsList(Coin* coin) { _coins.push_back(coin); }

        void removeFromPowerUpList(PowerUp* powerUp) { _powerUps.erase(remove(_powerUps.begin(), _powerUps.end(), powerUp), _powerUps.end()); }
        void removeFromEnemiesList(Enemy* enemy) { _enemies.erase(remove(_enemies.begin(), _enemies.end(), enemy), _enemies.end()); }
        void removeFromCoinsList(Coin* coin) { _coins.erase(remove(_coins.begin(), _coins.end(), coin), _coins.end()); }

        void updatePlayer();
        void updatePowerUps();
        void updateEnemies();
        void updateCoins();

        void updateEntities();
        void updateEntityEvents(SDL_Event event);
};