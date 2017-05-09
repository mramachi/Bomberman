#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "blast_it.h"
#include "gui.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

#define TICK_PER_S 40

#define FONT_H 8

#define FONT_H2 (FONT_H/2)
#define FONT_1H2 (FONT_H2)
#define FONT_2H2 (FONT_H)
#define FONT_3H2 ((3*FONT_H)/2)
#define FONT_4H2 ((4*FONT_H)/2)
#define FONT_5H2 ((5*FONT_H)/2)
#define FONT_6H2 ((6*FONT_H)/2)
#define FONT_7H2 ((7*FONT_H)/2)
#define FONT_8H2 ((8*FONT_H)/2)
#define FONT_9H2 ((9*FONT_H)/2)
#define FONT_10H2 ((10*FONT_H)/2)
#define FONT_11H2 ((11*FONT_H)/2)

#define FONT_1H (FONT_H)
#define FONT_2H (2*FONT_H)
#define FONT_3H (3*FONT_H)
#define FONT_4H (4*FONT_H)
#define FONT_5H (5*FONT_H)
#define FONT_6H (6*FONT_H)

#define TILE_W 30
#define TILE_H 30


#define PLAYER_NORTH_X 0
#define PLAYER_SOUTH_X 1
#define PLAYER_EAST_X 2
#define PLAYER_WEST_X 3

#define PLAYER_DEAD_X 4
#define PLAYER_DEAD_Y 2

#define BOMB_X 4
#define BOMB_Y 0

#define EXP_SINGLE_X 4
#define EXP_SINGLE_Y 1

#define WALL_X 5
#define WALL_SOLID_Y 0
#define WALL_BREAKABLE_Y 1

#define EMPTY_X 5
#define EMPTY_Y 2

#define ENEMY_NORTH_X 6
#define ENEMY_SOUTH_X 7
#define ENEMY_EAST_X 8
#define ENEMY_WEST_X 9

#define QUESTION_X 10
#define QUESTION_Y 0

#define BOMB_EXP_X 10
#define BOMB_EXP_Y 1

#define EXP_X 11
#define EXP_CENTER_Y 0
#define EXP_VERT_Y 1
#define EXP_HORIZ_Y 2

#define GAME_OFFSET_X 10
#define GAME_OFFSET_Y FONT_11H2
#define GAME_EXTRA_W 10
#define GAME_EXTRA_H 10

#define EXPLOSION_FIRE 1
#define EXPLOSION_HAD_BOMB 2
#define EXPLOSION_HAD_WALL 4
#define EXPLOSION_HAD_PLAYER 8
#define EXPLOSION_HAD_ENEMY 16


#define CHECK_INIT { if(!ready) { printf("gui error: Please call the initialize method first!\n"); exit(-1); } }

typedef enum {GUI_KEY_UP = 1, GUI_KEY_DOWN = 2, GUI_KEY_RIGHT = 4, GUI_KEY_LEFT = 8, GUI_KEY_SPACE = 16, GUI_KEY_ESC = 32} GUI_KEY;

int ready = 0;

ALLEGRO_DISPLAY *display;
ALLEGRO_BITMAP **alltiles = NULL;
ALLEGRO_BITMAP *tiles = NULL;
ALLEGRO_BITMAP *icon = NULL;
ALLEGRO_FONT *font = NULL;

ALLEGRO_TIMER *timer;
ALLEGRO_EVENT_QUEUE *queue;

bool show_fps = 0;
int fps = 0, fps_accum = 0;
double fps_time = 0.0;

int keys=0;

bool is_game_over = 0;
bool is_finished = 0;
int bombs_left = 0;
int enemies_left = 0;
int level_score = 0;
int total_score = 0;

int level_time = 0;
int tick_time = 0;
int walk_time = 0;
int step_time = 0;
int boss_time = 0;

int screen_w=0,screen_h=0;
LevelInfo* cur_level = NULL;
int** explosions;




int x_topixel(int x) {
	return ((x * TILE_W) / TILE_SIZE) + GAME_OFFSET_X;
}

int y_topixel(int y) {
	return ((y * TILE_H) / TILE_SIZE) + GAME_OFFSET_Y;
}

int orientation_offset(ORIENTATION orientation) {
	switch (orientation) {
		case NORTH: return 0;
		case EAST: return 2;
		case SOUTH: return 1;
		case WEST: return 3;
	}
	printf("Bad orientation %d\n", orientation);
	exit(-1);
}

void draw_base_level() {
	//background
	al_draw_filled_rectangle(
		0, 0,
		screen_w+GAME_OFFSET_X+GAME_EXTRA_W, screen_h+GAME_OFFSET_Y+GAME_EXTRA_H,
		al_map_rgb(0, 0, 0));

	if (cur_level != NULL) {
		int x,y;
		for (x = 0; x < cur_level->width; x++)
			for (y = 0; y < cur_level->height; y++) {
				int sx,sy;

				if ((x == 0) || (x == cur_level->width-1) || (y == 0) || (y == cur_level->height-1)) {
					sx = WALL_X * TILE_W;
					sy = WALL_SOLID_Y * TILE_H;
				} else {
					sx = EMPTY_X * TILE_W;
					sy = EMPTY_Y * TILE_H;
				}

				al_draw_bitmap_region(tiles, sx, sy, TILE_W, TILE_H, (x * TILE_W) + GAME_OFFSET_X, (y * TILE_H) + GAME_OFFSET_Y, 0);

				if (explosions[x][y] & EXPLOSION_FIRE) {
					int neighbors_x = 0;
					int neighbors_y = 0;
					if (x > 0 && explosions[x-1][y] & EXPLOSION_FIRE) neighbors_x++;
					if (x < cur_level->width-1 && explosions[x+1][y] & EXPLOSION_FIRE) neighbors_x++;
					if (y > 0 && explosions[x][y-1] & EXPLOSION_FIRE) neighbors_y++;
					if (y < cur_level->height-1 && explosions[x][y+1] & EXPLOSION_FIRE) neighbors_y++;

					//default
					sx = EXP_SINGLE_X * TILE_W;
					sy = EXP_SINGLE_Y * TILE_H;
					if (explosions[x][y] & EXPLOSION_HAD_BOMB) {
						sx = EXP_X * TILE_W;
						sy = EXP_CENTER_Y * TILE_H;
					} else {
						if (neighbors_x >= 1 && neighbors_y >= 1) {
							sx = EXP_X * TILE_W;
							sy = EXP_CENTER_Y * TILE_H;
						} else if (neighbors_x >= 1) {
							sx = EXP_X * TILE_W;
							sy = EXP_HORIZ_Y * TILE_H;
						} else if(neighbors_y >= 1) {
							sx = EXP_X * TILE_W;
							sy = EXP_VERT_Y * TILE_H;
						}

						if (explosions[x][y] & EXPLOSION_HAD_WALL) {
							sx = EXP_SINGLE_X * TILE_W;
							sy = EXP_SINGLE_Y * TILE_H;
						}
					}
					al_draw_bitmap_region(tiles, sx, sy, TILE_W, TILE_H, (x * TILE_W) + GAME_OFFSET_X, (y * TILE_H) + GAME_OFFSET_Y, 0);

					if (explosions[x][y] & EXPLOSION_HAD_PLAYER || explosions[x][y] & EXPLOSION_HAD_ENEMY) {
						sx = EXP_SINGLE_X * TILE_W;
						sy = EXP_SINGLE_Y * TILE_H;
						al_draw_bitmap_region(tiles, sx, sy, TILE_W, TILE_H, (x * TILE_W) + GAME_OFFSET_X, (y * TILE_H) + GAME_OFFSET_Y, 0);
					}
				}
			}
			
		for (x = 0; x < cur_level->width; x++)
			for (y = 0; y < cur_level->height; y++) {
					if (explosions[x][y] & EXPLOSION_FIRE)
						explosions[x][y] &= ~EXPLOSION_FIRE;
					else
						explosions[x][y] = 0;
			}
	}
}

void draw_game_over_overlay() {
	al_draw_filled_rounded_rectangle(
		GAME_OFFSET_X+(TILE_W/2), GAME_OFFSET_Y+(TILE_H/2), 
		(GAME_OFFSET_X+screen_w)-(TILE_W/2), (GAME_OFFSET_Y+screen_h)-(TILE_H/2),
		TILE_W/2, TILE_H/2, al_map_rgba(0, 0, 0, 200));
	
	al_draw_textf(font, al_map_rgb(255, 255, 255),
	    GAME_OFFSET_X+(screen_w/2), GAME_OFFSET_Y+(screen_h/2)-FONT_H2, 
		ALLEGRO_ALIGN_CENTRE, "Game Over!", fps);
	al_draw_textf(font, al_map_rgb(255, 255, 255),
	    GAME_OFFSET_X+(screen_w/2), GAME_OFFSET_Y+(screen_h/2)+FONT_3H2, 
		ALLEGRO_ALIGN_CENTRE, "Press SPACE to end.", fps);
}

void draw_finished_overlay(int level_nr, int level_s, int total_s) {
	al_draw_filled_rounded_rectangle(
		GAME_OFFSET_X+(TILE_W/2), GAME_OFFSET_Y+(TILE_H/2), 
		(GAME_OFFSET_X+screen_w)-(TILE_W/2), (GAME_OFFSET_Y+screen_h)-(TILE_H/2),
		8, 8, al_map_rgba(0, 0, 0, 200));

	al_draw_textf(font, al_map_rgb(0, 255, 0),
	    GAME_OFFSET_X+(screen_w/2), GAME_OFFSET_Y+(screen_h/2)-FONT_9H2, 
		ALLEGRO_ALIGN_CENTRE, "Level %d Completed!", level_nr);
	al_draw_textf(font, al_map_rgb(0, 255, 0),
	    GAME_OFFSET_X+(screen_w/2), GAME_OFFSET_Y+(screen_h/2)-FONT_4H2, 
		ALLEGRO_ALIGN_CENTRE, "Level Score: %d", level_s);
	al_draw_textf(font, al_map_rgb(0, 255, 0),
	    GAME_OFFSET_X+(screen_w/2), GAME_OFFSET_Y+(screen_h/2), 
		ALLEGRO_ALIGN_CENTRE, "Total Score: %d", total_s);
	al_draw_textf(font, al_map_rgb(0, 255, 0),
	    GAME_OFFSET_X+(screen_w/2), GAME_OFFSET_Y+(screen_h/2)+FONT_6H2, 
		ALLEGRO_ALIGN_CENTRE, "Press SPACE to start next level.", fps);
}

void gui_game_event(Event *ev) {
	bool has_event = 0;

	while (!has_event) {
		ALLEGRO_EVENT event;
		al_wait_for_event(queue, &event);
 
		switch (event.type) {
			case ALLEGRO_EVENT_DISPLAY_CLOSE: {
				keys |= GUI_KEY_ESC;
				ev->keyEvent.type = EVENT_KEY;
				ev->keyEvent.key = keys;
				has_event = 1;
				break;
			}
			case ALLEGRO_EVENT_KEY_UP: {
				 switch(event.keyboard.keycode) {
					case ALLEGRO_KEY_PAD_8:
					case ALLEGRO_KEY_W:
					case ALLEGRO_KEY_Z:
					case ALLEGRO_KEY_UP:
					   keys &= ~GUI_KEY_UP;
					   break;
 
					case ALLEGRO_KEY_PAD_2:
					case ALLEGRO_KEY_S:
					case ALLEGRO_KEY_DOWN:
					   keys &= ~GUI_KEY_DOWN;
					   break;
 
					case ALLEGRO_KEY_PAD_4:
					case ALLEGRO_KEY_A:
					case ALLEGRO_KEY_Q:
					case ALLEGRO_KEY_LEFT: 
					   keys &= ~GUI_KEY_LEFT;
					   break;
 
					case ALLEGRO_KEY_PAD_6:
					case ALLEGRO_KEY_D:
					case ALLEGRO_KEY_RIGHT:
					   keys &= ~GUI_KEY_RIGHT;
					   break;
 
					case ALLEGRO_KEY_ENTER:
					case ALLEGRO_KEY_SPACE:
						keys &= ~GUI_KEY_SPACE;
					   break;
				 }
				 ev->keyEvent.type = EVENT_KEY;
				 ev->keyEvent.key = keys;
				 has_event = 1;
				 break;
			}
			case ALLEGRO_EVENT_KEY_DOWN: {
				 switch(event.keyboard.keycode) {
					case ALLEGRO_KEY_ESCAPE:
						keys |= GUI_KEY_ESC;
						break;

					case ALLEGRO_KEY_PAD_8:
					case ALLEGRO_KEY_W:
					case ALLEGRO_KEY_Z:
					case ALLEGRO_KEY_UP:
					   keys |= GUI_KEY_UP;
					   break;
 
					case ALLEGRO_KEY_PAD_2:
					case ALLEGRO_KEY_S:
					case ALLEGRO_KEY_DOWN:
					   keys |= GUI_KEY_DOWN;
					   break;
 
					case ALLEGRO_KEY_PAD_4:
					case ALLEGRO_KEY_A:
					case ALLEGRO_KEY_Q:
					case ALLEGRO_KEY_LEFT: 
					   keys |= GUI_KEY_LEFT;
					   break;
 
					case ALLEGRO_KEY_PAD_6:
					case ALLEGRO_KEY_D:
					case ALLEGRO_KEY_RIGHT:
					   keys |= GUI_KEY_RIGHT;
					   break;
 
					case ALLEGRO_KEY_ENTER:
					case ALLEGRO_KEY_SPACE:
						keys |= GUI_KEY_SPACE;
					   break;
				 }
				 ev->keyEvent.type = EVENT_KEY;
				 ev->keyEvent.key = keys;
				 has_event = 1;
				 break;
			}
			case ALLEGRO_EVENT_TIMER: {
				if(++tick_time >= TICK_PER_S){
					level_time++;
					tick_time = 0;
				}

				if (++step_time >= 5) {
					step_time = 0;

					if (++walk_time >= 3)
						walk_time = 1;

					if (++boss_time >= REALM_COUNT)
						boss_time = 0;
				}
				ev->timerEvent.type = EVENT_TIMER;
				has_event = 1;
				break;
			}
			case ALLEGRO_EVENT_DISPLAY_RESIZE: {
				al_acknowledge_resize(display);
				has_event = 0;
				break;
			}
			default: {
				has_event = 0;
				break;
			}
		}
	}
}

void gui_initialize() 
{
	int i;

    al_init();
    al_init_image_addon();
    al_init_primitives_addon();
    al_init_font_addon();
    al_install_keyboard();

    al_set_new_display_flags(ALLEGRO_WINDOWED);
    display = al_create_display(640, 480);
    al_set_window_position(display,10,10);
    al_set_window_title(display, "Blast It!");
 
#ifdef _WIN32
    icon = al_load_bitmap("images\\icon.bmp");
#else
    icon = al_load_bitmap("images/icon.bmp");
#endif
    if (icon)
        al_set_display_icon(display, icon);

#ifdef _WIN32
    font = al_load_font("images\\fixed_font.tga", 0, 0);
#else
    font = al_load_font("images/fixed_font.tga", 0, 0);
#endif
	if (font == NULL) {
		printf("Failed to load font\n");
		exit(-1);
	}
 
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
 
    timer = al_create_timer(1.0 / TICK_PER_S);
    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_start_timer(timer);

	explosions = (int**) calloc(MAX_LEVEL_WIDTH, sizeof(int*));
	for (i = 0; i < MAX_LEVEL_WIDTH; i++)
		explosions[i] = (int*)calloc(MAX_LEVEL_HEIGHT, sizeof(int));

	alltiles = (ALLEGRO_BITMAP**) calloc(5, sizeof(ALLEGRO_BITMAP*));

	for (i = 0; i < REALM_COUNT; i++) {
		char sprite_filename[255];
#ifdef _WIN32
		sprintf_s(sprite_filename, 254, "images\\simple_sprite%d.tga", i+1);
#else
		snprintf(sprite_filename, 254, "images/simple_sprite%d.tga", i+1);
#endif
		alltiles[i] = al_load_bitmap(sprite_filename);
		if (alltiles[i] == NULL){
			printf("Failed to load spites for realm %d\n", i);
			exit(-1);
		}
	}
	tiles = alltiles[0];

	ready = 1;
}

void gui_clean() {
	int i;

	CHECK_INIT;

	ready = 0;
	if (alltiles != NULL) 
		for (i = 0; i < REALM_COUNT; i++) if (alltiles[i] != NULL) al_destroy_bitmap(alltiles[i]);
	free(alltiles);
	alltiles = NULL;
	tiles = NULL;
	
	al_stop_timer(timer);
	al_unregister_event_source(queue, al_get_keyboard_event_source());
	al_unregister_event_source(queue, al_get_display_event_source(display));
	al_unregister_event_source(queue, al_get_timer_event_source(timer));
	al_destroy_event_queue(queue);
	
	if (font != NULL) al_destroy_font(font);
	if (icon != NULL) al_destroy_bitmap(icon);

	al_destroy_display(display);

	al_uninstall_keyboard();
	al_shutdown_font_addon();
	al_shutdown_primitives_addon();
	al_shutdown_image_addon();

    al_uninstall_system();

	for (i = 0; i < MAX_LEVEL_WIDTH; i++)
		free(explosions[i]);
	free(explosions);
}

void gui_set_level_info(LevelInfo * level_info) 
{
	CHECK_INIT;

	if(level_info->width <= MAX_LEVEL_WIDTH && level_info->height <= MAX_LEVEL_HEIGHT) {
		screen_w = level_info->width * TILE_W;
		screen_h = level_info->height * TILE_H;
		
		if(level_info->realm < 5 && level_info->realm >= 0) {
			tiles = alltiles[level_info->realm];
		} else {
			printf("Illegal realm: %d", level_info->realm);
			exit(-1);
		}

		al_resize_display(display, screen_w+GAME_OFFSET_X+GAME_EXTRA_W, screen_h+GAME_OFFSET_Y+GAME_EXTRA_H);

		cur_level = level_info;

		is_finished = 0;
		is_game_over = 0;

		draw_base_level();
	} else {
		printf("Illegal level size %dx%d\n", level_info->width, level_info->height);
		exit(-1);
	}
}


void gui_add_powerup(Powerup *  powerup) 
{
	CHECK_INIT;

	if(powerup->x < MAX_LEVEL_WIDTH*TILE_SIZE && powerup->y < MAX_LEVEL_HEIGHT*TILE_SIZE) {
		int sx = QUESTION_X * TILE_W;
		int sy = QUESTION_Y * TILE_H;
		ALLEGRO_BITMAP *powerup_tiles = tiles;

		if(powerup->powerup_type == EXTRA_POWER) {
			powerup_tiles = alltiles[0];
		} else if(powerup->powerup_type == EXTRA_BOMB) {
			powerup_tiles = alltiles[1];
		} else if(powerup->powerup_type == FREEZE_ENEMIES) {
			powerup_tiles = alltiles[3];
		}
		al_draw_bitmap_region(powerup_tiles, sx, sy, TILE_W, TILE_H, x_topixel(powerup->x), y_topixel(powerup->y), 0);
	}
	
}

void gui_add_enemy(Enemy * enemy) 
{
	CHECK_INIT;

	if(enemy->x < MAX_LEVEL_WIDTH*TILE_SIZE && enemy->y < MAX_LEVEL_HEIGHT*TILE_SIZE) {
		int sx = (ENEMY_NORTH_X + orientation_offset(enemy->move_direction)) * TILE_W;
		int sy = 0;
		ALLEGRO_BITMAP *enemy_tiles;

		if(enemy->is_boss) {
			enemy_tiles = alltiles[boss_time];
		} else {
			enemy_tiles = tiles;
		}
		if (!enemy->frozen) { 
			if (enemy->move_direction == NORTH || enemy->move_direction == SOUTH) {
				sy = walk_time * TILE_H;
			} else {
				if (walk_time <= 1)
					sy = walk_time * TILE_H;
			}
		}
		al_draw_bitmap_region(enemy_tiles, sx, sy, TILE_W, TILE_H, x_topixel(enemy->x), y_topixel(enemy->y), 0);

		if (enemy->is_dead)
			explosions[(enemy->x  + (TILE_SIZE / 2)) / TILE_SIZE][(enemy->y  + (TILE_SIZE / 2)) / TILE_SIZE] |= EXPLOSION_HAD_ENEMY;
	}
}


void gui_add_player(Player * player)
{
	CHECK_INIT;

	if(player->x < MAX_LEVEL_WIDTH*TILE_SIZE && player->y < MAX_LEVEL_HEIGHT*TILE_SIZE) {
		int sx = (PLAYER_NORTH_X + orientation_offset(player->orientation)) * TILE_W;
		int sy = 0;
		if (keys != 0) sy = walk_time*TILE_H;

		if (is_game_over || player->is_dead) {
			sx = PLAYER_DEAD_X * TILE_W;
			sy = PLAYER_DEAD_Y * TILE_H;
		}

		al_draw_bitmap_region(tiles, sx, sy, TILE_W, TILE_H, x_topixel(player->x), y_topixel(player->y), 0);
		
		if (player->is_dead)
			explosions[(player->x + (TILE_SIZE / 2)) / TILE_SIZE][(player->y + (TILE_SIZE / 2)) / TILE_SIZE] |= EXPLOSION_HAD_PLAYER;	
	}
}

void gui_add_obstacle(Obstacle * obstacle) 
{
	CHECK_INIT;

	if(obstacle->x < MAX_LEVEL_WIDTH*TILE_SIZE && obstacle->y < MAX_LEVEL_HEIGHT*TILE_SIZE) {
		int sx = WALL_X * TILE_W;
		int sy;
		if (obstacle->is_destructable)
			sy = WALL_BREAKABLE_Y * TILE_H;
		else
			sy = WALL_SOLID_Y * TILE_H;
		al_draw_bitmap_region(tiles, sx, sy, TILE_W, TILE_H, x_topixel(obstacle->x), y_topixel(obstacle->y), 0);
		
		if (obstacle->is_destructable)
			explosions[obstacle->x / TILE_SIZE][obstacle->y / TILE_SIZE] |= EXPLOSION_HAD_WALL;	
	}
}

void gui_add_bomb(Bomb * bomb)
{
	CHECK_INIT;

	if(bomb->x < MAX_LEVEL_WIDTH*TILE_SIZE && bomb->y < MAX_LEVEL_HEIGHT*TILE_SIZE) {			
		int sx = BOMB_X * TILE_W;
		int sy = BOMB_Y * TILE_H;
		al_draw_bitmap_region(tiles, sx, sy, TILE_W, TILE_H, x_topixel(bomb->x), y_topixel(bomb->y), 0);

		explosions[bomb->x / TILE_SIZE][bomb->y / TILE_SIZE] |= EXPLOSION_HAD_BOMB;	
	}
}

void gui_add_explosion_tile(int x, int y)
{	
	int tx,ty;

	CHECK_INIT;
	
	if(x < MAX_LEVEL_WIDTH*TILE_SIZE && y < MAX_LEVEL_HEIGHT*TILE_SIZE) {
		if (x % TILE_SIZE != 0 || y % TILE_SIZE != 0) {
			printf("GUI error: Coordinate of explosion must be at tile edge: x=%d y=%d\n", x, y);
			exit(-1);
		}
		tx = x / TILE_SIZE;
		ty = y / TILE_SIZE;

		explosions[tx][ty] |= EXPLOSION_FIRE;		
	}
}

void gui_set_enemies_left(int nr_of_enemies)
{
	CHECK_INIT;

	enemies_left = nr_of_enemies;
}

void gui_set_bombs_left(int nr_of_bombs)
{
	CHECK_INIT;

	bombs_left = nr_of_bombs;
}

void gui_draw_buffer()
{
    double t = al_get_time();
	CHECK_INIT;

    if (font) {
		if (show_fps) {
			al_draw_filled_rounded_rectangle(4, 4, 100, 30,
				8, 8, al_map_rgba(0, 0, 0, 200));
			al_draw_textf(font, al_map_rgb(255, 255, 255),
	            54, 8, ALLEGRO_ALIGN_CENTRE, "FPS: %d", fps);
		}
		al_draw_filled_rectangle(0, 0, GAME_OFFSET_X+GAME_EXTRA_W+screen_w, GAME_OFFSET_Y,
			al_map_rgb(0, 0, 0));

		al_draw_filled_rectangle(0, GAME_OFFSET_Y+screen_h, GAME_OFFSET_X+GAME_EXTRA_W+screen_w, GAME_OFFSET_Y+screen_h+GAME_EXTRA_H,
			al_map_rgb(0, 0, 0));
		
		al_draw_textf(font, al_map_rgb(255, 255, 255),
	        GAME_OFFSET_X, FONT_H, ALLEGRO_ALIGN_LEFT, "Bombs left: %d", bombs_left);
		al_draw_textf(font, al_map_rgb(255, 255, 255),
	        GAME_OFFSET_X, FONT_3H, ALLEGRO_ALIGN_LEFT, "Enemies left: %d", enemies_left);
		
		if (cur_level != NULL)
			al_draw_textf(font, al_map_rgb(255, 255, 255),
				GAME_OFFSET_X+screen_w, FONT_H, ALLEGRO_ALIGN_RIGHT, "Level: %d", cur_level->level_nr);
		al_draw_textf(font, al_map_rgb(255, 255, 255),
	        GAME_OFFSET_X+screen_w, FONT_3H, ALLEGRO_ALIGN_RIGHT, "Time: %d", level_time);
    }
	
	if (is_game_over)
		draw_game_over_overlay();
	
	if (is_finished && cur_level != NULL)
		draw_finished_overlay(cur_level->level_nr, level_score, total_score);
	
    al_flip_display();
    fps_accum++;
    if (t - fps_time >= 1) {
        fps = fps_accum;
        fps_accum = 0;
        fps_time = t;
    }

	draw_base_level();
}

void gui_start_timer()
{
	CHECK_INIT;

	level_time = 0;
	tick_time = 0;
}

int gui_get_timer_score()
{
	CHECK_INIT;

	return level_time;
}

void gui_set_game_over()
{
	CHECK_INIT;

	is_game_over = 1;
}

void gui_set_finished_level(int level_s, int total_s)
{
	CHECK_INIT;

	is_finished = 1;
	level_score = level_s;
	total_score = total_s;
}
