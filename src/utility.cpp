#include <headers/utility.h>

SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* filepath) {
    SDL_Texture* tex = NULL;
    tex = IMG_LoadTexture(renderer, filepath);

    if (tex == NULL) {
        cout << "Image could not load from file path: " << filepath << " Error: " << SDL_GetError() << std::endl;
    }
    return tex;
}

TTF_Font* loadFont(const char* filepath, int size) {
    TTF_Font* font = NULL;
    font = TTF_OpenFont(filepath, size);

    if (font == NULL) {
        cout << "Font could not open from file path: " << filepath << " Error: " << SDL_GetError() << std::endl;
    }
    return font;
}