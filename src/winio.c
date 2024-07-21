// Keyboard input, read and write settings, process menu commands,
// and restart the application

#include <windows.h>
#include "snakestruct.h"
#include "winio.h"

// Converts keystrokes into movement direction (in game logic format)
void DispatchVector(WPARAM key, cpoint * newvect1, cpoint * newvect2, DWORD * next_tick, int mode)
{
     switch (key)
     {
     case 0x41:               // Key LEFT (A)
          newvect1->x = -1;
          newvect1->y = 0;
          break;

     case 0x44:               // Key RIGHT (D)
          newvect1->x = 1;
          newvect1->y = 0;
          break;

     case 0x57:               // Key UP (W)
          newvect1->x = 0;
          newvect1->y = -1;
          break;

     case 0x53:               // Key DOWN (S)
          newvect1->x = 0;
          newvect1->y = 1;
          break;

     case 0x25:               // Key LEFT
          if (mode)
          {
               newvect2->x = -1;
               newvect2->y = 0;
          }
          else
          {
               newvect1->x = -1;
               newvect1->y = 0;
          }

          break;

     case 0x27:               // Key RIGHT
          if (mode)
          {
               newvect2->x = 1;
               newvect2->y = 0;
          }
          else
          {
               newvect1->x = 1;
               newvect1->y = 0;
          }
          break;

     case 0x26:               // Key UP
          if (mode)
          {
               newvect2->x = 0;
               newvect2->y = -1;
          }
          else
          {
               newvect1->x = 0;
               newvect1->y = -1;
          }
          break;

     case 0x28:               // Key DOWN
          if (mode)
          {
               newvect2->x = 0;
               newvect2->y = 1;
          }
          else
          {
               newvect1->x = 0;
               newvect1->y = 1;
          }
          break;

     case 0x1B:               // Key PAUSE (ESC)
          *next_tick = *next_tick != UINT_MAX ? UINT_MAX : GetTickCount();
          break;

     default:
          break;
     }
}

// Handling menu item selections
void DispatchMenu(WPARAM val, savedata *gamesettings)
{
     switch (val)
     {
     case 1001:
          gamesettings->gamemap.x = SMALLMAPX;
          gamesettings->gamemap.y = SMALLMAPY;
          break;

     case 1002:
          gamesettings->gamemap.x = MEDIUMMAPX;
          gamesettings->gamemap.y = MEDIUMMAPY;
          break;

     case 1003:
          gamesettings->gamemap.x = LARGEMAPX;
          gamesettings->gamemap.y = LARGEMAPY;
          break;

     case 1011:
          gamesettings->gamescale = BIGSCALE;
          break;

     case 1012:
          gamesettings->gamescale = HUGESCALE;
          break;

     case 1013:
          gamesettings->gamemode = 0;
          break;

     case 1014:
          gamesettings->gamemode = 1;
          break;

     case 1100:
          gamesettings->lang = !(gamesettings->lang);
          break;

     default:
          break;
     }
}

savedata ReadSavegame() // No comments..
{
     savedata usersave = { .gamemap = {.x = MEDIUMMAPX, .y = MEDIUMMAPY},
                           .gamescale = BIGSCALE, .lang = 0, .gamemode = 0,
                           .maxs = 0 };
     HANDLE hFile = CreateFile(L"snake2.sav", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
     if (INVALID_HANDLE_VALUE == hFile) return usersave; // FIXME! Write check correctness later
     ReadFile(hFile, &usersave, sizeof(usersave), NULL, NULL);
     CloseHandle(hFile);
     return usersave;
}

gamelang ReadGamelang(int num)
{
     gamelang userlang = { .str1001 = L"Small",    .str1002 = L"Medium",        .str1003 = L"Large",
                           .str1008 = L"Map size", .str1009 = L"Scale",         .str1011 = L"Big",
                           .str1012 = L"Huge",     .str1013 = L"Single Player", .str1014 = L"Coop",
                           .str1100 = L"Lang",     .str1110 = L"Game Mode",
                           .str1501 = L"\nScore:\n\n%07i\n\nWins:\n\n%i",
                           .str1502 = L"\nScore\n\n%07i\n\nMax Score\n\n%07i",
                           .str1503 = L"\nTo start the game \njust select a direction\n\nSelecting direction - \narrows or «W S A D»\non the keyboard\n\nEscape - pause\n\n\nAll changes to game \nsettings require a \nrestart\n",
                         };
     if (num)
     {
          HANDLE hFile = CreateFile(L"snake.lng", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
          if (INVALID_HANDLE_VALUE == hFile) return userlang; // FIXME! Write check correctness later
          ReadFile(hFile, &userlang, sizeof(userlang), NULL, NULL);
          CloseHandle(hFile);
     }
     return userlang;
}

// Special function for make translation file
/*
void WriteGameLang(void)
{
     gamelang userlang = { .str1001 = L"Малый",        .str1002 = L"Средний",        .str1003 = L"Большой",
                           .str1008 = L"Размер карты", .str1009 = L"Масштаб",        .str1011 = L"Крупный",
                           .str1012 = L"Огромный",     .str1013 = L"Одиночная игра", .str1014 = L"Кооператив",
                           .str1100 = L"Язык Рус -> Eng",         .str1110 = L"Режим игры",
                           .str1501 = L"\nОчков:\n\n%07i\n\nПобед:\n\n%i",
                           .str1502 = L"\nОчков:\n\n%07i\n\nРекорд\n\n%07i",
                           .str1503 = L"\nВыберите направление \nчтобы играть\n\nУправление: \nстрелки или\n«W S A D»\nна клавиатуре\n\nEscape - пауза\n\n\nИзменение любых\nнастроек требует\nперезапуска\n",
                         };
     HANDLE hFile = CreateFile(L"snake.lng", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
     if (INVALID_HANDLE_VALUE == hFile) return;
     WriteFile(hFile, &userlang, sizeof(userlang), NULL, NULL);
     CloseHandle(hFile);
}
*/

void WriteSavegame(savedata const *gamesettings, int maxscore)
{
     HANDLE hFile = CreateFile(L"snake2.sav", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
     if (INVALID_HANDLE_VALUE == hFile) return;
     savedata usersave = { .gamemap = gamesettings->gamemap,
                           .gamescale = gamesettings->gamescale,
                           .gamemode = gamesettings->gamemode,
                           .lang = gamesettings->lang, .maxs = maxscore
                         };
     WriteFile(hFile, &usersave, sizeof(usersave), NULL, NULL);
     CloseHandle(hFile);
}

void RunAppCopy(void)
{
     wchar_t path[256];
     GetModuleFileNameW(0, path, 256); // Get full name of snake.exe
     STARTUPINFO si;  // Example of using CreateProcess from MSDN
     PROCESS_INFORMATION pi;
     ZeroMemory( &si,sizeof(si));
     si.cb = sizeof(si);
     ZeroMemory(&pi, sizeof(pi));
     CreateProcessW(path, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
     CloseHandle(pi.hProcess);
     CloseHandle(pi.hThread);
}