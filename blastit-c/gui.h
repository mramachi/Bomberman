#include "blast_it.h"
#include "util.h"
#include "highscores.h"

#ifndef H_GUI
#define H_GUI

void gui_initialize(void);

void gui_clean(void);

void gui_set_level_info(LevelInfo * level_info);

void gui_add_powerup(Powerup * powerup);

void gui_add_enemy(Enemy * enemy);

void gui_add_player(Player * player);

void gui_add_obstacle(Obstacle * obstacle);

void gui_add_bomb(Bomb * bomb);

void gui_add_explosion_tile(int x, int y);

void gui_set_enemies_left(int enemies);

void gui_set_bombs_left(int bombs);

void gui_draw_buffer(void);

void gui_start_timer(void);

int gui_get_timer_score(void);

void gui_set_game_over(void);

void gui_set_finished_level(int level_score, int total_score);

void gui_game_event(Event *ev);



#endif