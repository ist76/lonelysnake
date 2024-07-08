#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "snakestruct.h"

// For project build  in MinGW64 with UTF-8 support, need use wchar_t types, w-functions, _swprintf
// instead of sprintf, 'L'-prefix for all strings and $gcc ... '-municode'
// I'm sure there is another, simpler and correct way, but I don't know it yet

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
     switch (message)
     {
     case WM_DESTROY: PostQuitMessage(0);
	}
     return DefWindowProc(hwnd, message, wparam, lparam);
}

// Draw the game level
void MapShow(HDC dc, cpoint const *gamemap, cpoint const *body, cpoint const *apple, int scale, int len)
{
     HDC memDC = CreateCompatibleDC(dc);
     HBITMAP memBM = CreateCompatibleBitmap(dc, gamemap->x * scale, gamemap->y * scale);
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
void DispatchVector(WPARAM key, cpoint * newvect)
{
     switch (key)
     {
     case 0x25:
          newvect->x = -1;
          newvect->y = 0;
          break;
     
     case 0x27:
          newvect->x = 1;
          newvect->y = 0;
          break;
     
     case 0x26:
          newvect->x = 0;
          newvect->y = -1;
          break;
     
     case 0x28:
          newvect->x = 0;
          newvect->y = 1;
          break;
     
     default:
          break;
     }
}


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
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
         wcl.lpszClassName = L"mainwin";
         wcl.lpfnWndProc = WndProc;
     RegisterClass(&wcl);

     HWND hwnd = CreateWindow(L"mainwin", L"Lonely Snake", WS_OVERLAPPEDWINDOW&(~WS_MAXIMIZEBOX)&(~WS_THICKFRAME) | WS_VISIBLE,
                         10, 10, ((map.x+11)*winScale+winScale/2), (map.y+4)*winScale, NULL, NULL, NULL, NULL);
     
     // Separate window with game level
     HWND game_map = CreateWindow(L"STATIC", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, winScale+winScale/2,
                                   winScale, map.x*winScale, map.y*winScale, hwnd, NULL, NULL, NULL);    
          
     
     // Make Scoreboard
     HWND scores = CreateWindow(L"STATIC", NULL, SS_CENTER | WS_VISIBLE | WS_CHILD | WS_BORDER, (map.x+3)*winScale, winScale,
                                  7*winScale, 16*winScale, hwnd, (HMENU)1010, NULL, NULL);
                    SendMessage(scores, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
     
     MSG msg;
   
     SnakeRestart(&map, &GameTicks, &anaconda);            // Game initialization
     apple = GetApple(&map, anaconda.len, anaconda.body);  // And the creation of an apple
     DWORD next_game_tick = GetTickCount();                // Timer for game loop
     DWORD next_render_tick = GetTickCount();              // Timer for render loop
     wchar_t score[127];                                   // Text for score table

     srand(GetTickCount());                                // For generate Apple

     for(;;) // Main Game loop
     {
          if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
          {
              if (msg.message == WM_QUIT) break;
              if (msg.message == WM_KEYDOWN) DispatchVector(msg.wParam, &anaconda.newvectr);
              DispatchMessage(&msg);
          }
          
          Sleep(1); // Sleep, save the Battery!
          
          while((GetTickCount() > next_game_tick))
          {
              if (SnakeLogic(&map, &apple, &GameTicks, &anaconda)) SnakeRestart(&map, &GameTicks, &anaconda);
              _swprintf(score,L"\nScore\n\n%07i\n\nMax Score\n\n%07i", anaconda.coins, anaconda.maxscore);
              SetDlgItemTextW(hwnd, 1010, score);
              next_game_tick += GameTicks;
          }

          while((GetTickCount() > next_render_tick))
          {
              HDC dc = GetDC(game_map); // --> Here we will draw it!
              MapShow(dc, &map, anaconda.body, &apple, winScale, anaconda.len);
              ReleaseDC(game_map, dc);
              next_render_tick += 15;    // ~60fps
          }
     }
     return(0);
}