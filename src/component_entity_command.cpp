#include <headers/component.h>
#include <headers/entity.h>
#include <headers/command.h>
#include <headers/entityManager.h>

/// 
///     ENTITYMANAGER CLASS
/// 

void EntityManager::updateEntities() {
    updatePlayer();
    updateEnemies();
    updatePowerUps();
    updateCoins();
}

void EntityManager::updatePlayer() {
    _player->update();
}

void EntityManager::updateEnemies() {
    for (Enemy* enemy : _enemies) {
        enemy->update();
    }
}

void EntityManager::updatePowerUps() {
    for (PowerUp* powerUp : _powerUps) {
        powerUp->update();
    }
}

void EntityManager::updateCoins() {
    for (Coin* coin : _coins) {
        coin->update();
    }
}

void EntityManager::updateEntityEvents(SDL_Event event) {
    _player->currentEvent = event;

    for (Enemy* enemy : _enemies) {
        enemy->currentEvent = event;
    }

    for (PowerUp* powerUp : _powerUps) {
        powerUp->currentEvent = event;
    }

    for (Coin* coin : _coins) {
        coin->currentEvent = event;
    }
}

/// 
///     ENTITY CLASS
/// 

void Entity::addComponent(unique_ptr<Component> component) {
    _components.push_back(move(component));
}

void Entity::updateComponents(SDL_Renderer* renderer) {
    for (const auto& component : _components)
    {
        component->update(this);
    }
}

void Entity::handleEvents() {
    if (currentEvent.type == SDL_KEYDOWN) {
        keys[currentEvent.key.keysym.sym] = true;
        lastKeyPressed = currentEvent.key.keysym.sym;
    }
    else if (currentEvent.type == SDL_KEYUP) {
        keys[currentEvent.key.keysym.sym] = false;
        lastKeyReleased = currentEvent.key.keysym.sym;
    }
}

void Entity::draw(SDL_Renderer* renderer) {
    if (_currentDirection == "left") {
        SDL_RenderCopyEx(renderer, _currentTexture, &_rect, &_position, 0, NULL, _horizontalFlip);
    }
    else {
        SDL_RenderCopy(renderer, _currentTexture, &_rect, &_position); //_currentTexture, _testTexture
    }
}

bool Entity::collision(SDL_Rect otherRect) {
    if (_position.y + _position.h <= otherRect.y) { //Top Side
        return false;
    }
    if (_position.y >= otherRect.y + otherRect.h) { //Bottom Side
        return false;
    }
    if (_position.x + _position.w <= otherRect.x) { //Left Side
        return false;
    }
    if (_position.x >= otherRect.x + otherRect.h) { //Right SIde
        return false;
    }
    return true;
}

void Entity::moveUp() {
    if (_position.y > 0 - _position.h / 4 && !shooting) {
        _position.y -= _speed;
    }
}

void Entity::moveDown() {
    if (_position.y < _screenHeight - (_position.h - (_position.h / 8)) && !shooting) {
        _position.y += _speed;
    }
}

void Entity::moveLeft() {
    if (_position.x > 0 - _position.w / 4 && !shooting) {
        _position.x -= _speed;
    }
}

void Entity::moveRight() {
    if (_position.x < _screenWidth - (_position.w - (_position.w / 4)) && !shooting) {
        _position.x += _speed;
    }
}

void Entity::powerUpBoost(PowerUp* powerUp) {
    if (powerUp->boostType == "damage") {
        _damage = powerUp->damageBoost;
    }
    else if (powerUp->boostType == "armor") {
        _armor = powerUp->armorBoost;
    }
    else if (powerUp->boostType == "speed") {
        _speed = powerUp->speedBoost;
    }
}

void Entity::setRandomLocation() {
    _position.x = 0 + ( rand() % ( (_screenWidth - _frameWidth) - 0 + 1 ) );
    _position.y = 0 + ( rand() % ( (_screenHeight - _frameHeight) - 0 + 1 ) );
}

void Entity::resetTextureRect(int x, int y) {
    _rect.x = x;
    _rect.y = y;
}

/// 
///     PLAYER CLASS
/// 

Player::Player(SDL_Renderer* renderer, int x, int y, int w, int h, int fps, int screenWidth, int screenHeight) {
    _renderer = renderer;

    vector<SDL_Texture*> texture1;
    vector<SDL_Texture*> texture2;
    vector<SDL_Texture*> texture3 = {};

    texture1.push_back(loadTexture(_renderer, "res/sprites/player/up/standing/standing.png"));
    texture1.push_back(loadTexture(_renderer, "res/sprites/player/down/standing/standing.png"));
    texture1.push_back(loadTexture(_renderer, "res/sprites/player/side/standing/standing.png"));
    texture1.push_back(loadTexture(_renderer, "res/sprites/player/side/standing/standing.png"));

    texture2.push_back(loadTexture(_renderer, "res/sprites/player/up/running/running.png"));
    texture2.push_back(loadTexture(_renderer, "res/sprites/player/down/running/running.png"));
    texture2.push_back(loadTexture(_renderer, "res/sprites/player/side/running/running.png"));
    texture2.push_back(loadTexture(_renderer, "res/sprites/player/side/running/running.png"));

    texture3.push_back(loadTexture(_renderer, "res/sprites/player/up/shooting/shooting.png"));
    texture3.push_back(loadTexture(_renderer, "res/sprites/player/down/shooting/shooting.png"));
    texture3.push_back(loadTexture(_renderer, "res/sprites/player/side/shooting/shooting.png"));
    texture3.push_back(loadTexture(_renderer, "res/sprites/player/side/shooting/shooting.png"));

    _numberOfSprites = 8;
    _animationSpeed = 5;

    _position.x = x;
    _position.y = y;
    _position.w = w;
    _position.h = h;

    _screenWidth = screenWidth;
    _screenHeight = screenHeight;

    _currentTexture = texture1[0];
    SDL_QueryTexture(texture1[0], NULL, NULL, &_textureWidth, &_textureHeight);
    _frameWidth = _textureWidth / _numberOfSprites;
    _frameHeight = _textureHeight;

    _rect.x = 0;
    _rect.y = 0;
    _rect.w = _frameWidth;
    _rect.h = _frameHeight;

    pair<int, int> _speedStats = {2, 4}; //First is base speed, Second is upgraded speed
    pair<int, int> _armorStats = {5, 15};
    pair<int, int> _damageStats = {5, 15}; // {5, 15}

    _speed = _speedStats.first;
    _damage = _damageStats.first;
    _armor = _armorStats.first;

    centerPlayerToScreen();

    unique_ptr<Component> moveable = make_unique<PlayerControlledMovement>(_renderer);
    unique_ptr<Component> animation = make_unique<Animation>(_renderer, fps, _animationSpeed, 1, texture1, texture2, texture3);
    unique_ptr<Component> buffable = make_unique<Buffable>(_renderer, _damageStats, _armorStats, _speedStats);
    unique_ptr<Component> rangedWeapon = make_unique<RangedWeapon>(_renderer, "gun");
    unique_ptr<Component> healthBar = make_unique<HealthBar>(_renderer);
    unique_ptr<Component> coinCollector = make_unique<CoinCollector>(_renderer);

    addComponent(move(moveable));
    addComponent(move(animation));
    addComponent(move(buffable));
    addComponent(move(rangedWeapon));
    addComponent(move(healthBar));
    addComponent(move(coinCollector));
}

void Player::update() {
    handleEvents();
    updateComponents(_renderer);
    draw(_renderer);
}

void Player::centerPlayerToScreen() {
    _position.x = (_screenWidth / 2) - _position.w / 2;
    _position.y = (_screenHeight / 2) - _position.h / 2;
}

void Player::printStats() {
    cout << "Damage: " << _damage << endl;
    cout << "Armor: " << _armor << endl;
    cout << "Speed: " << _speed << endl;
}

/// 
///     ENEMY CLASS
///

Enemy::Enemy(SDL_Renderer* renderer, int x, int y, int w, int h, int fps, int screenWidth, int screenHeight) {
    _renderer = renderer;
    _fps = fps;

    vector<SDL_Texture*> texture1;
    vector<SDL_Texture*> texture2;
    vector<SDL_Texture*> texture3 = {};

    texture1.push_back(loadTexture(_renderer, "res/sprites/catcus/up/standing/standing.png"));
    texture1.push_back(loadTexture(_renderer, "res/sprites/catcus/down/standing/standing.png"));
    texture1.push_back(loadTexture(_renderer, "res/sprites/catcus/side/standing/standing.png"));
    texture1.push_back(loadTexture(_renderer, "res/sprites/catcus/side/standing/standing.png"));

    texture2.push_back(loadTexture(_renderer, "res/sprites/catcus/up/running/running.png"));
    texture2.push_back(loadTexture(_renderer, "res/sprites/catcus/down/running/running.png"));
    texture2.push_back(loadTexture(_renderer, "res/sprites/catcus/side/running/running.png"));
    texture2.push_back(loadTexture(_renderer, "res/sprites/catcus/side/running/running.png"));

    _numberOfSprites = 11;
    _animationSpeed = 5;

    _position.x = x;
    _position.y = y;
    _position.w = w;
    _position.h = h;

    _screenWidth = screenWidth;
    _screenHeight = screenHeight;

    _currentTexture = texture1[0];
    SDL_QueryTexture(texture1[0], NULL, NULL, &_textureWidth, &_textureHeight);
    _frameWidth = _textureWidth / _numberOfSprites;
    _frameHeight = _textureHeight;

    _rect.x = 0;
    _rect.y = 0;
    _rect.w = _frameWidth;
    _rect.h = _frameHeight;

    pair<int, int> _speedStats = {1, 2}; //First is base speed, Second is upgraded speed
    pair<int, int> _armorStats = {5, 15};
    pair<int, int> _damageStats = {5, 15};

    _speed = _speedStats.first;
    _damage = _damageStats.first;
    _armor = _armorStats.first;

    unique_ptr<Component> moveable = make_unique<RandomMovement>(_renderer);
    unique_ptr<Component> animation = make_unique<Animation>(renderer, fps, _animationSpeed, 2, texture1, texture2, texture3);
    unique_ptr<Component> buffable = make_unique<Buffable>(renderer, _damageStats, _armorStats, _speedStats);
    unique_ptr<Component> healthBar = make_unique<HealthBar>(_renderer);

    addComponent(move(moveable));
    addComponent(move(animation));
    addComponent(move(buffable));
    addComponent(move(healthBar));
}

void Enemy::update() {
    if (health <= 0) {
        Coin* coin = new Coin(_renderer, _position.x + 20, _position.y + _position.h / 2, 40, 40, _fps, _screenWidth, _screenHeight);
        EntityManager::get().addToCoinsList(coin);
        EntityManager::get().removeFromEnemiesList(this);
    }

    handleEvents();
    updateComponents(_renderer);
    draw(_renderer);
}

/// 
///     POWERUP CLASS
///

PowerUp::PowerUp(SDL_Renderer* renderer, int x, int y, int w, int h, int fps, int screenWidth, int screenHeight, string type) {
    _renderer = renderer;

    if (type == "damage") {
        _currentTexture = loadTexture(_renderer, "res/sprites/power-up/damage/base/base.png");
        _numberOfSprites = 6;
    }
    else if (type == "armor") {
        _currentTexture = loadTexture(_renderer, "res/sprites/power-up/armor/base/base.png");
        _numberOfSprites = 6;
    }
    else if (type == "speed") {
        _currentTexture = loadTexture(_renderer, "res/sprites/power-up/speed/base/base.png");
        _numberOfSprites = 12;
    }

    _animationSpeed = 5;
    boostType = type;

    _position.x = x;
    _position.y = y;
    _position.w = w;
    _position.h = h;

    _screenWidth = screenWidth;
    _screenHeight = screenHeight;

    SDL_QueryTexture(_currentTexture, NULL, NULL, &_textureWidth, &_textureHeight);
    _frameWidth = _textureWidth / _numberOfSprites;
    _frameHeight = _textureHeight;

    _rect.x = 0;
    _rect.y = 0;
    _rect.w = _frameWidth;
    _rect.h = _frameHeight;

    vector<SDL_Texture*> textures;
    textures.push_back(_currentTexture);

    unique_ptr<Component> animation = make_unique<Animation>(_renderer, fps, _animationSpeed, 0, textures);
    addComponent(move(animation));
}

void PowerUp::update() {
    handleEvents();
    updateComponents(_renderer);
    draw(_renderer);
}

/// 
///     COIN CLASS
///

Coin::Coin(SDL_Renderer* renderer, int x, int y, int w, int h, int fps, int screenWidth, int screenHeight) {
    _renderer = renderer;

    _currentTexture = loadTexture(_renderer, "res/sprites/coin/coinAnimation.png");
    _numberOfSprites = 7;

    _animationSpeed = 5;

    _position.x = x;
    _position.y = y;
    _position.w = w;
    _position.h = h;

    _screenWidth = screenWidth;
    _screenHeight = screenHeight;

    SDL_QueryTexture(_currentTexture, NULL, NULL, &_textureWidth, &_textureHeight);
    _frameWidth = _textureWidth / _numberOfSprites;
    _frameHeight = _textureHeight;

    _rect.x = 0;
    _rect.y = 0;
    _rect.w = _frameWidth;
    _rect.h = _frameHeight;

    vector<SDL_Texture*> textures;
    textures.push_back(_currentTexture);

    unique_ptr<Component> animation = make_unique<Animation>(_renderer, fps, _animationSpeed, 0, textures);
    addComponent(move(animation));
}

void Coin::update() {
    handleEvents();
    updateComponents(_renderer);
    draw(_renderer);
}

/// 
///     PROJECTILE CLASS
///

Projectile::Projectile(SDL_Renderer* renderer, int x, int y, int w, int h, int screenWidth, int screenHeight, int target_x, int target_y, string type) {
    _speed = 4;

    _renderer = renderer;
    _projectileType = type;

    _position.x = x;
    _position.y = y;
    _position.w = w;
    _position.h = h;

    _angle = atan2(y - target_y, x - target_x);
    _xVel = cos(_angle) * _speed;
    _yVel = sin(_angle) * _speed;

    if (_projectileType == "bullet") {
        _currentTexture = loadTexture(_renderer, "res/sprites/bullet/normal_bullet.png");
    }
    else {
        cout << "Invalid Projectile Type" << endl;
    }

    SDL_QueryTexture(_currentTexture, NULL, NULL, &_frameWidth, &_frameHeight);

    _rect.x = 0;
    _rect.y = 0;
    _rect.w = _frameWidth;
    _rect.h = _frameHeight;
}

void Projectile::update() {
    _position.x -= _xVel;
    _position.y -= _yVel;

    SDL_RenderCopy(_renderer, _currentTexture, &_rect, &_position);
}

/// 
///     PLAYERCONTRLLEDMOVEMENT CLASS
/// 

void PlayerControlledMovement::update(Entity* entity) {
    _inputHandler.handleInput(entity);
}

/// 
///     RANDOMMOVEMENT CLASS
///

void RandomMovement::update(Entity* entity) {
    moveTimer += 1;

    if (moveTimer >= 60) {
        int randomType = 0 + ( rand() % ( 3 - 0 + 1 ) );
        if (randomType == 0) {
            entity->_currentDirection = "up";
        }
        else if (randomType == 1) {
            entity->_currentDirection = "down";
        }
        else if (randomType == 2) {
            entity->_currentDirection = "left";
        }
        else if (randomType == 3) {
            entity->_currentDirection = "right";
        }
        moveTimer = 0;
    }

    if (entity->_currentDirection == "up") {
        entity->moveUp();
    }
    else if (entity->_currentDirection == "down") {
        entity->moveDown();
    }
    else if (entity->_currentDirection == "right") {
        entity->moveRight();
    }
    else if (entity->_currentDirection == "left") {
        entity->moveLeft();
    }
}

/// 
///     ANIMATION CLASS
/// 

Animation::Animation(SDL_Renderer* renderer, int fps, int animationPerSecond, int type, vector<SDL_Texture*> texture1, vector<SDL_Texture*> texture2, vector<SDL_Texture*> texture3) {
    //Type 0 - Single sprite sheet animation with no sprites for each direction
    //Type 1 - Multiple sprite sheets for each direction/state + Animation for player control
    //Type 2 - Multiple sprite sheets for each direction/state + Animation for enemy control
    _renderer = renderer;

    _type = type;
    _fps = fps;
    _animationPerSecond = animationPerSecond;

    if (type == 1 || type == 2) {
        _texture1["up"] = texture1[0];
        _texture1["down"] = texture1[1];
        _texture1["right"] = texture1[2];
        _texture1["left"] = texture1[3];

        if (!texture2.empty()) {
            _texture2["up"] = texture2[0];
            _texture2["down"] = texture2[1];
            _texture2["right"] = texture2[2];
            _texture2["left"] = texture2[3];
        }

        if (!texture3.empty()) {
            _texture3["up"] = texture3[0];
            _texture3["down"] = texture3[1];
            _texture3["right"] = texture3[2];
            _texture3["left"] = texture3[3];
        }
    }
}

bool Animation::onlyMovementActivated(SDL_Keycode code, bool keysPressed[]) {
    if (keysPressed[code] == false) { return false; }
    vector<SDL_KeyCode> codes;
    codes = movementKeys;

    codes.erase(remove(codes.begin(), codes.end(), code), codes.end());

    for (auto c : codes) {
        if (keysPressed[c] == true) {
            return false;
        }
    }
    return true;
}

bool Animation::movementKeysNotActivated(bool keysPressed[]) {
    for (auto key : movementKeys) {
        if (keysPressed[key]) {
            return false;
        }
    }
    return true;
}

void Animation::handlePlayerAnimation(Entity* entity) {
    if (entity->shooting) {
        entity->_currentTexture = _texture3[entity->_currentDirection];
    }
    else {
        if (!movementKeysNotActivated(entity->keys)) {
            if (entity->lastKeyPressed == SDLK_w) { entity->_currentDirection = "up"; }
            else if (entity->lastKeyPressed == SDLK_s) { entity->_currentDirection = "down"; }
            else if (entity->lastKeyPressed == SDLK_a) { entity->_currentDirection = "left"; }
            else if (entity->lastKeyPressed == SDLK_d) { entity->_currentDirection = "right"; }

            if (onlyMovementActivated(SDLK_w, entity->keys)) { entity->_currentDirection = "up"; }
            else if (onlyMovementActivated(SDLK_s, entity->keys)) { entity->_currentDirection = "down"; }
            else if (onlyMovementActivated(SDLK_a, entity->keys)) { entity->_currentDirection = "left"; }
            else if (onlyMovementActivated(SDLK_d, entity->keys)) { entity->_currentDirection = "right"; }

            entity->_currentTexture = _texture2[entity->_currentDirection];
        }
        else {
            if (entity->lastKeyReleased == SDLK_w) { entity->_currentDirection = "up"; }
            else if (entity->lastKeyReleased == SDLK_s) { entity->_currentDirection = "down"; }
            else if (entity->lastKeyReleased == SDLK_a) { entity->_currentDirection = "left"; }
            else if (entity->lastKeyReleased == SDLK_d) { entity->_currentDirection = "right"; }

            entity->_currentTexture = _texture1[entity->_currentDirection];
        }
    }
}

void Animation::handleEnemyAnimation(Entity* entity) {
    entity->_currentTexture = _texture2[entity->_currentDirection];
}

void Animation::update(Entity* entity) {
    _frameTime++;

    if (_fps / _frameTime <= _animationPerSecond) {
        _frameTime = 0;
        entity->_rect.x += entity->_frameWidth;
        if (entity->_rect.x >= entity->_textureWidth) {
            entity->_rect.x = 0;
        }
    }

    if (_type == 1) {
        handlePlayerAnimation(entity);
    }
    else if (_type == 2) {
        handleEnemyAnimation(entity);
    }
}

///
///     RANGEDWEAPON CLASS
/// 

RangedWeapon::RangedWeapon(SDL_Renderer* renderer, string type) {
    _renderer = renderer;
    _type = type;

    _shootSound = Mix_LoadWAV("audio/gunshot.wav");
    if (!_shootSound) {
        cout << "Failed: " << Mix_GetError() << endl;
    }
    Mix_VolumeChunk(_shootSound, 70);
}

void RangedWeapon::update(Entity* entity) {
    reloadTimer++;
    updateProjectile(entity);

    if (entity->shooting) {
        delayTimer++;
        if (delayTimer >= delayThreshold) {
            entity->shooting = false;
        }
    }

    if (entity->currentEvent.type == SDL_MOUSEBUTTONDOWN && reloadTimer >= reloadThreshold) {
        entity->shooting = true;
        entity->resetTextureRect();
        reloadTimer = 0;
        delayTimer = 0;
        shoot(entity);
        Mix_PlayChannel(-1, _shootSound, 0);
    }
}

void RangedWeapon::updateProjectile(Entity* entity) {
    for (Projectile* proj : _projectiles) {
        proj->update();
        for (Enemy* enemy : EntityManager::get().getEnemiesList()) {
            if (enemy->collision(proj->_position)) {
                enemy->health -= entity->_damage;
                destroyProjectile(proj);
            }
        }
    }
}

void RangedWeapon::destroyProjectile(Projectile* projectile) {
    _projectiles.erase(remove(_projectiles.begin(), _projectiles.end(), projectile), _projectiles.end());
}

void RangedWeapon::shoot(Entity* entity) {
    Projectile* projectile;
    int x, y;
    int x_bullet_pos = entity->_position.x + (entity->_frameWidth);
    int y_bullet_pos = entity->_position.y + (entity->_frameHeight);
    SDL_GetMouseState(&x, &y);

    int x_diff = x - x_bullet_pos;
    int y_diff = y - y_bullet_pos;

    if (x_diff >= 0) {
        if (abs(x_diff) > abs(y_diff)) {
            entity->_currentDirection = "right";
        }
        else {
            if (y_diff >= 0) {
                entity->_currentDirection = "down";
            }
            else {
                entity->_currentDirection = "up";
            }
        }
    }
    else {
        if (abs(x_diff) > abs(y_diff)) {
            entity->_currentDirection = "left";
        }
        else {
            if (y_diff >= 0) {
                entity->_currentDirection = "down";
            }
            else {
                entity->_currentDirection = "up";
            }
        }
    }

    if (_type == "gun") {
        projectile = new Projectile(_renderer, x_bullet_pos, y_bullet_pos, 7, 7, entity->_screenWidth, entity->_screenHeight, x, y, "bullet");
    }
    else {
        cout << "Incorrect Projectile Type" << endl;
    }
    _projectiles.push_back(projectile);
}

///
///     BUFFABLE CLASS
/// 

Buffable::Buffable(SDL_Renderer* renderer, pair<int, int> damage, pair<int, int> armor, pair<int, int> speed) {
    _renderer = renderer;

    _damage = damage;
    _armor = armor;
    _speed = speed;

    _damageIndicator = loadTexture(_renderer, "res/sprites/power-up/damage/base/static.png");
    _armorIndicator = loadTexture(_renderer, "res/sprites/power-up/armor/base/static.png");
    _speedIndicator = loadTexture(_renderer, "res/sprites/power-up/speed/base/static.png");

    _damageIPosition.w = 15;
    _damageIPosition.h = 15;
    _armorIPosition.w = 15;
    _armorIPosition.h = 15;
    _speedIPosition.w = 15;
    _speedIPosition.h = 15;

    _damageBoostSound = Mix_LoadWAV("audio/damage.wav");
    if (!_damageBoostSound) {
        cout << "Failed: " << Mix_GetError() << endl;
    }

    _armorBoostSound = Mix_LoadWAV("audio/armor.wav");
    if (!_armorBoostSound) {
        cout << "Failed: " << Mix_GetError() << endl;
    }

    _speedBoostSound = Mix_LoadWAV("audio/speed.wav");
    if (!_speedBoostSound) {
        cout << "Failed: " << Mix_GetError() << endl;
    }

    Mix_VolumeChunk(_damageBoostSound, 80);
    Mix_VolumeChunk(_armorBoostSound, 80);
    Mix_VolumeChunk(_speedBoostSound, 80);
}

void Buffable::update(Entity* e) {
    drawIndicators(e);
    handleBoosts(e);
    for (PowerUp* powerUp : EntityManager::get().getPowerUpList()) {
        if (powerUp->collision(e->_position)) {
            if (powerUp->boostType == "damage") {
                Mix_PlayChannel(-1, _damageBoostSound, 0);
                _damageBoosted = true;
                _damageBoostTimer = 300;
                e->_damage = _damage.second;
            }
            else if (powerUp->boostType == "armor") {
                Mix_PlayChannel(-1, _armorBoostSound, 0);
                _armorBoosted = true;
                _armorBoostTimer = 300;
                e->_armor = _armor.second;
            }
            else if (powerUp->boostType == "speed") {
                Mix_PlayChannel(-1, _speedBoostSound, 0);
                _speedBoosted = true;
                _speedBoostTimer = 300;
                e->_speed = _speed.second;
            }
            EntityManager::get().removeFromPowerUpList(powerUp);
        }
    }

    /*for (Coin* coin : EntityManager::get().getCoinsList()) {
        if (coin->collision(e->_position)) {
            Mix_PlayChannel(-1, _coinCollectedSound, 0);
            EntityManager::get().coinsCollected += 1;
            EntityManager::get().removeFromCoinsList(coin);
        }
    }*/
}

void Buffable::drawIndicators(Entity* e) {
    if (_damageBoosted) {
        _damageIPosition.x = e->_position.x + 16;
        _damageIPosition.y = e->_position.y;
        SDL_RenderCopy(_renderer, _damageIndicator, NULL, &_damageIPosition);
    }
    if (_armorBoosted) {
        _armorIPosition.x = e->_position.x + 31;
        _armorIPosition.y = e->_position.y;
        SDL_RenderCopy(_renderer, _armorIndicator, NULL, &_armorIPosition);
    }
    if (_speedBoosted) {
        _speedIPosition.x = e->_position.x + 46;
        _speedIPosition.y = e->_position.y ;
        SDL_RenderCopy(_renderer, _speedIndicator, NULL, &_speedIPosition);
    }

}

void Buffable::handleBoosts(Entity* e) {
    if (_damageBoosted) {
        _damageBoostTimer -= 1;
        if (_damageBoostTimer <= 0) { 
            _damageBoosted = false;
            e->_damage = _damage.first;
        }
    }

    if (_armorBoosted) {
        _armorBoostTimer -= 1;
        if (_armorBoostTimer <= 0) { 
            _armorBoosted = false;
            e->_armor = _armor.first;
        }
    }

    if (_speedBoosted) {
        _speedBoostTimer -= 1;
        if (_speedBoostTimer <= 0) { 
            _speedBoosted = false;
            e->_speed = _speed.first;
        }
    }
}

/// 
///     COINCOLLECTOR CLASS
/// 

CoinCollector::CoinCollector(SDL_Renderer* renderer) {
    _renderer = renderer;

    _coinCollectedSound = Mix_LoadWAV("audio/coin.wav");
    if (!_coinCollectedSound) {
        cout << "Failed: " << Mix_GetError() << endl;
    }
    Mix_VolumeChunk(_coinCollectedSound, 80);
}

void CoinCollector::update(Entity* e) {
    for (Coin* coin : EntityManager::get().getCoinsList()) {
        if (coin->collision(e->_position)) {
            Mix_PlayChannel(-1, _coinCollectedSound, 0);
            EntityManager::get().coinsCollected += 1;
            EntityManager::get().removeFromCoinsList(coin);
        }
    }
}

/// 
///     HEALTHBAR CLASS
/// 

HealthBar::HealthBar(SDL_Renderer* renderer) {
    _renderer = renderer;

    _textures.push_back(loadTexture(_renderer, "res/sprites/healthbar/healthbar1.png"));
    _textures.push_back(loadTexture(_renderer, "res/sprites/healthbar/healthbar2.png"));
    _textures.push_back(loadTexture(_renderer, "res/sprites/healthbar/healthbar3.png"));
    _textures.push_back(loadTexture(_renderer, "res/sprites/healthbar/healthbar4.png"));
    _textures.push_back(loadTexture(_renderer, "res/sprites/healthbar/healthbar5.png"));
    _textures.push_back(loadTexture(_renderer, "res/sprites/healthbar/healthbar6.png"));

    _currentTexture = _textures[0];

    _healthBarPosition.w = 30;
    _healthBarPosition.h = 10;
}

void HealthBar::update(Entity* e) {
    int threshold = e->startingHealth / 6;

    if (e->health >= e->startingHealth - (threshold * 1)) {
        _currentTexture = _textures[5];
    }
    else if (e->health >= e->startingHealth - (threshold * 2)) {
        _currentTexture = _textures[4];
    }
    else if (e->health >= e->startingHealth - (threshold * 3)) {
        _currentTexture = _textures[3];
    }
    else if (e->health >= e->startingHealth - (threshold * 4)) {
        _currentTexture = _textures[2];
    }
    else if (e->health >= e->startingHealth - (threshold * 5)) {
        _currentTexture = _textures[1];
    }
    else if (e->health >= e->startingHealth - (threshold * 6)) {
        _currentTexture = _textures[0];
    }

    drawBar(e);
}

void HealthBar::drawBar(Entity* e) {
    _healthBarPosition.x = e->_position.x + (23);
    _healthBarPosition.y = e->_position.y + (e->_frameHeight * 2);

    SDL_RenderCopy(_renderer, _currentTexture, NULL, &_healthBarPosition);
}

/// 
///     INPUTHANDLER CLASS
/// 

void InputHandler::handleInput(Entity* entity) {
    if (entity->keys[SDLK_w]) { commandQueue.push_back(&_wKey); }
    if (entity->keys[SDLK_s]) { commandQueue.push_back(&_sKey); }
    if (entity->keys[SDLK_a]) { commandQueue.push_back(&_aKey); }
    if (entity->keys[SDLK_d]) { commandQueue.push_back(&_dKey); }

    if (!commandQueue.empty()) { executeCommands(entity); }
}

void InputHandler::executeCommands(Entity* entity) {
    while (!commandQueue.empty()) {
        commandQueue.back()->execute(entity);
        commandQueue.pop_back();
    }
}

/// 
///     MOVEMENT COMMANDS
/// 

void UpCommand::execute(Entity* entity) {
    entity->moveUp();
}

void DownCommand::execute(Entity* entity) {
    entity->moveDown();
}

void LeftCommand::execute(Entity* entity) {
    entity->moveLeft();
}

void RightCommand::execute(Entity* entity) {
    entity->moveRight();
}