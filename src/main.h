// Declarations for main()

int SnakeLogic(cpoint const *gamemap, fruit *apple, int *ticks, snake *vyper, snake * wutu);
void SnakeRestart(cpoint const *gamemap, snake *vyper, snake *wutu, int *ticks, fruit *apple, int gamemode);
void GetSnakesCells(actors *allobj, snake const *vyper, snake const *wutu, int scale, int mode);
void GetSnakeColors(actors *allobj, int mode);
void GetGrid(actors *allobj, cpoint map, int scale);
void SetApple(actors *allobj, fruit *apple, int scale);
void ActorsShow(HDC dc, actors *allobj, fruit const *apple, int mode);
void ScoresShow(HDC dc, snake *vyper, HFONT font, RECT * const rt, wchar_t *message, int mode);
void SolutionShow(HDC dc, HFONT font, RECT * const rt, wchar_t *message);
fruit GetFruit(cpoint const *gamemap, snake *vyper, snake *wutu);