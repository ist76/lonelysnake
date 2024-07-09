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

// Draw the game actors
void ActorsShow(HDC dc, cpoint const *gamemap, cpoint const *body, cpoint const *apple, int scale, int len)
{
     HDC memDC = CreateCompatibleDC(dc);
     HBITMAP memBM = CreateCompatibleBitmap(dc, gamemap->x * scale, gamemap->y * scale);
     SelectObject(memDC, memBM);
     // FIXME! Не нужно рисовать сетку каждый раз, нужно сохранить фон
     SelectObject(memDC, GetStockObject(DC_BRUSH));
     SetDCBrushColor(memDC, RGB(248, 248, 224));
         Rectangle(memDC, 0, 0, gamemap->x*scale, gamemap->y*scale);
          
     // Draw cells
     SelectObject(memDC, GetStockObject(DC_PEN));
     SetDCPenColor(memDC, RGB(212, 224, 212));
     for (int i = scale; i < gamemap->x * scale; i +=scale)
     {
          MoveToEx(memDC, i, 1, NULL);
          LineTo(memDC, i, gamemap->y*scale - 1);
     }
     for (int i = scale; i < gamemap->y * scale; i +=scale)
     {
          MoveToEx(memDC, 1, i, NULL);
          LineTo(memDC, gamemap->x*scale - 1, i);
     }

     // Draw apple
     SelectObject(memDC, GetStockObject(DC_BRUSH));
     SetDCPenColor(memDC, RGB(8, 16, 8));
     SetDCBrushColor(memDC, RGB(249, 16, 16));
               Ellipse(memDC, apple->x*scale +2, apple->y*scale +2, (apple->x+1)*scale -2, (apple->y+1)*scale -2);

     // Draw Snake
     SetDCBrushColor(memDC, RGB(0, 16, 255));
     for (int i = 0; i < len; i++)
               {                   
                    int bChan = (i <= 63) ? 255 - i*4 : 3 + (i-64)*4;  // Snake's color gradient
                    int rChan = (i <= 63) ? i*4 : 255 - (i-64)*4;
                    SetDCBrushColor(memDC, RGB(rChan, 249, bChan));
                    RoundRect(memDC, body[i].x*scale, body[i].y*scale,
                    (body[i].x+1)*scale, (body[i].y+1)*scale, scale / 4, scale / 4);
               }

     BitBlt(dc, 0, 0, gamemap->x*scale, gamemap->y*scale, memDC, 0, 0, SRCCOPY);     
     DeleteDC(memDC);
     DeleteObject(memBM);
}

// Drawing the score table
void ScoresShow(HDC dc, int scale, int coins, int maxscore, HFONT font, RECT * const rt)
{
     
     wchar_t score[127];
     _swprintf(score,L"\nScore\n\n%07i\n\nMax Score\n\n%07i", coins, maxscore, *rt);
     
     
     HDC memDC = CreateCompatibleDC(dc);
     HBITMAP memBM = CreateCompatibleBitmap(dc, rt->right, rt->bottom);
     SelectObject(memDC, memBM);
     SelectObject(memDC, GetStockObject(DC_BRUSH));
     SetDCBrushColor(memDC, RGB(248, 248, 248));
         Rectangle(memDC, rt->left, rt->top, rt->right, rt->bottom);
    
     
     SelectObject(memDC, font);
     SetBkColor(memDC, RGB(248, 248, 248));
     DrawTextW(memDC, score, -1, rt, DT_CENTER);

     BitBlt(dc, rt->left, rt->top, rt->right, rt->bottom, memDC, 0, 0, SRCCOPY);     
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
     cpoint map = {.x = 30, .y = 22};   // Level size in cells
     int winScale = 40;                 // Scale
     int GameTicks;                     // Latency (in ms) between game loops
     cpoint apple;                      // Apple coordinates
     snake anaconda = {.maxscore = 0};  // Our snake

     RECT ScoreTable;                   // Size of score table
     SetRect(&ScoreTable, 0, 0, 7 * winScale, 16 * winScale);
     
     WNDCLASSW wcl;
         memset(&wcl, 0, sizeof(WNDCLASSW));
         wcl.lpszClassName = L"mainwin";
         wcl.lpfnWndProc = WndProc;
     RegisterClass(&wcl);

     HWND hwnd = CreateWindowW(L"mainwin", L"Lonely Snake", WS_OVERLAPPEDWINDOW&(~WS_MAXIMIZEBOX)&(~WS_THICKFRAME) | WS_VISIBLE,
                         10, 10, ((map.x+11)*winScale+winScale/2), (map.y+3)*winScale, NULL, NULL, NULL, NULL);
     
     // Separate window with game level
     HWND game_map = CreateWindowW(L"STATIC", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, winScale+winScale/2,
                                   winScale, map.x*winScale, map.y*winScale, hwnd, NULL, NULL, NULL);    
          
     
     // Make Scoreboard
     HWND scores = CreateWindowW(L"STATIC", NULL, SS_CENTER | WS_VISIBLE | WS_CHILD | WS_BORDER, (map.x+3)*winScale, winScale,
                                  ScoreTable.right, ScoreTable.bottom, hwnd, NULL, NULL, NULL);
     
     HFONT hFont = CreateFont(winScale, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 
                   OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, FALSE);
     MSG msg;
   
     SnakeRestart(&map, &GameTicks, &anaconda);            // Game initialization
     apple = GetApple(&map, &anaconda.len, anaconda.body);  // And the creation of an apple
     DWORD next_game_tick = GetTickCount();                // Timer for game loop
     DWORD next_render_tick = GetTickCount();              // Timer for render loop
     srand(GetTickCount());                                // For generate Apple

     for(;;) // Main Game loop
     {
          if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
          {
              if (msg.message == WM_QUIT) break;
              if (msg.message == WM_KEYDOWN) DispatchVector(msg.wParam, &anaconda.newvectr);
              DispatchMessageW(&msg);
          }
          
          Sleep(1); // Sleep, save the Battery!
          
          while((GetTickCount() > next_game_tick))
          {
              if (SnakeLogic(&map, &apple, &GameTicks, &anaconda)) SnakeRestart(&map, &GameTicks, &anaconda);
              HDC sdc = GetDC(scores);  // Draw scores
              ScoresShow(sdc, winScale, anaconda.coins, anaconda.maxscore, hFont, &ScoreTable);
              ReleaseDC(scores, sdc);
              next_game_tick += GameTicks;
          }

          while((GetTickCount() > next_render_tick))
          {
              HDC dc = GetDC(game_map); // --> Here we will draw it!
              ActorsShow(dc, &map, anaconda.body, &apple, winScale, anaconda.len);
              ReleaseDC(game_map, dc);
              next_render_tick += 15;    // ~60fps
          }
     }
     return(0);
}