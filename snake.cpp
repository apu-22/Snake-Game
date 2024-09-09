#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h> 
#include <iostream>

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
Mix_Music *backgroundMusic = nullptr; 

bool initializeSDL(SDL_Window *&window, SDL_Renderer *&renderer)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) 
    {
        cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << endl;
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << endl;
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    }

    window = SDL_CreateWindow("SDL Full-Window Image", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr)
    {
        cout << "Window could not be created! SDL Error: " << SDL_GetError() << endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << endl;
        return false;
    }

    return true;
}

//function to load image and texture
bool loadAndRenderImage(SDL_Renderer *renderer, const char *filePath)
{
    SDL_Texture *texture = IMG_LoadTexture(renderer, filePath);
    if (texture == nullptr)
    {
        cout << "Failed to load texture! SDL_image Error: " << IMG_GetError() << endl;
        return false;
    }

    SDL_SetRenderDrawColor(renderer, 205, 20, 205, 255);
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, texture, nullptr, nullptr);

    SDL_RenderPresent(renderer);

    SDL_DestroyTexture(texture);
    return true;
}

// Function to load and play background music
bool playBackgroundMusic(const char *musicPath)
{
    backgroundMusic = Mix_LoadMUS(musicPath);
    if (backgroundMusic == nullptr)
    {
        cout << "Failed to load background music! SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    }

    if (Mix_PlayMusic(backgroundMusic, -1) == -1)
    {
        cout << "Failed to play background music! SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    }

    return true;
}

//function to main gameloop
void GameLoop(SDL_Renderer *renderer)
{
    SDL_Event e;
    bool quit = false;

    while (!quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
        }
    }
}

void cleanUp(SDL_Window *window, SDL_Renderer *renderer)
{
    Mix_FreeMusic(backgroundMusic);
    backgroundMusic = nullptr;

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char *args[])
{
    //create window and render
    if (!initializeSDL(window, renderer))
    {
        cleanUp(window, renderer);
        return -1;
    }
    
    //snake game cover photo
    if (!loadAndRenderImage(renderer, "image/cover_photo.png"))
    {
        cleanUp(window, renderer);
        return -1;
    }

    // Play background music
    if (!playBackgroundMusic("audio/background_music.mp3"))
    {
        cleanUp(window, renderer);
        return -1;
    }

    //main gameloop
    GameLoop(renderer);

    cleanUp(window, renderer);

    return 0;
}
