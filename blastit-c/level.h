 #include "blast_it.h"


#ifndef H_LEVEL
#define H_LEVEL

typedef struct {
	LevelInfo level_info;
	Entity ** entities; //stelt een tegel van het level voor
} Level;

int xcoord,ycoord;

LevelInfo generate_level_info(int level_nr);

void init_level(Level * level, LevelInfo level_info);

void render_level(Level * level);
//void render_player(Game* game);
void destroy_level(Level * level);
void render_map(Level* level);

#endif