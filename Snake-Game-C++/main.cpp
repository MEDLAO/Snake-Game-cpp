//
//  main.cpp
//  Snake-Game-C++
//
//  Created by MEDLAO on 31/12/2024.
//

#include <iostream>
#include <deque>
#include <utility>
#include <raylib.h>


const int screen_width = 1280;
const int screen_height = 800;
const int grid_size = 20; // Size of each cell
const int columns = screen_width / grid_size;
const int rows = screen_height / grid_size;


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
        
        snake.push_front({newX, newY});
        snake.pop_back();
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
    
    bool checkAndGrow(){
        if (ateFood()) {
            const auto& tail = snake.back();
            snake.push_back({tail.first, tail.second});
            return true;
        }
        return false;
    }
};

Snake snake;

class Food{
private:
    int x, y;
public:
    // Getters
    int getX() const{
        return x;
    }
    
    int getY() const{
        return y;
    }
    
    
    void generateFood(){
        bool overlaps;
        do {
            // Generate random coordinates within the grid boundaries
            x = GetRandomValue(0, columns - 1);
            y = GetRandomValue(0, rows - 1);
            
            overlaps = false;
            for (const auto& segment : snake.getSnake()) {
                if (segment.first == x && segment.second == y) {
                    overlaps = true;
                    break;
                }
            }
        } while (overlaps);
    }
    
    void draw(){
        int pixelX = x * grid_size;
        int pixelY = y * grid_size;
        DrawRectangle(pixelX, pixelY , grid_size, grid_size, YELLOW);
    }
};

Food food;

// Initialize Snake with default speed and starting segments
Snake::Snake() : speedX(1), speedY(0) {
    int startX = columns / 2;
    int startY = rows / 2;
    
    // Add three initial segments
    for (int i = 0; i < 3; i++) {
        snake.push_back({startX - i, startY});
    }
}

bool Snake::ateFood(){
    return snake.front().first == food.getX() && snake.front().second == food.getY();
}


int main(int argc, const char * argv[]) {
    
    InitWindow(screen_width, screen_height, "Snake Game C++");
    SetTargetFPS(60);
    
    float updateInterval = 0.1f; // Time between snake updates
    float elapsedTime = 0.0f; // Accumulator for delta time
    
    
    while (WindowShouldClose() == false) {
        
        float deltaTime = GetFrameTime();
        elapsedTime += deltaTime;
        
        if (elapsedTime >= updateInterval) {
            snake.update(); // Move the snake
            if (snake.checkAndGrow()) {
                food.generateFood();
            }
            
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
