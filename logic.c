#include <stdio.h>
#include <stdlib.h>
#include "snakestruct.h"

// Warning! The game logic does not check the maxscore value during the first initialization.
// The null value must be assigned in main
// Also main should run initialization before the game loop
void SnakeRestart(cpoint const *gamemap, int *ticks, snake *vyper)
{
     *ticks = 256;
     vyper->vectr.x = 0;
     vyper->vectr.y = 0;
     vyper->newvectr.x = 0;
     vyper->newvectr.y = 0;
     vyper->len = 1;
     vyper->body[0].x = (gamemap->x / 2);
     vyper->body[0].y = (gamemap->y / 2);
     if (vyper->coins > vyper->maxscore) vyper->maxscore = vyper->coins;
     vyper->coins = 50;
}

int IfCannibal(cpoint dot, int len, cpoint const *body)
{
     for (int i = len; i >= 0; --i)
     {
          if ((dot.x == body[i].x) && (dot.y == body[i].y)) return(1);
     }
     return(0);
}

// Must be run for the first time before the game loop
cpoint GetApple(cpoint const *gamemap, int len, cpoint const *body)
{
    cpoint new;
    do
    {
          new.x = rand() % gamemap->x;
          new.y = rand() % gamemap->y;
    }
    while (IfCannibal(new, len, body) == 1);
    return(new);
}

// The game loop checks for the return value. If "0" is received, SnakeRestart is called
int SnakeLogic(cpoint const *gamemap, cpoint *apple, int *ticks, snake *vyper)
{
     if ((vyper->newvectr.x != vyper->vectr.x * (-1)) || // a single length snake can change
         (vyper->newvectr.y != vyper->vectr.y * (-1)) || // direction to the opposite direction
         (vyper->len == 1))
          {
               vyper->vectr = vyper->newvectr;
          }
     
     if ((vyper->vectr.x == 0) && (vyper->vectr.y == 0)) return (0); // --> the snake stands still, skip
     
     cpoint head = {vyper->body[0].x + vyper->vectr.x, vyper->body[0].y + vyper->vectr.y};
     
     if ((head.x < 0) || (head.y < 0 ) || (head.x >= gamemap->x) || (head.y >= gamemap->y ) ||
        ((vyper->len != 1) && IfCannibal(head, vyper->len, vyper->body)) ||
        (vyper->coins < 0))
        return(1); // --> restart round
     
     if ((head.x == apple->x) && (head.y == apple->y))
     {
          vyper->coins = vyper->coins + 95 + 5*vyper->len;  // Score growth depending on tail length
          ++vyper->len; // The snake has become longer, and the coordinate of the tail has already moved to where we need it
          *ticks -= 2;  // Speed ​​up the game with every apple you eat
          *apple = GetApple(gamemap, vyper->len, vyper->body);
     }
     
     for (int i = vyper->len; i > 0; --i) vyper->body[i] = vyper->body[i-1];
     vyper->body[0] = head; // insert head to body
     --vyper->coins;
     return(0); // --> normal exit
}