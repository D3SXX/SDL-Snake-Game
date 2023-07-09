#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <chrono>
#include <thread>
#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <algorithm>
#include <windows.h>

int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;
int GRID_SIZE = 20;
int GRID_WIDTH = SCREEN_WIDTH / GRID_SIZE;
int GRID_HEIGHT = SCREEN_HEIGHT / GRID_SIZE;

struct Point
{
    int x;
    int y;
};

enum class Move
{
    none,
    up,
    down,
    left,
    right
};

enum class MenuOption
{
    start,
    scores,
    exit
};

class Game
{
public:
    Game();
    ~Game();
    void Run();

private:
    void Initialize();
    void ProcessInput();
    void Update();
    void Render();
    void GenerateFood();
    bool CheckCollision(int x, int y);
    void GameOver();
    void Reset();
    void Delay(int milliseconds);
    void ShowMenu();
    void ShowScores();
    void HandleMenuInput(MenuOption option);
    void RenderText(const std::string& text, int x, int y, const SDL_Color& color);


    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;
    bool inMenu;
    std::vector<Point> snake;
    Point food;
    Move direction;
    int score;
    TTF_Font* font;
};

Game::Game()
    : window(nullptr), renderer(nullptr), isRunning(false), inMenu(true), score(0), font(nullptr)
{
}

Game::~Game()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
}

void Game::Run()
{
    Initialize();
    ShowMenu();

    while (isRunning)
    {
        ProcessInput();
        Update();
        Render();
        Delay(100); // Delay for 0.5 seconds
    }
}

void Game::Initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return;
    }

    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (window == nullptr)
    {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        return;
    }

    isRunning = true;

    // Initialize random seed
    std::srand(static_cast<unsigned int>(std::time(0)));

    // Initialize SDL_ttf
    if (TTF_Init() == -1)
    {
        std::cerr << "Failed to initialize SDL_ttf: " << TTF_GetError() << std::endl;
        return;
    }

    // Load font
    font = TTF_OpenFont("C:\\Windows\\Fonts\\Times.ttf", 24);
    if (font == nullptr)
    {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return;
    }

    // Initialize snake
    snake.push_back({ GRID_WIDTH / 2, GRID_HEIGHT / 2 });
    direction = Move::none;

    // Generate initial food
    GenerateFood();

    // Delete existing scores file
    std::remove("scores.txt");
}

void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            isRunning = false;
        }
        else if (inMenu && event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
                case SDLK_1:
                    HandleMenuInput(MenuOption::start);
                    break;
                case SDLK_2:
                    HandleMenuInput(MenuOption::scores);
                    break;
                case SDLK_3:
                    HandleMenuInput(MenuOption::exit);
                    break;
            }
        }
        else if (!inMenu && event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
                case SDLK_UP:
                    if (direction != Move::down)
                        direction = Move::up;
                    break;
                case SDLK_DOWN:
                    if (direction != Move::up)
                        direction = Move::down;
                    break;
                case SDLK_LEFT:
                    if (direction != Move::right)
                        direction = Move::left;
                    break;
                case SDLK_RIGHT:
                    if (direction != Move::left)
                        direction = Move::right;
                    break;
                case SDLK_ESCAPE:
                    inMenu = true;
                    break;
            }
        }
    }
}

void Game::Update()
{
    if (inMenu)
        return;

    // Move the snake
    Point head = snake[0];
    switch (direction)
    {
        case Move::up:
            head.y--;
            break;
        case Move::down:
            head.y++;
            break;
        case Move::left:
            head.x--;
            break;
        case Move::right:
            head.x++;
            break;
        case Move::none:
            return;
    }

    // Check collision with the food
    if (head.x == food.x && head.y == food.y)
    {
        // Increase score
        score += 10;

        // Generate new food
        GenerateFood();
    }
    else
    {
        // Remove tail segment
        snake.pop_back();
    }

    // Check collision with the snake's body or the wall
    if (head.x < 0 || head.x >= GRID_WIDTH || head.y < 0 || head.y >= GRID_HEIGHT || CheckCollision(head.x, head.y))
    {
        GameOver();
        return;
    }

    // Insert new head segment
    snake.insert(snake.begin(), head);
}

void Game::Render()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (inMenu)
    {
        SDL_Color color = { 255, 255, 255, 255 };

        // Render menu options
        SDL_Surface* startSurface = TTF_RenderText_Solid(font, "1. Start Game", color);
        SDL_Surface* scoresSurface = TTF_RenderText_Solid(font, "2. View Scores", color);
        SDL_Surface* exitSurface = TTF_RenderText_Solid(font, "3. Exit", color);

        SDL_Texture* startTexture = SDL_CreateTextureFromSurface(renderer, startSurface);
        SDL_Texture* scoresTexture = SDL_CreateTextureFromSurface(renderer, scoresSurface);
        SDL_Texture* exitTexture = SDL_CreateTextureFromSurface(renderer, exitSurface);

        SDL_Rect startRect = { SCREEN_WIDTH / 2 - startSurface->w / 2, SCREEN_HEIGHT / 2 - startSurface->h, startSurface->w, startSurface->h };
        SDL_Rect scoresRect = { SCREEN_WIDTH / 2 - scoresSurface->w / 2, SCREEN_HEIGHT / 2, scoresSurface->w, scoresSurface->h };
        SDL_Rect exitRect = { SCREEN_WIDTH / 2 - exitSurface->w / 2, SCREEN_HEIGHT / 2 + exitSurface->h, exitSurface->w, exitSurface->h };

        SDL_RenderCopy(renderer, startTexture, nullptr, &startRect);
        SDL_RenderCopy(renderer, scoresTexture, nullptr, &scoresRect);
        SDL_RenderCopy(renderer, exitTexture, nullptr, &exitRect);

        SDL_FreeSurface(startSurface);
        SDL_FreeSurface(scoresSurface);
        SDL_FreeSurface(exitSurface);
        SDL_DestroyTexture(startTexture);
        SDL_DestroyTexture(scoresTexture);
        SDL_DestroyTexture(exitTexture);
    }
    else
    {
        // Render snake
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        for (const auto& segment : snake)
        {
            SDL_Rect rect = { segment.x * GRID_SIZE, segment.y * GRID_SIZE, GRID_SIZE, GRID_SIZE };
            SDL_RenderFillRect(renderer, &rect);
        }

        // Render food
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect foodRect = { food.x * GRID_SIZE, food.y * GRID_SIZE, GRID_SIZE, GRID_SIZE };
        SDL_RenderFillRect(renderer, &foodRect);

        // Render score
        SDL_Color color = { 255, 255, 255, 255 };
        std::string scoreText = "Score: " + std::to_string(score);
        SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText.c_str(), color);
        SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
        SDL_Rect scoreRect = { 10, 10, scoreSurface->w, scoreSurface->h };
        SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect);
        SDL_FreeSurface(scoreSurface);
        SDL_DestroyTexture(scoreTexture);
    }

    SDL_RenderPresent(renderer);
}

void Game::GenerateFood()
{
    int x, y;
    do
    {
        x = std::rand() % GRID_WIDTH;
        y = std::rand() % GRID_HEIGHT;
    } while (CheckCollision(x, y));

    food.x = x;
    food.y = y;
}

bool Game::CheckCollision(int x, int y)
{
    for (const auto& segment : snake)
    {
        if (segment.x == x && segment.y == y)
            return true;
    }
    return false;
}

void Game::GameOver()
{
    // Save the score to a file
    std::ofstream scoresFile("scores.txt", std::ios::app);
    if (scoresFile)
    {
        scoresFile << score << std::endl;
        scoresFile.close();
    }

    // Reset the game
    Reset();
}

void Game::Reset()
{
    snake.clear();
    snake.push_back({ GRID_WIDTH / 2, GRID_HEIGHT / 2 });
    direction = Move::none;
    score = 0;
    GenerateFood();
    inMenu = true;
}

void Game::Delay(int milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void Game::ShowMenu()
{
    inMenu = true;
    while (inMenu)
    {
        ProcessInput();
        Render();
    }
}

void Game::ShowScores()
{
    std::ifstream scoresFile("scores.txt");
    if (scoresFile)
    {
        std::vector<int> scores; // Vector to store the scores

        std::string line;
        while (std::getline(scoresFile, line))
        {
            int score = std::stoi(line);
            scores.push_back(score);
        }
        scoresFile.close();

        // Sort the scores in descending order
        std::sort(scores.begin(), scores.end(), std::greater<int>());

        // Display the sorted scores
        bool running = true;
        SDL_Event event;
        float i = 0.2; 
    while (running) {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            running = false;
        }
    }

    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Color color = { 255, 255, 255 };

    // Render "The leaderboard:" text
    std::string leaderboardText = "The leaderboard:";
    SDL_Surface* leaderboardSurface = TTF_RenderText_Solid(font, leaderboardText.c_str(), color);
    SDL_Texture* leaderboardTexture = SDL_CreateTextureFromSurface(renderer, leaderboardSurface);

    SDL_Rect leaderboardRect;
    leaderboardRect.x = (SCREEN_WIDTH - leaderboardSurface->w) / 2;
    leaderboardRect.y = 0.2;
    leaderboardRect.w = leaderboardSurface->w;
    leaderboardRect.h = leaderboardSurface->h;

    SDL_RenderCopy(renderer, leaderboardTexture, nullptr, &leaderboardRect);

    SDL_FreeSurface(leaderboardSurface);
    SDL_DestroyTexture(leaderboardTexture);

    // Render scores
    float yOffset = 0.2f;
    for (const auto& score : scores) {
        std::string t_score = std::to_string(score);
        SDL_Surface* surface = TTF_RenderText_Solid(font, t_score.c_str(), color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_Rect textRect;
        textRect.x = SCREEN_WIDTH / 2;
        textRect.y = static_cast<int>(SCREEN_HEIGHT * yOffset) + leaderboardRect.y + leaderboardRect.h; // Adjust the vertical position relative to the leaderboard text
        yOffset += 0.1f;
        textRect.w = surface->w;
        textRect.h = surface->h;

        SDL_RenderCopy(renderer, texture, nullptr, &textRect);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
            std::string exitText = "To exit, press the Escape button";
        SDL_Surface* exitSurface = TTF_RenderText_Solid(font, exitText.c_str(), color);
        SDL_Texture* exitTexture = SDL_CreateTextureFromSurface(renderer, exitSurface);

        SDL_Rect exitRect;
        exitRect.x = (SCREEN_WIDTH - exitSurface->w) / 2;
        exitRect.y = SCREEN_HEIGHT - exitSurface->h;  // Position at the bottom of the window
        exitRect.w = exitSurface->w;
        exitRect.h = exitSurface->h;

        SDL_RenderCopy(renderer, exitTexture, nullptr, &exitRect);

        SDL_FreeSurface(exitSurface);
        SDL_DestroyTexture(exitTexture);


    // Present the rendered frame
    SDL_RenderPresent(renderer);
}


    }
}
void Game::RenderText(const std::string& text, int x, int y, const SDL_Color& color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = { x, y, surface->w, surface->h };
    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
    SDL_DestroyTexture(texture);
}

void Game::HandleMenuInput(MenuOption option)
{
    switch (option)
    {
        case MenuOption::start:
            inMenu = false;
            break;
        case MenuOption::scores:
            ShowScores();
            break;
        case MenuOption::exit:
            isRunning = false;
            exit(1);
            break;
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    std::string commandLine(lpCmdLine);
    std::string resolutionArg("--resolution");
    size_t resolutionPos = commandLine.find(resolutionArg);
    if (resolutionPos != std::string::npos)
    {
        // Extract the resolution values
        size_t valuePos = resolutionPos + resolutionArg.length() + 1;
        std::string resolutionValue = commandLine.substr(valuePos);
        size_t xPosition = resolutionValue.find('x');
        std::string widthStr = resolutionValue.substr(0, xPosition);
        std::string heightStr = resolutionValue.substr(xPosition + 1);
        SCREEN_WIDTH = std::stoi(widthStr);
        SCREEN_HEIGHT = std::stoi(heightStr);
        GRID_WIDTH = SCREEN_WIDTH / GRID_SIZE;
        GRID_HEIGHT = SCREEN_HEIGHT / GRID_SIZE;
    }

    ShowWindow(GetConsoleWindow(), SW_HIDE);
    Game game;
    game.Run();
    return 0;
}

