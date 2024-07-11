typedef struct cpoint // The logic of the game is calculated in "cells"
{
     int x, y;
} cpoint;

typedef struct snake      // All snake data
{
     int coins;           // Current score
     int maxscore;        // Maximum score achieved
     cpoint vectr;        // Current direction of the snake
     cpoint newvectr;     // Player-entered direction
     int len;             // Current snake length (in segments)
     cpoint body[254];    // An array of segment coordinates (I hope 254 is enough for everyone)
} snake;