// Launching the application, drawing windows and the main loop - here
// For project build  in MinGW64 with UTF-8 support, need use wchar_t types, w-functions, _swprintf
// instead of sprintf, 'L'-prefix for all strings and $gcc ... '-municode'
// I'm sure there is another, simpler and correct way, but I don't know it yet

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "snakestruct.h"
#include "winproc.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
     if (message == WM_DESTROY) PostQuitMessage(0);
     return DefWindowProc(hwnd, message, wparam, lparam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
     savedata gamesettings = ReadSavegame();                              // Get previously saved data
     int GameTicks;                                                       // Latency (in ms) between game loops
     snake anaconda;                                                      // Our snake
     cpoint map = gamesettings.gamemap;                                   // Level size in cells
     int winScale = gamesettings.gamescale;                               // Scale
     anaconda.maxscore = gamesettings.gamemaxscore;                       // User's game record
     RECT ScoreTable;                                                     // Size of score table
     SetRect(&ScoreTable, 0, 0, 7 * winScale, (map.x/3 - 1) * winScale);  // Size of scoretable's window
     
     WNDCLASSW wcl;
         memset(&wcl, 0, sizeof(WNDCLASSW));
         wcl.lpszClassName = L"mainwin";
         wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
         wcl.style = CS_GLOBALCLASS;
         wcl.lpfnWndProc = WndProc;
     
     RegisterClassW(&wcl);

     HWND hwnd = CreateWindowW(L"mainwin", L"Lonely Snake", WS_OVERLAPPEDWINDOW&(~WS_MAXIMIZEBOX)&(~WS_THICKFRAME) | WS_VISIBLE,
                               10, 10, ((map.x+11)*winScale+winScale/2), (map.y+4)*winScale, NULL, NULL, NULL, NULL);
     
     // Separate window with game level
     HWND game_map = CreateWindowW(L"static", NULL, WS_VISIBLE | WS_CHILD , winScale+winScale/2,
                                   winScale, map.x*winScale, map.y*winScale, hwnd, NULL, NULL, NULL);    
     
     // Make Scoreboard
     HWND scores = CreateWindowW(L"static", NULL, SS_CENTER | WS_VISIBLE | WS_CHILD , (map.x+3)*winScale, winScale,
                                 ScoreTable.right, ScoreTable.bottom, hwnd, NULL, NULL, NULL);
     
     // Font for scoreboard
     HFONT hFont = CreateFontW((map.y * winScale) / 22, 0, 0, 0, 400, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 
                               OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, FALSE);

     // Make Solution board
     HWND solutions = CreateWindowW(L"static", NULL, SS_CENTER | WS_VISIBLE | WS_CHILD , (map.x+3)*winScale, ScoreTable.bottom + 3 * winScale,
                                    ScoreTable.right, ScoreTable.bottom, hwnd, NULL, NULL, NULL);
     
     HFONT hFont2 = CreateFontW((map.y * winScale) / 35, 0, 0, 0, 400, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 
                                OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, FALSE);
     

     // Make main menu
     HMENU MenuBar = CreateMenu();
          HMENU hPopMenu1 = CreatePopupMenu();
          HMENU hPopMenu2 = CreatePopupMenu();

          AppendMenuW(MenuBar,   MF_STRING | MF_POPUP, (UINT_PTR)hPopMenu1, L"Map size");
          AppendMenuW(MenuBar,   MF_STRING | MF_POPUP, (UINT_PTR)hPopMenu2, L"Scale");
            
          AppendMenuW(hPopMenu1, MF_STRING | (map.x    == SMALLMAPX  ? MF_CHECKED : MF_UNCHECKED), 1001, L"Small");
          AppendMenuW(hPopMenu1, MF_STRING | (map.x    == MEDIUMMAPX ? MF_CHECKED : MF_UNCHECKED), 1002, L"Medium");
          AppendMenuW(hPopMenu1, MF_STRING | (map.x    == LARGEMAPX  ? MF_CHECKED : MF_UNCHECKED), 1003, L"Large");
          AppendMenuW(hPopMenu2, MF_STRING | (winScale == BIGSCALE   ? MF_CHECKED : MF_UNCHECKED), 1011, L"Big");
          AppendMenuW(hPopMenu2, MF_STRING | (winScale == HUGESCALE  ? MF_CHECKED : MF_UNCHECKED), 1012, L"Huge");

          SetMenu(hwnd, MenuBar);
          SetMenu(hwnd, hPopMenu1);
          SetMenu(hwnd, hPopMenu2);
     
     srand(GetTickCount());                                         // For generate Apple
     SnakeRestart(&map, &GameTicks, &anaconda);                     // Game initialization
     cpoint apple = GetApple(&map, &anaconda.len, anaconda.body);   // And the creation of an apple
     DWORD next_game_tick = GetTickCount();                         // Timer for game loop
     DWORD next_render_tick = GetTickCount();                       // Timer for render loop
     MSG msg;                                                       // Messages from app
     
     for(;anaconda.len < 253;) // Main Game loop. Remember the snake.body[254]? We don't need to go beyond the array
     {
          if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
          {
              if (msg.message == WM_QUIT)
              {
                   WriteSavegame(map, winScale, anaconda.maxscore);
                   break;  // Autosave on exit
              }
              if (msg.message == WM_KEYDOWN) DispatchVector(msg.wParam, &anaconda.newvectr, &next_game_tick);
              if (msg.message == WM_COMMAND)
              {
                   DispatchMenu(msg.wParam, &map, &winScale);
                   SnakeRestart(&map, &GameTicks, &anaconda);
                   WriteSavegame(map, winScale, anaconda.maxscore);
                   RunAppCopy();  // Try restart the application. If unsuccessful, no problem, the settings are already saved
                   break;
              }
              DispatchMessageW(&msg);
          }
          
          Sleep(1); // Sleep, save the Battery!
          
          while(GetTickCount() > next_game_tick)
          {
              if (SnakeLogic(&map, &apple, &GameTicks, &anaconda)) SnakeRestart(&map, &GameTicks, &anaconda);
              
              HDC sdc = GetDC(scores);  // Draw scores
              ScoresShow(sdc, winScale, anaconda.coins, anaconda.maxscore, hFont, &ScoreTable);
              ReleaseDC(scores, sdc);
              
              HDC tdc = GetDC(solutions);  // Draw solution
              SolutionShow(tdc, winScale, hFont2, &ScoreTable);
              ReleaseDC(solutions, tdc);
              
              next_game_tick += GameTicks;
          }

          while(GetTickCount() > next_render_tick)
          {
              HDC dc = GetDC(game_map);  // Draw level and actors
              ActorsShow(dc, &map, anaconda.body, &apple, winScale, anaconda.len);
              ReleaseDC(game_map, dc);
              
              next_render_tick += 16;    // ~60fps
          }
     }
     return 0;
}