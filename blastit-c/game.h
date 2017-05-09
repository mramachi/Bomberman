#include "blast_it.h"
#include "level.h"

#ifndef H_GAME
#define H_GAME
#define COLLISION_DISTANCE 10

typedef struct {
	int hasMoved;
	int moves[4];
	int dropBomb;
} Input;

typedef struct {
	Level level;
	Player player;
	Enemy * enemies;
	int enemies_left;
	int game_over;
	Input input;
	int score;
} Game;


/*Get the "floor"-tile*/
int get_tile(int position);

/*Get the "ceil"-tile*/
int get_up_tile(int position);

/*Get the edges of the player/enemy that is at position (x,y)*/
void get_edges(int x, int y, int * leftedge, int * rightedge, int * upperedge, int * underedge);

/*Get the next position of (x,y) in direction*/
void get_next_position(Game* game, int * x, int * y, int direction,int player,int boss);

/*Get the previous position of (x,y) in direction*/
void get_previous_position(int * x, int * y, int direction,int player,int boss);

/*From int to ORIENTATION*/
ORIENTATION get_direction(int dir);

/*Get the powerup*/
POWERUP_TYPE get_powerup(int powerup);

/*At the beginning of the level: gets a random position for the enemies to start (not in upper left corner)*/
void get_random_position(int * x, int * y, ORIENTATION * direction, Game * game);

/*See if the edges of two elements overlapped*/
int edge_overlap(int l1, int r1, int u1, int un1, int l2, int r2, int u2, int un2);

/*See if player collides with enemy or enemies collide with each other (return 1), else return 0*/
int enemy_collision(Game * game, int x, int y, int direction);

/*Check every possible collision*/
	/*
	Returns 0 if no collision
	Returns 1 if the player collided and needs to go back (so collided with object and keeps standing still)
	Returns 2 if the player must die
	Returns 3 if player encountered a Powerup
	Returns 4,5,6... - ... if Enemy number 0,1,2,... collided
	*/
void collision_detection(Game * game, int * xbonus, int * ybonus, int * col1, int * col2);

/*The function with the actual collision_detection*/
	/*
	Returns 0 if no collision
	Returns 1 if the player collided and needs to go back (so collided with object and keeps standing still)
	Returns 2 if the player must die
	Returns 3 if player encountered a Powerup
	Returns 4,5,6... - ... if Enemy number 0,1,2,... collided
	*/
int collided (int x, int y, Game* game,int player, int enemy, int * xbonus, int * ybonus);

/*Draw the enemies*/
void draw_enemies(Game *  game);

/*Control the explosions (ticks)*/
void do_explosion(Game* game);

/*Create the explosion and update the spread*/
void create_explosion(Game * game);

/*Add explosion tiles with only one collision on each side*/
void explode(Game * game);

/*Creates the player*/
void create_player(Game * game);

/*Creates the enemy*/
void create_enemies(Game * game);

/*Can we drop a bomb?*/
int enough_bombs(Game * game);

/*Initialize game*/
void init_game(Game * game, int level_nr);

/*Game loop*/
void do_game_loop(Game * game, int previous_score);

/*Handle keys*/
void check_game_input(Game * game, int key);

/*Update game*/
void update_game(Game * game);

/*Render game*/
void render_game(Game * game);

/*Destroy game*/
void destroy_game(Game * game);

/*Wait untill we press space*/
void wait_for_space();

/*Player movement*/
void do_player_movement(Game * game);

/*Artificial Intelligence Enemies*/
void do_enemy_ai(Game * game);

/*Is (xtile,ytile) out of bounds?*/
int out_of_bounds(int xtile, int ytile, Game * game);

/*Freeze enemies*/
void freeze_enemies(Game * game);

/*Enemy AI*/
void enemy_crosspoint_ai(Game * game, int i);

/*Bonus items*/
void process_bonus_items(Game * game, int x , int y);

/*Update spread*/
void update_spread(Game * game);

/*Bombs*/
void process_bombs(Game * game);

/*Compare key and value*/
int compare(int key, int value);

#endif