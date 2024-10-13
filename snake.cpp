#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <bits/stdc++.h>
#include <vector>
#include <cstdlib>
#include <ctime>

void renderText(SDL_Renderer *renderer, const char *message, int x, int y, SDL_Color color);
void renderStartButton(SDL_Renderer *renderer, int x, int y, int width, int height, SDL_Color textColor);
void renderExitButton(SDL_Renderer *renderer, int x, int y, int width, int height, SDL_Color textColor);
void renderGameOverButton(SDL_Renderer *renderer, int x, int y, int width, int height, SDL_Color textColor);
void renderRestartButton(SDL_Renderer *renderer, int x, int y, int width, int height, SDL_Color textColor);
void drawCircle(SDL_Renderer *renderer, int centerX, int centerY, int radius);
void displayGameOverScreen(SDL_Renderer *renderer, int score);
void GameStarted(SDL_Renderer *renderer);
void GameLoop(SDL_Renderer *renderer);
void cleanUp(SDL_Window *window, SDL_Renderer *renderer);

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
Mix_Music *backgroundMusic = nullptr;
TTF_Font *font = nullptr;
TTF_Font *score = nullptr;
TTF_Font *finalScore = nullptr;
TTF_Font *game_over = nullptr;
Mix_Chunk *eatingSound = nullptr;
Mix_Chunk *bonusEatingSound = nullptr;
Mix_Chunk *gameOverSound = nullptr;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

struct SnakeSegment
{
    int x, y;
};

using namespace std;

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

    font = TTF_OpenFont("Fonts/arial.ttf", 28);
    if (font == nullptr)
    {
        cout << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << endl;
        return false;
    }

    score = TTF_OpenFont("Fonts/score.otf", 18);
    if (score == nullptr)
    {
        cout << "Failed to load score font! SDL_ttf Error: " << TTF_GetError() << endl;
        return false;
    }

    game_over = TTF_OpenFont("Fonts/game_over.ttf", 28);
    if (game_over == nullptr)
    {
        cout << "Failed to load game over font! SDL_ttf Error: " << TTF_GetError() << endl;
        return false;
    }

    finalScore = TTF_OpenFont("Fonts/finalScore.otf", 30);
    if (finalScore == nullptr)
    {
        cout << "Failed to load final score font! SDL_ttf Error: " << TTF_GetError() << endl;
        return false;
    }

    eatingSound = Mix_LoadWAV("audio/eating_sound.wav");
    if (eatingSound == nullptr)
    {
        cout << "Failed to load eating sound effect! SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    }

    bonusEatingSound = Mix_LoadWAV("audio/bonus_eating_sound.mp3");
    if (bonusEatingSound == nullptr)
    {
        cout << "Failed to load eating sound effect! SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    }

    gameOverSound = Mix_LoadWAV("audio/game_over_sound.wav");
    if (eatingSound == nullptr)
    {
        cout << "Failed to load game over sound effect! SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    }

    return true;
}

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

void renderText(SDL_Renderer *renderer, const char *message, int x, int y, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderText_Solid(font, message, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dstrect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &dstrect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void scoreRenderText(SDL_Renderer *renderer, const char *message, int x, int y, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderText_Solid(score, message, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dstrect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &dstrect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void finalScoreRenderText(SDL_Renderer *renderer, const char *message, int x, int y, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderText_Solid(finalScore, message, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dstrect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &dstrect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void gameOverRenderText(SDL_Renderer *renderer, const char *message, int x, int y, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderText_Solid(game_over, message, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dstrect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &dstrect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

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

void renderStartButton(SDL_Renderer *renderer, int x, int y, int width, int height, SDL_Color textColor)
{
    SDL_Rect startRect = {x, y, width, height};
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &startRect);
    renderText(renderer, "Start Game", x + 30, y + 10, textColor);
}

void renderExitButton(SDL_Renderer *renderer, int x, int y, int width, int height, SDL_Color textColor)
{
    SDL_Rect exitRect = {x, y, width, height};
    SDL_SetRenderDrawColor(renderer, 139, 0, 0, 255);
    SDL_RenderFillRect(renderer, &exitRect);
    renderText(renderer, "Exit Game", x + 30, y + 10, textColor);
}

void renderGameOverButton(SDL_Renderer *renderer, int x, int y, int width, int height, SDL_Color textColor)
{
    SDL_Rect gameOverRect = {x, y, width, height};
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    SDL_RenderFillRect(renderer, &gameOverRect);
    gameOverRenderText(renderer, "Game Over", x + 30, y + 10, textColor);
}

void renderRestartButton(SDL_Renderer *renderer, int x, int y, int width, int height, SDL_Color textColor)
{
    SDL_Rect restartRect = {x, y, width, height};
    SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
    SDL_RenderFillRect(renderer, &restartRect);
    renderText(renderer, "Restart Game", x + 30, y + 10, textColor);
}

bool handleRestartButtonClick(int mouseX, int mouseY, int x, int y, int width, int height)
{
    return (mouseX > x && mouseX < x + width && mouseY > y && mouseY < y + height);
}

bool handleGameOverButtonClick(int mouseX, int mouseY, int x, int y, int width, int height)
{
    return (mouseX > x && mouseX < x + width && mouseY > y && mouseY < y + height);
}

bool handleStartButtonClick(int mouseX, int mouseY, int x, int y, int width, int height)
{
    return (mouseX > x && mouseX < x + width && mouseY > y && mouseY < y + height);
}

bool handleExitButtonClick(int mouseX, int mouseY, int x, int y, int width, int height)
{
    return (mouseX > x && mouseX < x + width && mouseY > y && mouseY < y + height);
}

bool isMouseOverButton(int mouseX, int mouseY, int buttonX, int buttonY, int buttonWidth, int buttonHeight)
{
    return (mouseX >= buttonX && mouseX <= buttonX + buttonWidth &&
            mouseY >= buttonY && mouseY <= buttonY + buttonHeight);
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

void displayGameOverScreen(SDL_Renderer *renderer, int score)
{
    bool gameOverRunning = true;
    SDL_Event event;

    loadAndRenderImage(renderer, "image/game_over_screen.png");

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};

    string scoreText = "Final  Score: " + to_string(score);
    finalScoreRenderText(renderer, scoreText.c_str(), SCREEN_WIDTH / 2 - 165, SCREEN_HEIGHT / 2 + 20 , black);

    int buttonWidth = 240, buttonHeight = 50;
    int restartX = SCREEN_WIDTH / 2 - 120;
    int restartY = SCREEN_HEIGHT / 2 +70;

    int exitX = SCREEN_WIDTH / 2 - 100;
    int exitY = SCREEN_HEIGHT / 2+150;

    SDL_Cursor *arrowCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    SDL_Cursor *handCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

    SDL_SetCursor(arrowCursor);

    renderRestartButton(renderer, restartX, restartY, buttonWidth, buttonHeight, black);
    renderExitButton(renderer, exitX, exitY, 200, buttonHeight, white);

    SDL_RenderPresent(renderer);

    while (gameOverRunning)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                gameOverRunning = false;
            }
            else if (event.type == SDL_MOUSEMOTION)
            {
                int mouseX = event.motion.x;
                int mouseY = event.motion.y;

                bool restartButton = isMouseOverButton(mouseX, mouseY, restartX, restartY, buttonWidth, buttonHeight);
                bool overExitButton = isMouseOverButton(mouseX, mouseY, exitX, exitY, 200, buttonHeight);

                if (restartButton || overExitButton)
                {
                    SDL_SetCursor(handCursor);
                }
                else
                {
                    SDL_SetCursor(arrowCursor);
                }
            }
            else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
            {
                int mouseX = event.button.x;
                int mouseY = event.button.y;

                if (handleRestartButtonClick(mouseX, mouseY, restartX, restartY, buttonWidth, buttonHeight))
                {
                    gameOverRunning = false;
                    SDL_FreeCursor(arrowCursor);
                    SDL_FreeCursor(handCursor);
                    GameStarted(renderer);
                }
                else if (handleExitButtonClick(mouseX, mouseY, exitX, exitY, 200, buttonHeight))
                {
                    gameOverRunning = false;
                }
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

    SDL_Texture *regularFoodTexture = IMG_LoadTexture(gameRenderer, "image/normal_fruit.png");
    if (regularFoodTexture == nullptr)
    {
        cout << "Failed to load regular food texture! SDL_image Error: " << IMG_GetError() << endl;
        return;
    }

    SDL_Texture *bonusFoodTexture = IMG_LoadTexture(gameRenderer, "image/bonus_fruit.png");
    if (bonusFoodTexture == nullptr)
    {
        cout << "Failed to load bonus food texture! SDL_image Error: " << IMG_GetError() << endl;
        return;
    }

    // Snake initialization
    vector<SnakeSegment> snake = {{SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}};
    int snakeVelocity = 10;
    int dirX = 1, dirY = 0;

    const int wallThickness = 20;

    // Regular food initialization
    SDL_Rect food = {rand() % ((SCREEN_WIDTH - wallThickness * 2) / snakeVelocity) * snakeVelocity + wallThickness,
                     rand() % ((SCREEN_HEIGHT - wallThickness * 2) / snakeVelocity) * snakeVelocity + wallThickness,
                     10, 10};

    bool bonusFoodActive = false;
    SDL_Point bonusFood;
    int bonusFoodRadius = 10;

    int score = 0;

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
            Mix_HaltMusic();
            Mix_PlayChannel(-1, gameOverSound, 0);

            bool gameOver = true;
            SDL_Color orange = {255, 165, 0, 255};

            SDL_Cursor *arrowCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
            SDL_Cursor *handCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

            SDL_SetCursor(arrowCursor);

            int buttonWidth = 200, buttonHeight = 50;
            int overX = SCREEN_WIDTH / 2 - 100;
            int overY = SCREEN_HEIGHT / 2 - 50;

            renderGameOverButton(gameRenderer, overX, overY, buttonWidth, buttonHeight, orange);
            SDL_RenderPresent(gameRenderer);

            SDL_Event event;
            while (gameOver)
            {
                while (SDL_PollEvent(&event))
                {
                    if (event.type == SDL_QUIT)
                    {
                        gameOver = false;
                    }
                    else if (event.type == SDL_MOUSEMOTION)
                    {
                        int mouseX = event.motion.x;
                        int mouseY = event.motion.y;

                        bool gameOverButton = isMouseOverButton(mouseX, mouseY, overX, overY, buttonWidth, buttonHeight);
                        if (gameOverButton)
                        {
                            SDL_SetCursor(handCursor);
                        }
                        else
                        {
                            SDL_SetCursor(arrowCursor);
                        }
                    }

                    else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
                    {
                        int mouseX = event.button.x;
                        int mouseY = event.button.y;

                        if (handleGameOverButtonClick(mouseX, mouseY, overX, overY, buttonWidth, buttonHeight))
                        {
                            gameOver = false;
                            SDL_FreeCursor(arrowCursor);
                            SDL_FreeCursor(handCursor);
                            displayGameOverScreen(gameRenderer, score);
                        }
                    }
                }
            }
            break;
        }
        for (size_t i = 1; i < snake.size(); i++)
        {
            if (newHead.x == snake[i].x && newHead.y == snake[i].y)
            {
                Mix_HaltMusic();
                Mix_PlayChannel(-1, gameOverSound, 0);

                bool gameOver = true;
                SDL_Color orange = {255, 165, 0, 255};

                SDL_Cursor *arrowCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
                SDL_Cursor *handCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

                SDL_SetCursor(arrowCursor);

                int buttonWidth = 200, buttonHeight = 50;
                int overX = SCREEN_WIDTH / 2 - 100;
                int overY = SCREEN_HEIGHT / 2 - 50;

                renderGameOverButton(gameRenderer, overX, overY, buttonWidth, buttonHeight, orange);
                SDL_RenderPresent(gameRenderer);

                SDL_Event event;
                while (gameOver)
                {
                    while (SDL_PollEvent(&event))
                    {
                        if (event.type == SDL_QUIT)
                        {
                            gameOver = false;
                            gameRunning = false;
                            return;
                        }
                        else if (event.type == SDL_MOUSEMOTION)
                        {
                            int mouseX = event.motion.x;
                            int mouseY = event.motion.y;

                            bool gameOverButton = isMouseOverButton(mouseX, mouseY, overX, overY, buttonWidth, buttonHeight);
                            if (gameOverButton)
                            {
                                SDL_SetCursor(handCursor);
                            }
                            else
                            {
                                SDL_SetCursor(arrowCursor);
                            }
                        }
                        else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
                        {
                            int mouseX = event.button.x;
                            int mouseY = event.button.y;

                            if (handleGameOverButtonClick(mouseX, mouseY, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 25, 200, 50))
                            {
                                gameOver = false;
                                gameRunning = false;
                                SDL_FreeCursor(arrowCursor);
                                SDL_FreeCursor(handCursor);
                                displayGameOverScreen(gameRenderer, score);
                                return;
                            }
                        }
                    }
                }

                break;
            }
        }

        if (!gameRunning)
        {
            break;
        }

        snake.insert(snake.begin(), newHead);

        if (newHead.x == food.x && newHead.y == food.y)
        {
            Mix_PlayChannel(-1, eatingSound, 0);

            food.x = rand() % ((SCREEN_WIDTH - wallThickness * 2) / snakeVelocity) * snakeVelocity + wallThickness;
            food.y = rand() % ((SCREEN_HEIGHT - wallThickness * 2) / snakeVelocity) * snakeVelocity + wallThickness;

            score += 5;

            static int foodCount = 0;
            foodCount++;
            if (foodCount % 5 == 0)
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

        SDL_SetRenderDrawColor(gameRenderer, 180, 180, 180, 0);
        SDL_Rect topWall = {0, 0, SCREEN_WIDTH, wallThickness + 2};
        SDL_Rect bottomWall = {0, SCREEN_HEIGHT - wallThickness, SCREEN_WIDTH, wallThickness};
        SDL_Rect leftWall = {0, 0, wallThickness, SCREEN_HEIGHT};
        SDL_Rect rightWall = {SCREEN_WIDTH - wallThickness, 0, wallThickness, SCREEN_HEIGHT};
        SDL_RenderFillRect(gameRenderer, &topWall);
        SDL_RenderFillRect(gameRenderer, &bottomWall);
        SDL_RenderFillRect(gameRenderer, &leftWall);
        SDL_RenderFillRect(gameRenderer, &rightWall);

        for (size_t i = 0; i < snake.size(); i++)
        {
            int colorIntensity = 200 - (int)(pow(i, 1.5) * 5);
            int glowIntensity = colorIntensity + 30;

            if (i == 0)
            {
                SDL_SetRenderDrawColor(gameRenderer, 0, glowIntensity, 0, 100);
                drawCircle(gameRenderer, snake[i].x + snakeVelocity / 2, snake[i].y + snakeVelocity / 2, snakeVelocity / 2 + 2);

                SDL_SetRenderDrawColor(gameRenderer, 128, 128, 128, 255);                                                        
                drawCircle(gameRenderer, snake[i].x + snakeVelocity / 2, snake[i].y + snakeVelocity / 2, snakeVelocity / 2 + 1); 

                SDL_SetRenderDrawColor(gameRenderer, 0, 255, 0, 255);
                drawCircle(gameRenderer, snake[i].x + snakeVelocity / 2, snake[i].y + snakeVelocity / 2, snakeVelocity / 2);

                SDL_SetRenderDrawColor(gameRenderer, 0, 0, 0, 255);                                                     
                SDL_RenderDrawPoint(gameRenderer, snake[i].x + snakeVelocity / 4, snake[i].y + snakeVelocity / 4);      
                SDL_RenderDrawPoint(gameRenderer, snake[i].x + (3 * snakeVelocity) / 4, snake[i].y + snakeVelocity / 4); 
            }
            else
            {
                SDL_SetRenderDrawColor(gameRenderer, 0, glowIntensity, 0, 100);
                drawCircle(gameRenderer, snake[i].x + snakeVelocity / 2, snake[i].y + snakeVelocity / 2, snakeVelocity / 2 + 2);

                SDL_SetRenderDrawColor(gameRenderer, 128, 128, 128, 255);                                                       
                drawCircle(gameRenderer, snake[i].x + snakeVelocity / 2, snake[i].y + snakeVelocity / 2, snakeVelocity / 2 + 1); 

                SDL_SetRenderDrawColor(gameRenderer, 0, colorIntensity, 0, 255); 
                drawCircle(gameRenderer, snake[i].x + snakeVelocity / 2, snake[i].y + snakeVelocity / 2, snakeVelocity / 2);
            }
        }

        SDL_Rect foodRect = {food.x, food.y, 15, 15};
        SDL_RenderCopy(gameRenderer, regularFoodTexture, nullptr, &foodRect);

        if (bonusFoodActive)
        {
            SDL_Rect bonusFoodRect = {bonusFood.x - bonusFoodRadius, bonusFood.y - bonusFoodRadius, 25, 25};
            SDL_RenderCopy(gameRenderer, bonusFoodTexture, nullptr, &bonusFoodRect);
        }

        SDL_Color black = {0, 0, 0, 255};
        string scoreText = "Score: " + to_string(score);

        int scoreX = 1;
        int scoreY = 1;
        scoreRenderText(gameRenderer, scoreText.c_str(), scoreX, scoreY, black);

        SDL_RenderPresent(gameRenderer);

        SDL_Delay(100);
    }

    SDL_DestroyTexture(regularFoodTexture);
    SDL_DestroyTexture(bonusFoodTexture);
    SDL_DestroyRenderer(gameRenderer);
    SDL_DestroyWindow(gameWindow);
}

void GameLoop(SDL_Renderer *renderer)
{
    SDL_Event e;
    bool quit = false;
    bool gameStarted = false;

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};

    int buttonWidth = 200, buttonHeight = 50;
    int startX = SCREEN_WIDTH / 2 - 100;
    int startY = SCREEN_HEIGHT / 2 - 50;

    int exitX = SCREEN_WIDTH / 2 - 100;
    int exitY = SCREEN_HEIGHT / 2 + 50;

    SDL_Cursor *arrowCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    SDL_Cursor *handCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

    SDL_SetCursor(arrowCursor);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    loadAndRenderImage(renderer, "image/cover_photo.png");

    renderStartButton(renderer, startX, startY, buttonWidth, buttonHeight, black);
    renderExitButton(renderer, exitX, exitY, buttonWidth, buttonHeight, white);

    SDL_RenderPresent(renderer); 

    while (!quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (e.type == SDL_MOUSEMOTION)
            {
                int mouseX = e.motion.x;
                int mouseY = e.motion.y;

                bool overStartButton = isMouseOverButton(mouseX, mouseY, startX, startY, buttonWidth, buttonHeight);
                bool overExitButton = isMouseOverButton(mouseX, mouseY, exitX, exitY, buttonWidth, buttonHeight);

                if (overStartButton || overExitButton)
                {
                    SDL_SetCursor(handCursor);
                }
                else
                {
                    SDL_SetCursor(arrowCursor);
                }
            }
            else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT)
            {
                int mouseX = e.button.x;
                int mouseY = e.button.y;

                if (handleStartButtonClick(mouseX, mouseY, startX, startY, buttonWidth, buttonHeight))
                {
                    gameStarted = true;
                    quit = true;
                }
                else if (handleExitButtonClick(mouseX, mouseY, exitX, exitY, buttonWidth, buttonHeight))
                {
                    quit = true;
                }
            }
        }
    }

    SDL_FreeCursor(arrowCursor);
    SDL_FreeCursor(handCursor);

    if (gameStarted)
    {
        GameStarted(renderer);
    }
}

void cleanUp(SDL_Window *window, SDL_Renderer *renderer)
{
    Mix_FreeChunk(gameOverSound);
    gameOverSound = nullptr;

    Mix_FreeChunk(bonusEatingSound);
    bonusEatingSound = nullptr;

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
    if (!initializeSDL(window, renderer))
    {
        cleanUp(window, renderer);
        return -1;
    }

    if (!loadAndRenderImage(renderer, "image/cover_photo.png"))
    {
        cleanUp(window, renderer);
        return -1;
    }

    if (!playBackgroundMusic("audio/background_music.mp3"))
    {
        cleanUp(window, renderer);
        return -1;
    }

    GameLoop(renderer);

    cleanUp(window, renderer);

    return 0;
}
