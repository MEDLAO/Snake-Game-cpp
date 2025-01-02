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


class Snake{
private:
    std::deque<std::pair<int, int>> snake;
public:
    
};


class Food{
private:
public:
    
};

int main(int argc, const char * argv[]) {
    const int screen_width = 1280;
    const int screen_height = 800;
    
    InitWindow(screen_width, screen_height, "Snake Game C++");
    SetTargetFPS(60);
    
    while (WindowShouldClose() == false) {
        BeginDrawing();
        
        ClearBackground(BLUE);
        EndDrawing();
    }
    
    return 0;
}
