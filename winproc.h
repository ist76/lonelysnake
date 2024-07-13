// Savefile structure and function prototypes

// Constants for possible game level sizes and application scaling
#define SMALLMAPX 24
#define SMALLMAPY 16
#define MEDIUMMAPX 30
#define MEDIUMMAPY 20
#define LARGEMAPX 36
#define LARGEMAPY 24
#define BIGSCALE 32
#define HUGESCALE 38

typedef struct savedata
{
     cpoint gamemap;
     int gamescale;
     int gamemaxscore;
} savedata;

int SnakeLogic(cpoint const *gamemap, cpoint *apple, int *ticks, snake *vyper);
savedata ReadSavegame();
cpoint GetApple(cpoint const *gamemap, int const *len, cpoint const *body);
void SnakeRestart(cpoint const *gamemap, int *ticks, snake *vyper);
void ActorsShow(HDC dc, cpoint const *gamemap, cpoint const *body, cpoint const *apple, int scale, int len);
void ScoresShow(HDC dc, int scale, int coins, int maxscore, HFONT font, RECT * const rt);
void SolutionShow(HDC dc, int scale, HFONT font, RECT * const rt);
void DispatchVector(WPARAM key, cpoint * newvect, DWORD * next_tick);
void DispatchMenu(WPARAM val, cpoint * map, int * scale);
void WriteSavegame(cpoint maps, int scale, int maxs);
void RunAppCopy(void);

