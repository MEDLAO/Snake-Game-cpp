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
public:
    Snake();
    void draw(){
        for (const auto& segment : snake) {
            int pixelX = segment.first * grid_size;
            int pixelY = segment.second * grid_size;
            DrawRectangle(pixelX, pixelY, grid_size, grid_size, GREEN);
        }
    }
};


class Food{
private:
public:
    
};

Snake::Snake() {
    int startX = columns / 2;
    int startY = rows / 2;
    
    // Add three initial segments
    for (int i = 0; i < 3; i++) {
        snake.push_back({startX - i, startY});
    }
}


Snake snake;


int main(int argc, const char * argv[]) {
    
    InitWindow(screen_width, screen_height, "Snake Game C++");
    SetTargetFPS(60);
    
    while (WindowShouldClose() == false) {
        BeginDrawing();
        ClearBackground(BLUE);
        
        snake.draw();
        
        EndDrawing();
    }
    
    return 0;
}
