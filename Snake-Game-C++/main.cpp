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
#include <raylib.h>


const int screen_width = 1280;
const int screen_height = 800;
const int grid_size = 20; // Size of each cell
const int columns = screen_width / grid_size;
const int rows = screen_height / grid_size;
std::set<std::pair<int, int>> occupiedPositions;


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
    }
    
    bool ateFood();
    
    void checkAndGrow(){
        if (ateFood()) {
            const auto& tail = snake.back();
            snake.push_back({tail.first, tail.second});
            occupiedPositions.insert(tail);
            
        }
    }
    
    void reset(){
        snake.clear();
        occupiedPositions.clear();
        init();
    }
    
    void selfCollision(){
        // Get the head of the snake
        const auto& head = snake.front();
        
        // Iterate over the body excluding the head
        for (auto it = snake.begin() + 1; it != snake.end(); ++it) {
            if (head.first == it->first && head.second == it->second) {
                reset();
            }
        }
    }
};

Snake snake;

class Food{
private:
    const int foodCount = 15;
    
public:
    std::set<std::pair<int, int>> foodPositions;
    
    void generateFood(){
        while (foodPositions.size() < foodCount) {
            int x = GetRandomValue(0, columns - 1);
            int y = GetRandomValue(0, rows - 1);
            
            // Ensure no overlap with snake or existing food
            if (occupiedPositions.find({x, y}) == occupiedPositions.end()) {
                foodPositions.insert({x, y});
                occupiedPositions.insert({x, y});
            }
        }
    }
    
    void draw(){
        for (const auto& pos : foodPositions) {
            int pixelX = pos.first * grid_size;
            int pixelY = pos.second * grid_size;
            DrawRectangle(pixelX, pixelY , grid_size, grid_size, YELLOW);
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
    for (auto it = food.foodPositions.begin(); it != food.foodPositions.end(); ++it) {
        if (*it == head) {
            food.foodPositions.erase(it);
            return true;
        }
    }
    return false;
}


int main(int argc, const char * argv[]) {
    
    InitWindow(screen_width, screen_height, "Snake Game C++");
    SetTargetFPS(60);
    
    float updateInterval = 0.1f; // Time between snake updates
    float elapsedTime = 0.0f; // Accumulator for delta time
    
    food.generateFood();
    
    while (WindowShouldClose() == false) {
        
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
    
    return 0;
}
