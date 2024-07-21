// Functions for a windowed application
// Responsible for drawing the level, leaderboards, actors

#include <stdio.h>
#include <windows.h>
#include "snakestruct.h"
#include "winproc.h"

// Calculate Snake's coords before rendering
void GetSnakesCells(actors *allobj, snake const *vyper, snake const *wutu, int scale, int mode)
{
     allobj->ALen = vyper->len;
     for (int i = 0; i < vyper->len; i++)
     {
          allobj->ASnake[i].left   =  vyper->body[i].x * scale;
          allobj->ASnake[i].top    =  vyper->body[i].y * scale;
          allobj->ASnake[i].right  = (vyper->body[i].x + 1) * scale;
          allobj->ASnake[i].bottom = (vyper->body[i].y + 1) * scale;
     }
     if (!mode) return;  // Single Player

     allobj->BLen = wutu->len;
     for (int i = 0; i < wutu->len; i++)
     {
          allobj->BSnake[i].left   =  wutu->body[i].x * scale;
          allobj->BSnake[i].top    =  wutu->body[i].y * scale;
          allobj->BSnake[i].right  = (wutu->body[i].x + 1) * scale;
          allobj->BSnake[i].bottom = (wutu->body[i].y + 1) * scale;
     }
}

// Calculate Snake's cells color gradient (one time)
void GetSnakeColors(actors *allobj, int mode)
{
     for (int i = 0; i <= 63; i++)
     {
          allobj->AColor[i] = allobj->AColor[126 - i] = allobj->AColor[126 + i] = allobj->AColor[253 - i]
                            = RGB(i * 4, 249, 255 - i * 4);
          if (mode)
          {
          allobj->BColor[i] = allobj->BColor[126 - i] = allobj->BColor[126 + i] = allobj->BColor[253 - i]
                            = RGB(191 + i, i * 4, 255 - i);
          }
     }
}

// Calculate grid lines only one time, use every 16ms
void GetGrid(actors *allobj, cpoint map, int scale)
{
     int counter = 0;
     for (int i = 0; i < map.x; i++)
     {
          allobj->Grid[counter].left   = (i+1) * scale;
          allobj->Grid[counter].top    = 1;
          allobj->Grid[counter].right  = (i+1) * scale;
          allobj->Grid[counter].bottom = map.y * scale - 1;
          counter++;
     }
     for (int i = 0; i < map.y; i++)
     {
          allobj->Grid[counter].left   = 1;
          allobj->Grid[counter].top    = (i+1) * scale;
          allobj->Grid[counter].right  = map.x * scale - 1;
          allobj->Grid[counter].bottom = (i+1) * scale;
          counter++;
     }
     allobj->GLen = counter;
}

// Calculate apple coordinates & color
void SetApple(actors *allobj, fruit *apple, int scale)
{
     switch (apple->price)
     {
     case ColorGold:
          allobj->AppleColor = 0x000080FF;
          break;

     case ColorBlack:
          allobj->AppleColor = 0x00080808;
          break;

     default:
          allobj->AppleColor = 0x000000FF;
          break;
     }
     allobj->RApple.left   =  apple->coord.x*scale + 2;
     allobj->RApple.top    =  apple->coord.y*scale + 2;
     allobj->RApple.right  = (apple->coord.x + 1) * scale - 2;
     allobj->RApple.bottom = (apple->coord.y + 1) * scale - 2;
}

void DrawGrid(HDC sdc, actors const *allobj)
{
     SelectObject(sdc, GetStockObject(DC_PEN));
     SetDCPenColor(sdc, RGB(212, 224, 212));
     for (int i = 0; i < allobj->GLen; i++)
     {
          MoveToEx(sdc, allobj->Grid[i].left, allobj->Grid[i].top, NULL);
          LineTo  (sdc, allobj->Grid[i].right, allobj->Grid[i].bottom);
     }
}

void DrawSnakes(HDC sdc, actors const *allobj, int mode)
{
     int round = (allobj->ASnake[0].right - allobj->ASnake[0].left) / 4; // In order not to drag the scale, we calculate again
     for (int i = 0; i < allobj->ALen; i++)
     {
          SetDCPenColor(sdc, 0x00008000);
          SetDCBrushColor(sdc, allobj->AColor[i]);
          RoundRect(sdc, allobj->ASnake[i].left, allobj->ASnake[i].top,
                    allobj->ASnake[i].right, allobj->ASnake[i].bottom, round, round);
     }
     if (!mode) return;  // If single player

     for (int i = 0; i < allobj->BLen; i++)
     {
          SetDCPenColor(sdc, 0x00000080);
          SetDCBrushColor(sdc, allobj->BColor[i]);
          RoundRect(sdc, allobj->BSnake[i].left, allobj->BSnake[i].top,
                    allobj->BSnake[i].right, allobj->BSnake[i].bottom, round, round);
     }
}

void DrawApple(HDC sdc, actors *allobj)
{
     SelectObject(sdc, GetStockObject(DC_BRUSH));
     SetDCPenColor(sdc, RGB(8, 16, 8));
     SetDCBrushColor(sdc, allobj->AppleColor);
     Ellipse(sdc, allobj->RApple.left, allobj->RApple.top, allobj->RApple.right, allobj->RApple.bottom);
}

// Draw level and the game actors
void ActorsShow(HDC dc, actors *allobj, fruit const *apple, int mode)
{
     HDC memDC = CreateCompatibleDC(dc);
     HBITMAP memBM = CreateCompatibleBitmap(dc, allobj->LewelWin.x, allobj->LewelWin.y);
     SelectObject(memDC, memBM);

     // Draw background
     SelectObject(memDC, GetStockObject(DC_BRUSH));
     SetDCBrushColor(memDC, RGB(248, 248, 224));
          Rectangle(memDC, 0, 0, allobj->LewelWin.x, allobj->LewelWin.y);

     DrawGrid(memDC, allobj);
     DrawApple(memDC, allobj);
     DrawSnakes(memDC, allobj, mode);

     BitBlt(dc, 0, 0, allobj->LewelWin.x, allobj->LewelWin.y, memDC, 0, 0, SRCCOPY);
     DeleteDC(memDC);
     DeleteObject(memBM);
}

// Drawing the score table(s)
void ScoresShow(HDC dc, snake *vyper, HFONT font, RECT * const rt, wchar_t *message, int mode)
{
     wchar_t score[63];
     mode ?
          _swprintf(score, message, vyper->coins, vyper->win, *rt) :
          _swprintf(score, message, vyper->coins, vyper->maxscore, *rt) ;
     HDC memDC = CreateCompatibleDC(dc);
     HBITMAP memBM = CreateCompatibleBitmap(dc, rt->right, rt->bottom);
     SelectObject(memDC, memBM);
     SelectObject(memDC, GetStockObject(DC_BRUSH));
     SetDCBrushColor(memDC, RGB(248, 248, 248));
         Rectangle(memDC, rt->left, rt->top, rt->right, rt->bottom);

     SelectObject(memDC, font);
     SetBkColor(memDC, RGB(248, 248, 248));
     DrawTextW(memDC, score, -1, rt, DT_CENTER); // Write text directly to the window

     BitBlt(dc, rt->left, rt->top, rt->right, rt->bottom, memDC, 0, 0, SRCCOPY);
     DeleteDC(memDC);
     DeleteObject(memBM);
}

void SolutionShow(HDC dc, HFONT font, RECT * const rt, wchar_t *message)
{
     wchar_t score[254];
     _swprintf(score, message, *rt);
     HDC memDC = CreateCompatibleDC(dc);
     HBITMAP memBM = CreateCompatibleBitmap(dc, rt->right, rt->bottom);
     SelectObject(memDC, memBM);
     SelectObject(memDC, GetStockObject(DC_BRUSH));
     SetDCBrushColor(memDC, RGB(248, 248, 248));
         Rectangle(memDC, rt->left, rt->top, rt->right, rt->bottom);

     SelectObject(memDC, font);
     SetBkColor(memDC, RGB(248, 248, 248));
     DrawTextW(memDC, score, -1, rt, DT_CENTER); // Write text directly to the window

     BitBlt(dc, rt->left, rt->top, rt->right, rt->bottom, memDC, 0, 0, SRCCOPY);
     DeleteDC(memDC);
     DeleteObject(memBM);
}