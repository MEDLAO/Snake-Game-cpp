//
//  main.cpp
//  Snake-Game-C++
//
//  Created by MEDLAO on 31/12/2024.
//

#include <iostream>
#include <deque>
#include <utility>
#include <set>
#include <unordered_map>
#include <raylib.h>


const int screen_width = 1280;
const int screen_height = 800;
const int grid_size = 20; // Size of each cell
const int columns = screen_width / grid_size;
const int rows = screen_height / grid_size;
std::set<std::pair<int, int>> occupiedPositions;

Sound yellowSound;
Sound redSound;
Sound blueSound;
Sound failSound;
Music backgroundMusic;

bool musicStarted = false;
bool playerWon;
bool resetGame;


// Custom hash function for std::pair
struct pair_hash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& pair) const {
        return std::hash<T1>()(pair.first) ^ (std::hash<T2>()(pair.second) << 1);
    }
};

void displayFinalMessage(){
    occupiedPositions.clear();
    if (playerWon) {
        
        // Add coordinates for "YOU WIN" message
        std::vector<std::pair<int, int>> youWinCoords = {
            {5, 5}, {6, 5}, {7, 5}, // Y
            {9, 5}, {10, 5}, {11, 5}, // O
            {13, 5}, {14, 5}, {15, 5}, // U
            {5, 7}, {6, 7}, {7, 7}, // W
            {9, 7}, {10, 7}, {11, 7}, // I
            {13, 7}, {14, 7}, {15, 7} // N
        };
        
        for (const auto& coord : youWinCoords) {
            occupiedPositions.insert(coord);
        }
    } else {
        // Add coordinates for "GAME OVER" message
        std::vector<std::pair<int, int>> gameOverCoords = {
            {5, 10}, {6, 10}, {7, 10}, // G
            {9, 10}, {10, 10}, {11, 10}, // A
            {13, 10}, {14, 10}, {15, 10}, // M
            {17, 10}, {18, 10}, {19, 10}, // E
            {5, 12}, {6, 12}, {7, 12}, // O
            {9, 12}, {10, 12}, {11, 12}, // V
            {13, 12}, {14, 12}, {15, 12} // E R
        };
        
        for (const auto& coord : gameOverCoords) {
            occupiedPositions.insert(coord);
        }
    }
}


// Helper function to compare colors
bool areColorsEqual(const Color& c1, const Color& c2){
    return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b && c1.a == c2.a;
}

class Snake{
private:
    std::deque<std::pair<int, int>> snake;
    int speedX;
    int speedY;
public:
    Snake();
    
    // Getter
    const std::deque<std::pair<int, int>>& getSnake() const{
        return snake;
    }
    
    void init(){
        
        speedX = 0;
        speedY = 0;
        
        int startX = columns / 2;
        int startY = rows / 2;
        
        // Add three initial segments
        for (int i = 0; i < 3; i++) {
            snake.push_back({startX - i, startY});
            occupiedPositions.insert({startX - i, startY});
            
        }
    }
    
    void draw(){
        for (const auto& segment : snake) {
            int pixelX = segment.first * grid_size;
            int pixelY = segment.second * grid_size;
            DrawRectangle(pixelX, pixelY, grid_size, grid_size, GREEN);
        }
    }
    
    void update(){
        handleInput();
        if ((speedX != 0 || speedY != 0)) {
            musicStarted = true;
        }
        
        int newX = snake.front().first + speedX;
        int newY = snake.front().second + speedY;
        
        // Wrapping screen logic
        if (newX < 0) {
            newX = columns - 1;
        }
        else if (newX >= columns){
            newX = 0;
        }
        
        if (newY < 0) {
            newY = rows - 1;
        }
        else if (newY >= rows){
            newY = 0;
        }
        
        auto tail = snake.back();
        snake.push_front({newX, newY});
        occupiedPositions.insert({newX, newY});
        snake.pop_back();
        occupiedPositions.erase(tail);
        
        selfCollision();
        
        if (resetGame) {
            reset();
            resetGame = false;
        }
        
    }
    
    void handleInput(){
        if (IsKeyDown(KEY_RIGHT) && speedX != -1) {
            speedX = 1;
            speedY = 0;
        }
        else if (IsKeyDown(KEY_LEFT) && speedX != 1) {
            speedX = -1;
            speedY = 0;
        }
        else if (IsKeyDown(KEY_DOWN) && speedY != -1) {
            speedX = 0;
            speedY = 1;
        }
        else if (IsKeyDown(KEY_UP) && speedY != 1) {
            speedX = 0;
            speedY = -1;
        }
        
        if (IsKeyDown(KEY_SPACE)) {
            resetGame = true; // flag for reset logic
        }
    }
    
    bool ateFood();
    
    void reset();
    
    void checkAndGrow(){
        if (ateFood()) {
            const auto& tail = snake.back();
            snake.push_back({tail.first, tail.second});
            occupiedPositions.insert(tail);
            
        }
    }
    
    
    void selfCollision(){
        // Get the head of the snake
        const auto& head = snake.front();
        
        // Iterate over the body excluding the head
        for (auto it = snake.begin() + 1; it != snake.end(); ++it) {
            if (head.first == it->first && head.second == it->second) {
                playerWon = false;
                displayFinalMessage();
                reset();
            }
        }
    }
};

Snake snake;

class Food{
private:
    const int foodCount = 120;
    const std::vector<Color> colors = {YELLOW, RED, BLUE};
    
public:
    Color lastFoodColor = BLACK;
    std::unordered_map<std::pair<int, int>, Color, pair_hash> foodItems;
    std::unordered_map<std::pair<int, int>, Color, pair_hash> initialFoodItems;
    
    void generateFood(){
        int colorIndex = 0;
        while (foodItems.size() < foodCount) {
            int x = GetRandomValue(0, columns - 1);
            int y = GetRandomValue(0, rows - 1);
            
            // Ensure no overlap with snake or existing food
            if (occupiedPositions.find({x, y}) == occupiedPositions.end()) {
                Color color = colors[colorIndex % colors.size()];
                foodItems[{x, y}] = color;
                initialFoodItems[{x, y}] = color; // Store in the initial state
                occupiedPositions.insert({x, y});
                colorIndex++; // Cycle through colors
            }
        }
    }
    
    void reset() {
        lastFoodColor = BLACK;
        foodItems.clear();
        for (const auto& item : initialFoodItems) {
            foodItems[item.first] = item.second;
            occupiedPositions.insert(item.first);
        }
    }
    
    void draw(){
        int i = 0;
        for (const auto& [position, color] : foodItems) {
            int pixelX = position.first * grid_size;
            int pixelY = position.second * grid_size;
            DrawRectangle(pixelX, pixelY , grid_size, grid_size, color);
            i++;
        }
    }
};

Food food;

// Initialize Snake with default speed and starting segments
Snake::Snake(){
    init();
}

bool Snake::ateFood(){
    const auto& head = snake.front(); // Read-only access
    for (auto it = food.foodItems.begin(); it != food.foodItems.end(); ++it) {
        if (it->first == head) { // Compare the key (position) with the snake's head
            if (areColorsEqual(it->second, food.lastFoodColor)){
                reset();
                return false;
            } else {
                food.lastFoodColor = it->second;
                food.foodItems.erase(it);
                // Play one of the three sounds randomly
                int randomIndex = GetRandomValue(0, 2);
                if (randomIndex == 0) {
                    PlaySound(yellowSound);
                } else if (randomIndex == 1) {
                    PlaySound(redSound);
                } else if (randomIndex == 2) {
                    PlaySound(blueSound);
                }
                
                return true;
            }
        }
    }
    return false;
}

void Snake::reset(){
    snake.clear();
    occupiedPositions.clear();
    init();
    food.reset();
}


int main(int argc, const char * argv[]) {
    
    InitWindow(screen_width, screen_height, "Snake Game C++");
    InitAudioDevice();
    SetTargetFPS(60);
   
    yellowSound = LoadSound("sound1.wav");
    redSound = LoadSound("sound2.wav");
    blueSound = LoadSound("sound3.wav");
    failSound = LoadSound("fail.wav");
    
    Music backgroundMusic = LoadMusicStream("background.wav");
    
    
    float updateInterval = 0.1f; // Time between snake updates
    float elapsedTime = 0.0f; // Accumulator for delta time
    
   
    
    food.generateFood();
    
    while (WindowShouldClose() == false) {
        
        UpdateMusicStream(backgroundMusic);
        if (musicStarted == true) {
            PlayMusicStream(backgroundMusic);
        }
        float deltaTime = GetFrameTime();
        elapsedTime += deltaTime;
        
        if (elapsedTime >= updateInterval) {
            snake.update(); // Move the snake
            snake.checkAndGrow();
         
            elapsedTime = 0.0f; // Reset elapsed time
        }
        
        BeginDrawing();
        
        ClearBackground(BLACK);
        snake.draw();
        food.draw();
    
        EndDrawing();
    }
    
    UnloadSound(yellowSound);
    UnloadSound(redSound);
    UnloadSound(blueSound);
    UnloadSound(failSound);
    UnloadMusicStream(backgroundMusic);
    
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
