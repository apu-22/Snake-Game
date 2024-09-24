#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <bits/stdc++.h>
#include <vector>
#include <cstdlib>
#include <ctime>

struct SnakeSegment
{
    int x, y;
};

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
Mix_Music *backgroundMusic = nullptr;
TTF_Font *font = nullptr;
Mix_Chunk *eatingSound = nullptr;
Mix_Chunk *bonusEatingSound = nullptr;

bool initializeSDL(SDL_Window *&window, SDL_Renderer *&renderer)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) // Initialize audio along with video
    {
        cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << endl;
        return false;
    }

    // initialize SDL_Image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << endl;
        return false;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1)
    {
        cout << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << endl;
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

    // Load font
    font = TTF_OpenFont("Fonts/arial.ttf", 28);
    if (font == nullptr)
    {
        cout << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << endl;
        return false;
    }

    eatingSound = Mix_LoadWAV("audio/eating_sound.mp3");
    if (eatingSound == nullptr)
    {
        cout << "Failed to load eating sound effect! SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    }

    bonusEatingSound = Mix_LoadWAV("audio/bonus_eating_sound.mp3");
    if (eatingSound == nullptr)
    {
        cout << "Failed to load eating sound effect! SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    }

    return true;
}

bool loadAndRenderImage(SDL_Renderer *renderer, const char *filePath)
{
    SDL_Texture *texture = IMG_LoadTexture(renderer, filePath);
    if (texture == nullptr)
    {
        std::cout << "Failed to load texture! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }

    SDL_SetRenderDrawColor(renderer, 205, 20, 205, 255);
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, texture, nullptr, nullptr);

    SDL_RenderPresent(renderer);

    SDL_DestroyTexture(texture);
    return true;
}

// Function to render text to the screen
void renderText(SDL_Renderer *renderer, const char *message, int x, int y, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderText_Solid(font, message, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dstrect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &dstrect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
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

// Function to render the "Start Game" button
void renderStartButton(SDL_Renderer *renderer, int x, int y, int width, int height, SDL_Color textColor)
{
    SDL_Rect startRect = {x, y, width, height};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &startRect);
    renderText(renderer, "Start Game", x + 30, y + 10, textColor);
}

// Function to render the "Exit Game" button
void renderExitButton(SDL_Renderer *renderer, int x, int y, int width, int height, SDL_Color textColor)
{
    SDL_Rect exitRect = {x, y, width, height};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &exitRect);
    renderText(renderer, "Exit Game", x + 30, y + 10, textColor);
}

// Function to handle mouse click for the "Start Game" button
bool handleStartButtonClick(int mouseX, int mouseY, int x, int y, int width, int height)
{
    return (mouseX > x && mouseX < x + width && mouseY > y && mouseY < y + height);
}

// Function to handle mouse click for the "Exit Game" button
bool handleExitButtonClick(int mouseX, int mouseY, int x, int y, int width, int height)
{
    return (mouseX > x && mouseX < x + width && mouseY > y && mouseY < y + height);
}

void drawCircle(SDL_Renderer *renderer, int centerX, int centerY, int radius)
{
    for (int w = 0; w < radius * 2; w++)
    {
        for (int h = 0; h < radius * 2; h++)
        {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx * dx + dy * dy) <= (radius * radius))
            {
                SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }
}

void GameStarted(SDL_Renderer *gameRenderer)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Window *gameWindow = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gameWindow == nullptr)
    {
        cout << "Game window could not be created! SDL Error: " << SDL_GetError() << endl;
        return;
    }

    gameRenderer = SDL_CreateRenderer(gameWindow, -1, SDL_RENDERER_ACCELERATED);
    if (gameRenderer == nullptr)
    {
        cout << "Game renderer could not be created! SDL Error: " << SDL_GetError() << endl;
        SDL_DestroyWindow(gameWindow);
        return;
    }

    // Snake initialization
    vector<SnakeSegment> snake = {{SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}};
    int snakeVelocity = 10;
    int dirX = 1, dirY = 0;

    // Wall dimensions
    const int wallThickness = 20;

    // Regular food initialization
    SDL_Rect food = {rand() % ((SCREEN_WIDTH - wallThickness * 2) / snakeVelocity) * snakeVelocity + wallThickness,
                     rand() % ((SCREEN_HEIGHT - wallThickness * 2) / snakeVelocity) * snakeVelocity + wallThickness,
                     10, 10};

    // Bonus food initialization
    bool bonusFoodActive = false;
    SDL_Point bonusFood;
    int bonusFoodRadius = 10;

    // Score initialization
    int score = 0;

    // Main game loop
    bool gameRunning = true;
    SDL_Event e;

    while (gameRunning)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                gameRunning = false;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_UP:
                case SDLK_w:
                    if (dirY == 0)
                    {
                        dirX = 0;
                        dirY = -1;
                    }
                    break;
                case SDLK_DOWN:
                case SDLK_s:
                    if (dirY == 0)
                    {
                        dirX = 0;
                        dirY = 1;
                    }
                    break;
                case SDLK_LEFT:
                case SDLK_a:
                    if (dirX == 0)
                    {
                        dirX = -1;
                        dirY = 0;
                    }
                    break;
                case SDLK_RIGHT:
                case SDLK_d:
                    if (dirX == 0)
                    {
                        dirX = 1;
                        dirY = 0;
                    }
                    break;
                }
            }
        }

        SnakeSegment newHead = {snake[0].x + dirX * snakeVelocity, snake[0].y + dirY * snakeVelocity};

        if (newHead.x < wallThickness || newHead.x >= SCREEN_WIDTH - wallThickness ||
            newHead.y < wallThickness || newHead.y >= SCREEN_HEIGHT - wallThickness)
        {
            cout << "Game Over! Snake hit the wall." << endl;
            gameRunning = false;
        }

        snake.insert(snake.begin(), newHead);

        // Regular food is eaten and increment the food count
        if (newHead.x == food.x && newHead.y == food.y)
        {
            Mix_PlayChannel(-1, eatingSound, 0);

            food.x = rand() % ((SCREEN_WIDTH - wallThickness * 2) / snakeVelocity) * snakeVelocity + wallThickness;
            food.y = rand() % ((SCREEN_HEIGHT - wallThickness * 2) / snakeVelocity) * snakeVelocity + wallThickness;

            score += 5;

            static int foodCount = 0;
            foodCount++;
            if (foodCount % 5 == 0) // Spawn bonus food after every 5 regular foods
            {
                bonusFoodActive = true;
                bonusFood.x = rand() % (SCREEN_WIDTH - wallThickness * 2 - 2 * bonusFoodRadius) + wallThickness + bonusFoodRadius;
                bonusFood.y = rand() % (SCREEN_HEIGHT - wallThickness * 2 - 2 * bonusFoodRadius) + wallThickness + bonusFoodRadius;
            }
        }
        else
        {
            snake.pop_back();
        }

        // Check for collision with bonus food (if active)
        if (bonusFoodActive)
        {
            int distX = newHead.x - bonusFood.x;
            int distY = newHead.y - bonusFood.y;
            int distance = sqrt(distX * distX + distY * distY);

            if (distance < bonusFoodRadius + snakeVelocity / 2)
            {
                Mix_PlayChannel(-1, bonusEatingSound, 0);
                score += 10;
                bonusFoodActive = false;
            }
        }

        SDL_SetRenderDrawColor(gameRenderer, 100, 150, 200, 255);
        SDL_RenderClear(gameRenderer);

        // Draw walls
        SDL_SetRenderDrawColor(gameRenderer, 180, 180, 180, 0);
        SDL_Rect topWall = {0, 0, SCREEN_WIDTH, wallThickness + 10};
        SDL_Rect bottomWall = {0, SCREEN_HEIGHT - wallThickness, SCREEN_WIDTH, wallThickness};
        SDL_Rect leftWall = {0, 0, wallThickness, SCREEN_HEIGHT};
        SDL_Rect rightWall = {SCREEN_WIDTH - wallThickness, 0, wallThickness, SCREEN_HEIGHT};
        SDL_RenderFillRect(gameRenderer, &topWall);
        SDL_RenderFillRect(gameRenderer, &bottomWall);
        SDL_RenderFillRect(gameRenderer, &leftWall);
        SDL_RenderFillRect(gameRenderer, &rightWall);

        // Draw snake
        SDL_SetRenderDrawColor(gameRenderer, 0, 255, 0, 255);
        for (const SnakeSegment &segment : snake)
        {
            SDL_Rect rect = {segment.x, segment.y, snakeVelocity, snakeVelocity};
            SDL_RenderFillRect(gameRenderer, &rect);
        }

        // Draw normal food
        SDL_SetRenderDrawColor(gameRenderer, 255, 0, 0, 255);
        SDL_RenderFillRect(gameRenderer, &food);

        // Draw bonus food if active
        if (bonusFoodActive)
        {
            SDL_SetRenderDrawColor(gameRenderer, 0, 0, 255, 255); // Bonus food color (blue)
            drawCircle(gameRenderer, bonusFood.x, bonusFood.y, bonusFoodRadius);
        }

        SDL_Color white = {255, 255, 255, 255};
        string scoreText = "Score: " + to_string(score);

        // Render score within the top-left wall with a small offset
        int scoreX = 1;
        int scoreY = 1;
        renderText(gameRenderer, scoreText.c_str(), scoreX, scoreY, white);

        SDL_RenderPresent(gameRenderer);

        SDL_Delay(100);
    }

    SDL_DestroyRenderer(gameRenderer);
    SDL_DestroyWindow(gameWindow);
}

void GameLoop(SDL_Renderer *renderer)
{
    SDL_Event e;
    bool quit = false;
    bool gameStarted = false;

    SDL_Color white = {255, 255, 255, 255};

    // Button size and position
    int buttonWidth = 200, buttonHeight = 50;
    int startX = SCREEN_WIDTH / 2 - 100;
    int startY = SCREEN_HEIGHT / 2 - 50;

    int exitX = SCREEN_WIDTH / 2 - 100;
    int exitY = SCREEN_HEIGHT / 2 + 50;

    // Draw the initial screen (background and buttons)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    loadAndRenderImage(renderer, "image/cover_photo.png");

    // Render buttons
    renderStartButton(renderer, startX, startY, buttonWidth, buttonHeight, white);
    renderExitButton(renderer, exitX, exitY, buttonWidth, buttonHeight, white);

    SDL_RenderPresent(renderer); // Present the rendered content

    while (!quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT)
            {
                int mouseX = e.button.x;
                int mouseY = e.button.y;

                // Handle "Start Game" button click
                if (handleStartButtonClick(mouseX, mouseY, startX, startY, buttonWidth, buttonHeight))
                {
                    gameStarted = true;
                    quit = true; // Exit the loop and start the game
                }
                // Handle "Exit Game" button click
                else if (handleExitButtonClick(mouseX, mouseY, exitX, exitY, buttonWidth, buttonHeight))
                {
                    quit = true; // Exit the loop and end the program
                }
            }
        }
    }

    if (gameStarted)
    {
        // Start the game if the "Start Game" button was clicked
        GameStarted(renderer);
    }
}

void cleanUp(SDL_Window *window, SDL_Renderer *renderer)
{
    Mix_FreeChunk(eatingSound);
    eatingSound = nullptr;

    Mix_FreeMusic(backgroundMusic);
    backgroundMusic = nullptr;

    TTF_CloseFont(font);
    font = nullptr;

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char *args[])
{
    // create window and render
    if (!initializeSDL(window, renderer))
    {
        cleanUp(window, renderer);
        return -1;
    }

    // snake game cover photo
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

    // main gameloop

    GameLoop(renderer);

    cleanUp(window, renderer);

    return 0;
}
