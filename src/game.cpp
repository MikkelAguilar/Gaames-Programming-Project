#include <headers/game.h>

Game::Game(const char* title, int x, int y, int w, int h, Uint32 flags) {
    SDL_Init(SDL_INIT_EVERYTHING);
    Mix_Init(0);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);

    if (TTF_Init() == -1) {
        cout << "Failed to Initialize" << endl;
    }

    _screenWidth = w;
    _screenHeight = h;

    _window = SDL_CreateWindow(title, x, y, _screenWidth, _screenHeight, flags);
    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    Player* _player = new Player(_renderer, 0, 0, 80, 80, _fps, _screenWidth, _screenHeight);
    EntityManager::get().initPlayer(_player);

    messagePosition.x = 50;
    messagePosition.y = _screenHeight - 60;
    messagePosition.w = 30;
    messagePosition.h = 50;
    font = loadFont("fonts/WorkSans-Black.ttf", 12);

    coinPosition.x = 10;
    coinPosition.y = _screenHeight - 60;
    coinPosition.w = 30;
    coinPosition.h = 50;
    coin = loadTexture(_renderer, "res/sprites/coin/coin.png");

    backgroundMusic = Mix_LoadMUS("audio/background.wav");

    if (!backgroundMusic) {
        std::cout << "Failed: " << Mix_GetError() << std::endl;
    }

    Mix_PlayMusic(backgroundMusic, -1);
    Mix_VolumeMusic(10);
    musicPlaying = true;

    spawnEnemy(0);

    SDL_SetRenderDrawColor(_renderer, 220, 220, 220, 255);
    SDL_RenderClear(_renderer);
    SDL_RenderPresent(_renderer);
 };

void Game::run() {
    gameLoop();
}

void Game::gameLoop() {
    while (running) {
        _frameStart = SDL_GetTicks();

        handleEvents();
        handleSpawning();
        EntityManager::get().updateEntities();
        handleUI();
        display();
        clear();

        _frameTime = SDL_GetTicks() - _frameStart;

        if (_frameDelay > _frameTime) {
            SDL_Delay(_frameDelay - _frameTime);
        }
    }
}

void Game::handleEvents() {
    SDL_Event event;
    SDL_PollEvent(&event);
    EntityManager::get().updateEntityEvents(event);
    
    if (event.type == SDL_QUIT) {
        running = false;
    }
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_1) {
            spawnPowerUp(0);
        }
        if (event.key.keysym.sym == SDLK_2) {
            spawnPowerUp(1);
        }
        if (event.key.keysym.sym == SDLK_3) {
            spawnPowerUp(2);
        }
        if (event.key.keysym.sym == SDLK_0) {
            spawnEnemy(0);
        }
        if (event.key.keysym.sym == SDLK_m) {
            if (musicPlaying) {
                Mix_PauseMusic();
                musicPlaying = false;
            }
            else {
                Mix_ResumeMusic();
                musicPlaying = true;
            }
        }
    }
}

void Game::handleSpawning() {
    powerUpTimer++;

    if (powerUpTimer >= powerUpThreshold) {
        int randomType = 0 + ( rand() % ( 2 - 0 + 1 ) );
        powerUpTimer = 0;
        spawnPowerUp(randomType);
    }

    /*enemyTimer++;

    if (enemyTimer >= enemyThreshold) {
        //int randomType = 0 + ( rand() % ( 2 - 0 + 1 ) );
        enemyTimer = 0;
        spawnEnemy(0);
    }*/
}

void Game::handleUI() {
    string text = to_string(EntityManager::get().coinsCollected);
    surfaceMessage = TTF_RenderText_Solid(font, text.c_str(), black); 
    message = SDL_CreateTextureFromSurface(_renderer, surfaceMessage);
    SDL_RenderCopy(_renderer, message, NULL, &messagePosition);

    SDL_RenderCopy(_renderer, coin, NULL, &coinPosition);
}

void Game::spawnPowerUp(int type) {
    PowerUp* powerUp;

    if (type == 0) {
        powerUp = new PowerUp(_renderer, 0, 0, 40, 40, _fps, _screenWidth, _screenHeight, "damage");
    }
    else if (type == 1) {
        powerUp = new PowerUp(_renderer, 0, 0, 40, 40, _fps, _screenWidth, _screenHeight, "armor");
    }
    else if (type == 2) {
        powerUp = new PowerUp(_renderer, 0, 0, 40, 40, _fps, _screenWidth, _screenHeight, "speed");
    }
    powerUp->setRandomLocation();
    EntityManager::get().addToPowerUpList(powerUp);
}

void Game::spawnEnemy(int type) {
    Enemy* enemy;

    if (type == 0) {
        enemy = new Enemy(_renderer, 50, 50, 80, 80, _fps, _screenWidth, _screenHeight);
    }
    
    enemy->setRandomLocation();
    EntityManager::get().addToEnemiesList(enemy);
}

void Game::display() {
    SDL_RenderPresent(_renderer);
}
void Game::clear() {
    SDL_RenderClear(_renderer);
}
void Game::cleanUp() {
    SDL_DestroyWindow(_window);
}

bool Game::collision(SDL_Rect a, SDL_Rect b) {
    if (a.y + a.h <= b.y) {
        return false;
    }
    if (a.y >= b.y + b.h) {
        return false;
    }
    if (a.x + a.w <= b.x) {
        return false;
    }
    if (a.x >= b.x + b.h) {
        return false;
    }
    return true;
}