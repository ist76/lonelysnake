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
#define RENDERLAG 16  // Pause (ms) between game level redraw cycles

typedef struct savedata
{
     cpoint gamemap;  // Level size in cells
     int gamescale;   // Interface scale
     int gamemode;    // 0 - singleplayer 1 - COOP
     int lang;        // Interface language (only 0 or 1)
     int maxs;        // Global game best score
} savedata;

typedef struct gamelang
{
     wchar_t str1008[16];   // Map size
     wchar_t str1009[16];   // Scale
     wchar_t str1110[16];   // Game Mode
     wchar_t str1100[16];   // Lang
     wchar_t str1001[16];   // Small
     wchar_t str1002[16];   // Medium
     wchar_t str1003[16];   // Large
     wchar_t str1011[16];   // Big
     wchar_t str1012[16];   // Huge
     wchar_t str1013[16];   // Single Player
     wchar_t str1014[16];   // Coop
     wchar_t str1501[63];   // Scoreboard Message1
     wchar_t str1502[63];   // Scoreboard Message2
     wchar_t str1503[254];  // Scoreboard Message3
} gamelang;

savedata ReadSavegame();
gamelang ReadGamelang(int num);
void DispatchVector(WPARAM key, cpoint * newvect1, cpoint * newvect2, DWORD * next_tick, int mode);
void DispatchMenu(WPARAM val, savedata *gamesettings);
void WriteSavegame(savedata const *gamesettings, int maxscore);
void RunAppCopy(void);
//void WriteGameLang(void);  // See winio.c