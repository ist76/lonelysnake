// Helper functions for a windowed application
// Responsible for drawing the level, leaderboards, reading and writing the save file

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "snakestruct.h"
#include "winproc.h"

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
     int gChan = (len%10 == 0) ? 128 : 16;  // Reg or Gold apple
     SetDCBrushColor(memDC, RGB(249, gChan, 16));
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
     DrawTextW(memDC, score, -1, rt, DT_CENTER); // Writes text directly to the window

     BitBlt(dc, rt->left, rt->top, rt->right, rt->bottom, memDC, 0, 0, SRCCOPY);     
     DeleteDC(memDC);
     DeleteObject(memBM);
}

// Drawing the solution
void SolutionShow(HDC dc, int scale, HFONT font, RECT * const rt)
{
     wchar_t solution[256];
     _swprintf(solution, L"\nTo start the game \njust select a direction\n\nSelecting direction - \narrows or «W S A D»\non the keyboard\n\nEscape - pause\n\n\nAll changes to game \nsettings require a \nrestart\n", *rt);
     
     HDC memDC = CreateCompatibleDC(dc);
     HBITMAP memBM = CreateCompatibleBitmap(dc, rt->right, rt->bottom);
     SelectObject(memDC, memBM);
     SelectObject(memDC, GetStockObject(DC_BRUSH));
     SetDCBrushColor(memDC, RGB(248, 248, 248));
         Rectangle(memDC, rt->left, rt->top, rt->right, rt->bottom);
     
     SelectObject(memDC, font);
     SetBkColor(memDC, RGB(248, 248, 248));
     DrawTextW(memDC, solution, -1, rt, DT_CENTER); // Writes text directly to the window

     BitBlt(dc, rt->left, rt->top, rt->right, rt->bottom, memDC, 0, 0, SRCCOPY);     
     DeleteDC(memDC);
     DeleteObject(memBM);
}

// Converts keystrokes into movement direction (in game logic format)
void DispatchVector(WPARAM key, cpoint * newvect, DWORD * next_tick)
{
     switch (key)
     {
     case 0x41:               // Key LEFT
          newvect->x = -1;
          newvect->y = 0;
          break;
     
     case 0x27:               // Key RIGHT
          newvect->x = 1;
          newvect->y = 0;
          break;
     
     case 0x26:               // Key UP
          newvect->x = 0;
          newvect->y = -1;
          break;
     
     case 0x28:               // Key DOWN
          newvect->x = 0;
          newvect->y = 1;
          break;

     case 0x25:               // Key LEFT
          newvect->x = -1;
          newvect->y = 0;
          break;
     
     case 0x44:               // Key RIGHT
          newvect->x = 1;
          newvect->y = 0;
          break;
     
     case 0x57:               // Key UP
          newvect->x = 0;
          newvect->y = -1;
          break;
     
     case 0x53:               // Key DOWN
          newvect->x = 0;
          newvect->y = 1;
          break;

     case 0x1B:               // Key PAUSE
          *next_tick = *next_tick != UINT_MAX ? UINT_MAX : GetTickCount();
          break;
     
     default:
          break;
     }
}

// Handling menu item selections
void DispatchMenu(WPARAM val, cpoint * map, int * scale)
{
     switch (val)
     {
     case 1001:
          map->x = 24;
          map->y = 16;
          break;

     case 1002:
          map->x = 30;
          map->y = 20;
          break;

     case 1003:
          map->x = 36;
          map->y = 24;
          break;

     case 1011:
          *scale = 32;
          break;

     case 1012:
          *scale = 38;
          break;
     
     default:
          break;
     }
}

savedata ReadSavegame() // No comments..
{
     savedata usersave = { .gamemap = {.x = 30, .y = 20},
                           .gamescale = 38,
                           .gamemaxscore = 0};
     HANDLE hFile = CreateFile(L"snake.sav", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
     if (INVALID_HANDLE_VALUE == hFile) return usersave; // FIXME! Write check correctness later
     ReadFile(hFile, &usersave, sizeof(usersave), NULL, NULL);
     CloseHandle(hFile);
     return usersave;
}

void WriteSavegame(cpoint maps, int scale, int maxs)
{
     HANDLE hFile = CreateFile(L"snake.sav", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
     if (INVALID_HANDLE_VALUE == hFile) return;
     savedata usersave = { .gamemap = maps,
                           .gamescale = scale,
                           .gamemaxscore = maxs};
     WriteFile(hFile, &usersave, sizeof(usersave), NULL, NULL);
     CloseHandle(hFile);
}

void RunAppCopy(void)
{
     wchar_t path[256];
     GetModuleFileNameW(0, path, 256); // Get full name of snake.exe
     STARTUPINFO si;
     PROCESS_INFORMATION pi;
     ZeroMemory( &si,sizeof(si));
     si.cb = sizeof(si);
     ZeroMemory(&pi, sizeof(pi));
     CreateProcessW(path, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
     CloseHandle(pi.hProcess);
     CloseHandle(pi.hThread);
}