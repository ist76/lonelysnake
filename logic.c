// Almost all the logic of the game is here

#include <stdio.h>
#include <stdlib.h>
#include "snakestruct.h"

// Warning! The game logic does not check the maxscore value during the first initialization.
// The '0' value must be assigned in main
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

int IfCannibal(cpoint const *dot, int const *len, cpoint const *body)
{
     for (int i = *len-1; i >= 0; --i)
     {
          if ((dot->x == body[i].x) && (dot->y == body[i].y)) return 1;
     }
     return 0;
}

// Must be run for the first time before the game loop
cpoint GetApple(cpoint const *gamemap, int const *len, cpoint const *body)
{
    cpoint new;
    do
    {
          new.x = rand() % gamemap->x;
          new.y = rand() % gamemap->y;
    }
    while (IfCannibal(&new, len, body) == 1);
    return new;
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
     
     if ((vyper->vectr.x == 0) && (vyper->vectr.y == 0)) return 0; // --> the snake stands still, skip
     
     cpoint head = {vyper->body[0].x + vyper->vectr.x, vyper->body[0].y + vyper->vectr.y};
     
     if ((head.x < 0) || (head.y < 0 ) || (head.x >= gamemap->x) || (head.y >= gamemap->y ) ||
        ((vyper->len != 1) && IfCannibal(&head, &vyper->len, vyper->body)) ||
        (vyper->coins < 0))  // Don't crash into walls, don't bite yourself, and don't waste all coins else:
        return 1;            // --> restart round
     
     if ((IfCannibal(apple, &vyper->len, vyper->body)) || ((head.x == apple->x) && (head.y == apple->y))) // not good (((
     {
          vyper->coins = (vyper->len%10 == 0)?                 // Score growth depending on tail length
                         (vyper->coins + 50*vyper->len):       // If the length is a multiple of 10
                         (vyper->coins + 95 + 5*vyper->len);   // Else

          ++vyper->len; // The snake has become longer, and the coordinate of the tail has already moved to where we need it
          *ticks = *ticks >=128 ? *ticks -2 : *ticks -1;  // Speed ​​up the game with every apple you eat
          *apple = GetApple(gamemap, &vyper->len, vyper->body);
     }
     
     for (int i = vyper->len; i > 0; --i) vyper->body[i] = vyper->body[i-1];
     vyper->body[0] = head; // insert head to body
     vyper->coins -= (vyper->len <= 64) ?  1 : 2;
     return 0; // --> normal exit
}