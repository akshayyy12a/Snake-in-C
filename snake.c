#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>


#define MAX 100  // max snake length
#define W 60     // width
#define H 20     // height

// for the snake body
typedef struct {
    int x;
    int y;
} pos;

// globals
pos snake[MAX];
pos food;  
int snake_len = 1;   
int pts = 0;     // points
int dir = 3;     // 0=up, 1=down, 2=left, 3=right
bool game_over = false;

// sets up the game
void init() 
{
    // put snake in middle of screen
    snake[0].x = W/2;
    snake[0].y = H/2;
}

// shows current score
void show_score() 
{
    mvprintw(H+1, 1, "Score: %d", pts);
}

// draws the snake
void draw_snake() 
{
    for(int i = 0; i < snake_len; i++) 
    {
        mvprintw(snake[i].y, snake[i].x, "o"); 
    }
}

// checks if food spawned on snake
bool food_on_snake() 
{
    for(int i = 0; i < snake_len; i++)
        if(snake[i].x == food.x && snake[i].y == food.y)
            return true;
    return false;
}

// spawns new food
void new_food() 
{
    do 
    {
        food.x = rand() % (W-2) + 1;
        food.y = rand() % (H-2) + 1;
    } while(food_on_snake());
}

void draw_food() 
{
    mvprintw(food.y, food.x, "+"); 
}

// returns true if snake hits wall or self
bool check_crash() 
{
    // hit wall?
    if(snake[0].x <= 0 || snake[0].x >= W-1 || snake[0].y <= 0 || snake[0].y >= H-1)
        return true;
    
    // hit self
    for(int i = 1; i < snake_len; i++)
        if(snake[0].x == snake[i].x && snake[0].y == snake[i].y)
            return true;
    
    return false;
}

void move_snake() 
{
    int key = getch();
    
    // remember old positions
    pos old[MAX];
    for(int i = 0; i < snake_len; i++) 
    {
        old[i] = snake[i];
    }

    // handle keys - no 180 turns!
    if(key != ERR) 
    {
        switch(key) 
        {
            case 'z': case 'Z':  // up
                if(dir != 1) dir = 0; 
                break;
            case 's': case 'S':  // down
                if(dir != 0) dir = 1; 
                break;
            case 'q': case 'Q':  // left
                if(dir != 3) dir = 2; 
                break;
            case 'd': case 'D':  // right
                if(dir != 2) dir = 3; 
                break;
        }
    }

    // move head
    switch(dir) 
    {
        case 0: snake[0].y--; break;  // up
        case 1: snake[0].y++; break;  // down
        case 2: snake[0].x--; break;  // left
        case 3: snake[0].x++; break;  // right
    }

    // move body
    for(int i = 1; i < snake_len; i++) 
    {
        snake[i] = old[i-1];
    }

    if(snake[0].x == food.x && snake[0].y == food.y) 
    {
        pts += 10;
        snake_len++;
        new_food();
    }
}

void draw_map() 
{
    for(int i = 0; i < H; i++) 
    {
        for(int j = 0; j < W; j++) 
        {
            if(i == 0 || i == H-1 || j == 0 || j == W-1) 
            {
                mvprintw(i, j, "*");
            }
        }
    }
}

int main() 
{
    // setup
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    timeout(50);  // game speed 
    keypad(stdscr, TRUE);
    srand(time(NULL));

    init();
    new_food();

    while(!game_over) 
    {
        clear();
        draw_map();
        draw_snake();
        draw_food();
        show_score();
        
        move_snake();
        
        if(check_crash()) 
        {
            game_over = true;
            mvprintw(H+2, 1, "GAME OVER! You scored %d points!", pts);
            refresh();
            timeout(-1);
            getch();
        }
        
        refresh();
        usleep(50000);  
    }

    endwin();
    return 0;
}