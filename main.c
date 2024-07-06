#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "snakestruct.h"

// Will be needed if you build without -mwindows. I'll delete it soon
LRESULT WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
     switch (message)
     {
     case WM_DESTROY: PostQuitMessage(0);
	}
     return DefWindowProc(hwnd, message, wparam, lparam);
}

// Draw the game level
void MapShow(HDC dc, cpoint const *gamemap, cpoint const *body, cpoint const * apple, int scale, int len)
{
     HDC memDC = CreateCompatibleDC(dc);
     HBITMAP memBM = CreateCompatibleBitmap(dc, gamemap->x*scale, gamemap->y*scale);
     SelectObject(memDC, memBM);
     // FIXME! Не нужно рисовать сетку каждый раз, нужно сохранить фон
     SelectObject(memDC, GetStockObject(DC_BRUSH));
     SetDCBrushColor(memDC, RGB(255, 255, 255));
         Rectangle(memDC, 0, 0, gamemap->x*scale, gamemap->y*scale);
          
     // Draw cells
     SelectObject(memDC, GetStockObject(DC_PEN));
     SetDCPenColor(memDC, RGB(128, 128, 160));
     for (int i = 0; i < gamemap->x*scale; i += scale)
     {
          SetDCBrushColor(memDC, RGB(248, 248, 240));
          for (int j = 0; j < gamemap->y*scale; j += scale)
              Rectangle(memDC, i, j, i + scale, j + scale);
     }

     // Draw Snake
     SelectObject(memDC, GetStockObject(DC_BRUSH));
     SetDCPenColor(memDC, RGB(8, 16, 8));
     SetDCBrushColor(memDC, RGB(0, 16, 255));
     for (int i = 0; i < len; i++)
               {
                    SetDCBrushColor(memDC, RGB(i*4, 249, 255-i*4));
                    Rectangle(memDC, body[i].x*scale, body[i].y*scale,
                    (body[i].x+1)*scale, (body[i].y+1)*scale);
               }

     // Draw apple
     SelectObject(memDC, GetStockObject(DC_BRUSH));
     SetDCBrushColor(memDC, RGB(249, 16, 16));
               Ellipse(memDC, apple->x*scale, apple->y*scale, (apple->x+1)*scale, (apple->y+1)*scale);

     BitBlt(dc, 0, 0, gamemap->x*scale, gamemap->y*scale, memDC, 0, 0, SRCCOPY);     
     DeleteDC(memDC);
     DeleteObject(memBM);
}

// Converts keystrokes into movement direction (in game logic format)
cpoint DispatchVector(WPARAM key)
{
     cpoint out;
     switch (key)
     {
     case 0x25:
          out.x = -1;
          out.y = 0;
          break;
     
     case 0x27:
          out.x = 1;
          out.y = 0;
          break;
     
     case 0x26:
          out.x = 0;
          out.y = -1;
          break;
     
     case 0x28:
          out.x = 0;
          out.y = 1;
          break;
     
     default:
          break;
     }
     return(out);
}


int main()
{
     cpoint map = {.x = 28, .y = 21};   // Level size in cells
     int winScale = 40;                 // Scale
     int GameTicks;                     // Latency (in ms) between game loops
     cpoint apple;                      // Apple coordinates
     snake anaconda = {.maxscore = 0};  // Our snake

     // Font for scoreboard
     HFONT hFont = CreateFont(winScale, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 
     OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, FALSE);
     
     WNDCLASS wcl;
         memset(&wcl, 0, sizeof(WNDCLASS));
         wcl.lpszClassName = "mainwin";
         wcl.lpfnWndProc = WndProc;
     RegisterClass(&wcl);

     HWND hwnd = CreateWindow("mainwin", "Lonely Snake", WS_OVERLAPPEDWINDOW&(~WS_MAXIMIZEBOX)&(~WS_THICKFRAME) | WS_VISIBLE,
                         10, 10, ((map.x+11)*winScale+winScale/2), (map.y+4)*winScale, NULL, NULL, NULL, NULL);
     
     // Separate window with game level
     HWND game_map = CreateWindow("STATIC", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, winScale+winScale/2,
                                   winScale, map.x*winScale, map.y*winScale, hwnd, NULL, NULL, NULL);    
          HDC dc = GetDC(game_map); // --> Here we will draw it!
     
     // Make Scoreboard
     HWND scores = CreateWindow("STATIC", NULL, SS_CENTER | WS_VISIBLE | WS_CHILD | WS_BORDER, (map.x+3)*winScale, winScale,
                                  7*winScale, 16*winScale, hwnd, (HMENU)1010, NULL, NULL);
                    SendMessage(scores, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
     
     MSG msg;
   
     SnakeRestart(&map, &GameTicks, &anaconda);            // Game initialization
     apple = GetApple(&map, anaconda.len, anaconda.body);  // And the creation of an apple
     DWORD next_game_tick = GetTickCount();                // Timer for game loop
     TCHAR score[127];                                     // Text for score table

     for(;;) // Main Game loop
     {
          if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
          {
              if (msg.message == WM_QUIT) break;
              if (msg.message == WM_KEYDOWN) anaconda.newvectr = DispatchVector(msg.wParam);
              DispatchMessage(&msg);
          }
          
          Sleep(1); // Sleep, save the Battery!
          
          while((GetTickCount() > next_game_tick))
          {
              if (SnakeLogic(&map, &apple, &GameTicks, &anaconda)) SnakeRestart(&map, &GameTicks, &anaconda);
              next_game_tick += GameTicks;
              MapShow(dc, &map, anaconda.body, &apple, winScale, anaconda.len);
              sprintf(score,"\nScore\n\n%07i\n\nMax Score\n\n%07i", anaconda.coins, anaconda.maxscore);
              SetDlgItemText(hwnd, 1010, score);
          }
     }
     return(0);
}