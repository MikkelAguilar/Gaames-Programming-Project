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

using namespace std;

SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* filepath);
TTF_Font* loadFont(const char* filepath, int size);