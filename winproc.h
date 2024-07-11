// Savefile structure and function prototypes

typedef struct savedata
{
     cpoint gamemap;
     int gamescale;
     int gamemaxscore;
} savedata;

void SnakeRestart(cpoint const *gamemap, int *ticks, snake *vyper);
cpoint GetApple(cpoint const *gamemap, int const *len, cpoint const *body);
int SnakeLogic(cpoint const *gamemap, cpoint *apple, int *ticks, snake *vyper);

void ActorsShow(HDC dc, cpoint const *gamemap, cpoint const *body, cpoint const *apple, int scale, int len);
void ScoresShow(HDC dc, int scale, int coins, int maxscore, HFONT font, RECT * const rt);
void DispatchVector(WPARAM key, cpoint * newvect, DWORD * next_tick);
void DispatchMenu(WPARAM val, cpoint * map, int * scale);
savedata ReadSavegame();
VOID WriteSavegame(cpoint maps, int scale, int maxs);