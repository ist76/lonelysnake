// Launching the application, drawing windows and the main loop - here
// For project build  in MinGW64 with UTF-8 support, need use wchar_t types, w-functions, _swprintf
// instead of sprintf, 'L'-prefix for all strings and $gcc ... '-municode'
// I'm sure there is another, simpler and correct way, but I don't know it yet

#include <stdio.h>
#include <windows.h>
#include "snakestruct.h"
#include "winproc.h"
#include "main.h"
#include "winio.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
     if (message == WM_DESTROY) PostQuitMessage(0);
     return DefWindowProc(hwnd, message, wparam, lparam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
     savedata gsets = ReadSavegame();                                        // Get previously saved data
     int GameTicks;                                                          // Latency (ms) between game loops
     snake anaconda   = {.win = 0, .maxscore = gsets.maxs};                  // First snake
     snake bushmaster = {.win = 0};                                          // Second snake
     actors AllActors;                                                       // For render level, look at winproc.h
            AllActors.LewelWin.x = gsets.gamemap.x * gsets.gamescale;        // Set game level size in px
            AllActors.LewelWin.y = gsets.gamemap.y * gsets.gamescale;
     RECT ScoreTable;                                                        // Size of score table
     SetRect(&ScoreTable, 0, 0, 7 * gsets.gamescale,
             (gsets.gamemap.x/3 - 1) * gsets.gamescale + gsets.gamescale/2);
     GetGrid(&AllActors, gsets.gamemap, gsets.gamescale);                    // Array of points to draw the frid
     GetSnakeColors(&AllActors, gsets.gamemode);
     gamelang translate = ReadGamelang(gsets.lang);
     //WriteGameLang();

     // Create all GUI
     WNDCLASSW wcl;
         memset(&wcl, 0, sizeof(WNDCLASSW));
         wcl.lpszClassName = L"mainwin";
         wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
         wcl.style = CS_GLOBALCLASS;
         wcl.lpfnWndProc = WndProc;

     RegisterClassW(&wcl);

     HWND hwnd = CreateWindowW(L"mainwin", L"Snake Party", WS_OVERLAPPEDWINDOW&(~WS_MAXIMIZEBOX)&(~WS_THICKFRAME) | WS_VISIBLE,
                               10, 10, ((gsets.gamemap.x+9)*gsets.gamescale - gsets.gamescale/2),
                               (gsets.gamemap.y+3)*gsets.gamescale - gsets.gamescale/2, NULL, NULL, NULL, NULL);

     // Separate window with game level
     HWND game_map = CreateWindowW(L"static", NULL, WS_VISIBLE | WS_CHILD , gsets.gamescale/2,
                                   gsets.gamescale/2, AllActors.LewelWin.x, AllActors.LewelWin.y, hwnd, NULL, NULL, NULL);

     // Make Scoreboard 1
     HWND scores1 = CreateWindowW(L"static", NULL, SS_CENTER | WS_VISIBLE | WS_CHILD , (gsets.gamemap.x+1)*gsets.gamescale, gsets.gamescale/2,
                                  ScoreTable.right, ScoreTable.bottom, hwnd, NULL, NULL, NULL);

     // Make Scoreboard 2
     HWND scores2 = CreateWindowW(L"static", NULL, SS_CENTER | WS_VISIBLE | WS_CHILD , (gsets.gamemap.x+1)*gsets.gamescale,
                                  ScoreTable.bottom + 1 * gsets.gamescale + gsets.gamescale/2,
                                  ScoreTable.right, ScoreTable.bottom, hwnd, NULL, NULL, NULL);

     // Font for scoreboards
     HFONT hFont = CreateFontW(AllActors.LewelWin.y / 20, 0, 0, 0, 500, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                               OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, FALSE);

     HFONT hFontS = CreateFontW(AllActors.LewelWin.y / 34, 0, 0, 0, 500, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                               OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, FALSE);


     // Make main menu
     HMENU MenuBar = CreateMenu();
          HMENU hPopMenu1 = CreatePopupMenu();
          HMENU hPopMenu2 = CreatePopupMenu();
          HMENU hPopMenu3 = CreatePopupMenu();

          AppendMenuW(MenuBar,   MF_STRING | MF_POPUP, (UINT_PTR)hPopMenu1, translate.str1008);
          AppendMenuW(MenuBar,   MF_STRING | MF_POPUP, (UINT_PTR)hPopMenu2, translate.str1009);
          AppendMenuW(MenuBar,   MF_STRING | MF_POPUP, (UINT_PTR)hPopMenu3, translate.str1110);
          AppendMenuW(MenuBar,   MF_STRING,                           1100, translate.str1100);

          AppendMenuW(hPopMenu1, MF_STRING | (gsets.gamemap.x    == SMALLMAPX  ? MF_CHECKED : MF_UNCHECKED), 1001, translate.str1001);
          AppendMenuW(hPopMenu1, MF_STRING | (gsets.gamemap.x    == MEDIUMMAPX ? MF_CHECKED : MF_UNCHECKED), 1002, translate.str1002);
          AppendMenuW(hPopMenu1, MF_STRING | (gsets.gamemap.x    == LARGEMAPX  ? MF_CHECKED : MF_UNCHECKED), 1003, translate.str1003);
          AppendMenuW(hPopMenu2, MF_STRING | (gsets.gamescale    == BIGSCALE   ? MF_CHECKED : MF_UNCHECKED), 1011, translate.str1011);
          AppendMenuW(hPopMenu2, MF_STRING | (gsets.gamescale    == HUGESCALE  ? MF_CHECKED : MF_UNCHECKED), 1012, translate.str1012);
          AppendMenuW(hPopMenu3, MF_STRING | (!gsets.gamemode                  ? MF_CHECKED : MF_UNCHECKED), 1013, translate.str1013);
          AppendMenuW(hPopMenu3, MF_STRING | (gsets.gamemode                   ? MF_CHECKED : MF_UNCHECKED), 1014, translate.str1014);

          SetMenu(hwnd, MenuBar);
          SetMenu(hwnd, hPopMenu1);
          SetMenu(hwnd, hPopMenu2);
     // End of creating GUI

     srand(GetTickCount());                                                                      // For generate Apple
     fruit apple;                                                                                // Make empty Apple struct
     SnakeRestart(&gsets.gamemap, &anaconda, &bushmaster, &GameTicks, &apple, gsets.gamemode);   // Game first initialization
     DWORD next_game_tick = GetTickCount();                                                      // Timer for game loop
     DWORD next_render_tick = GetTickCount();                                                    // Timer for render loop
     MSG msg;                                                                                    // Messages from app
     HDC dc;                                                                                     // Temporary context

     for(;(anaconda.len < 253) || (bushmaster.len < 253);) // Main Game loop. Remember the snake.body[254]? We don't need to go beyond the array
     {
          if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
          {
              // Processing keyboard commands
              if (msg.message == WM_KEYDOWN) DispatchVector(msg.wParam, &anaconda.newvectr, &bushmaster.newvectr, &next_game_tick, gsets.gamemode);

              // When you select a menu item, the settings are overwritten and the application is restarted
              if (msg.message == WM_COMMAND)
              {
                   DispatchMenu(msg.wParam, &gsets);
                   WriteSavegame(&gsets, anaconda.maxscore);
                   RunAppCopy();  // Try restart the application. If unsuccessful, no problem, the settings are already saved
                   break;
              }

              // Autosave on exit
              if (msg.message == WM_QUIT)
              {
                   WriteSavegame(&gsets, anaconda.maxscore);
                   break;
              }
              DispatchMessageW(&msg);
          }

          Sleep(1); // Sleep, save the Battery!

          while(GetTickCount() > next_game_tick)
          {
              if (!SnakeLogic(&gsets.gamemap, &apple, &GameTicks, &anaconda, &bushmaster) ||
                  !SnakeLogic(&gsets.gamemap, &apple, &GameTicks, &bushmaster, &anaconda))
                  {
                        SnakeRestart(&gsets.gamemap, &anaconda, &bushmaster, &GameTicks, &apple, gsets.gamemode);
                  }

              // We calculate the coordinates of all actors not every 16ms, but only ever GameTick
              SetApple(&AllActors, &apple, gsets.gamescale);
              GetSnakesCells(&AllActors, &anaconda, &bushmaster, gsets.gamescale, gsets.gamemode);

              dc = GetDC(scores1);  // Draw scores 1
              ScoresShow(dc, &anaconda, hFont, &ScoreTable, gsets.gamemode ? translate.str1501 : translate.str1502, gsets.gamemode);
              ReleaseDC(scores1, dc);

              if (gsets.gamemode)
              {
                   dc = GetDC(scores2);  // Draw scores 2
                   ScoresShow(dc, &bushmaster, hFont, &ScoreTable, translate.str1501, gsets.gamemode);
                   ReleaseDC(scores2, dc);
              }
              else
              {
                   dc = GetDC(scores2);
                   SolutionShow(dc, hFontS, &ScoreTable, translate.str1503);
                   ReleaseDC(scores2, dc);
              }

              next_game_tick += GameTicks;
          }

          while(GetTickCount() > next_render_tick)
          {
              dc = GetDC(game_map);  // Draw level and actors
              ActorsShow(dc, &AllActors, &apple, gsets.gamemode);
              ReleaseDC(game_map, dc);

              next_render_tick += RENDERLAG;
          }
     }
     return 0;
}