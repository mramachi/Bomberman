
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "blast_it.h"
#include "gui.h"
#include "game.h"
#include "util.h"
#include "level.h"

#define INIT_BOMB_POWER 2
#define INIT_NUMBER_BOMBS 1


/*Global variables*/
int enemy2[10]={0,0,0,0,0,0,0,0,0,0}; //Never more than 10 enemies
int same_explosion=0;
int ticks;


void destruct_object(Game * game,int x, int y);
void set_game_score_enemy(Game* game);
void set_game_score_boss(Game* game);

void do_bombs(Game* game){
	int x_bomb,y_bomb;

	if(game->input.dropBomb==1 && game->player.remaining_bombs>0){  
		if(game->player.x - get_tile(game->player.x) < get_up_tile(game->player.x) - game->player.x){
			x_bomb = get_tile(game->player.x);
		}
		else{
			x_bomb = get_up_tile(game->player.x);
		}

		if(game->player.y - get_tile(game->player.y) < get_up_tile(game->player.y) - game->player.y){
			y_bomb = get_tile(game->player.y);
		}
		else{
			y_bomb = get_up_tile(game->player.y);
		}


		if(game->level.entities	[y_bomb/TILE_SIZE][x_bomb/TILE_SIZE].type!=BOMB){ //prevents stacking
			game->level.entities[y_bomb/TILE_SIZE][x_bomb/TILE_SIZE].bomb.type=BOMB;
			game->level.entities[y_bomb/TILE_SIZE][x_bomb/TILE_SIZE].bomb.x=x_bomb;
			game->level.entities[y_bomb/TILE_SIZE][x_bomb/TILE_SIZE].bomb.y=y_bomb;
			game->level.entities[y_bomb/TILE_SIZE][x_bomb/TILE_SIZE].type=BOMB;
			game->level.entities[y_bomb/TILE_SIZE][x_bomb/TILE_SIZE].bomb.ticks_left=BOMB_TICKS;

			game->player.remaining_bombs--;
		}
	}

	game->input.dropBomb=0;


}

int square(int a){

	return a*a;
}

int collision_explosie(Game* game, int player,int enemy){

	/*
	explosie gaat enkel raken als:
	case 0 : geen explosie
	case 2 : geraakt
	*/

	int x1,y1,verschil_x1,verschil_y1;
	int x2 = game->level.level_info.width, y2 = game->level.level_info.height,verschil_x2 = game->level.level_info.width, verschil_y2 = game->level.level_info.height;
	int hitarray[] = {0,0,0,0}; //To see if the player is close to any border (so "small" collisions are not detected)

	if(player==1){
		x1=get_tile(game->player.x)/TILE_SIZE;
		y1=get_tile(game->player.y)/TILE_SIZE;

		if(game->player.x % TILE_SIZE != 0){
			x2 = get_up_tile(game->player.x)/TILE_SIZE;
		}
		if(game->player.y % TILE_SIZE != 0){
			y2 = get_up_tile(game->player.y)/TILE_SIZE;
		}

	}
	else{
		x1=get_tile(game->enemies[enemy].x)/TILE_SIZE;
		y1=get_tile(game->enemies[enemy].y)/TILE_SIZE;

		if(game->enemies[enemy].x % TILE_SIZE != 0){
			x2 = get_up_tile(game->enemies[enemy].x)/TILE_SIZE;
		}
		if(game->enemies[enemy].y % TILE_SIZE != 0){
			y2 = get_up_tile(game->enemies[enemy].y)/TILE_SIZE;
		}


	}

	if(player==1){
		if((TILE_SIZE - (game->player.x % TILE_SIZE)) < COLLISION_DISTANCE && (TILE_SIZE - (game->player.x % TILE_SIZE)) != 0){
			hitarray[0]=game->player.x/TILE_SIZE;  //The player goes a little bit over the left bound to the tile in hitarray[0]
		}
		if(game->player.x % TILE_SIZE < COLLISION_DISTANCE && game->player.x % TILE_SIZE != 0){
			hitarray[1]=game->player.x/TILE_SIZE +1; //The player goes a little bit over the right bound to the tile in hitarray[1]
		}
		if((TILE_SIZE - (game->player.y % TILE_SIZE)) < COLLISION_DISTANCE && (TILE_SIZE -(game->player.y % TILE_SIZE)) != 0){
			hitarray[2] = game->player.y/TILE_SIZE; //The player goes a little bit over the upper bound to the tile in hitarray[2]
		}
		if(game->player.y % TILE_SIZE < COLLISION_DISTANCE && game->player.y % TILE_SIZE != 0){
			hitarray[3] = game->player.y/TILE_SIZE +1;
		}
		verschil_y1=square(ycoord-y1);
		verschil_x1=square(xcoord-x1);

		if(x2 != game->level.level_info.width)
			verschil_x2 = square(xcoord-x2);
		if(y2 != game->level.level_info.height) 
			verschil_y2 = square(ycoord - y2);
	}
	else{
		verschil_y1 = ycoord-y1;
		verschil_x1= xcoord-x1;

		if(x2 != game->level.level_info.width)
			verschil_x2 = xcoord-x2;
		if(y2 != game->level.level_info.height) 
			verschil_y2 = ycoord - y2;
	}






	if(player==1){
		/*Op dezelfde y*/
		if(verschil_y1==0 || verschil_y2 == 0){
			if(verschil_x1<=square(game->level.entities[ycoord][xcoord].explosion.spread[0])){  //LINKS
				if(hitarray[2] == ycoord || hitarray[3] == ycoord){  //So if the player is a bit on the ycoord
					return 0;
				}
				if(hitarray[1]!=0 && hitarray[1]<= xcoord - game->level.entities[ycoord][xcoord].explosion.spread[0]){
					return 0;
				}
				return 2;
			}
			if(verschil_x1<=square(game->level.entities[ycoord][xcoord].explosion.spread[1])){  //RECHTS
				if(hitarray[2] == ycoord || hitarray[3] == ycoord){  //So if the player is a bit on the ycoord
					return 0;
				}
				if( hitarray[0]>= (xcoord + game->level.entities[ycoord][xcoord].explosion.spread[1])){
					return 0;
				}
				return 2;
			}
			if(verschil_x2 != game->level.level_info.width && (verschil_x2 <= square(game->level.entities[ycoord][xcoord].explosion.spread[0]))){   //LINKS
				if(hitarray[2] == ycoord || hitarray[3] == ycoord){  //So if the player is a bit on the ycoord
					return 0;
				}
				if(hitarray[1]!=0 && hitarray[1]<= xcoord - game->level.entities[ycoord][xcoord].explosion.spread[0]){
					return 0;
				}
				return 2;
			}
			if(verschil_x2 != game->level.level_info.width && verschil_x2 <= square(game->level.entities[ycoord][xcoord].explosion.spread[1])){ //RECHTS
				if(hitarray[2] == ycoord || hitarray[3] == ycoord){  //So if the player is a bit on the ycoord
					return 0;
				}
				if( hitarray[0]>= (xcoord + game->level.entities[ycoord][xcoord].explosion.spread[1])){
					return 0;
				}
				return 2;
			}
		}



		/*Op dezelfde x*/
		if(verschil_x1==0 || verschil_x2 == 0){
			if(verschil_y1<=square(game->level.entities[ycoord][xcoord].explosion.spread[3])){  //ONDER
				if((hitarray[0]== xcoord) ||  ( hitarray[1] == xcoord)){  //So if the player is a bit on the xcoord
					return 0;
				}
				if(hitarray[2]>= (ycoord + game->level.entities[ycoord][xcoord].explosion.spread[3])){
					return 0;
				}
				return 2;
			}
			if(verschil_y1<=square(game->level.entities[ycoord][xcoord].explosion.spread[2])){  //BOVEN
				if( hitarray[0]== xcoord ||   hitarray[1] == xcoord){  //So if the player is a bit on the xcoord
					return 0;
				}
				if(hitarray[3]!= 0 && hitarray[3]<=(ycoord - game->level.entities[ycoord][xcoord].explosion.spread[2])){
					return 0;
				}
				return 2;
			}

			if(verschil_y2 != game->level.level_info.height && (verschil_y2 <= square(game->level.entities[ycoord][xcoord].explosion.spread[3]))){
				if((hitarray[0]== xcoord) ||  ( hitarray[1] == xcoord)){  //So if the player is a bit on the xcoord
					return 0;
				}
				if(hitarray[2]>= (ycoord + game->level.entities[ycoord][xcoord].explosion.spread[3])){
					return 0;
				}
				return 2;
			}
			if(verschil_y2 != game->level.level_info.height && verschil_y2 <= square(game->level.entities[ycoord][xcoord].explosion.spread[2])){
				if( hitarray[0]== xcoord ||   hitarray[1] == xcoord){  //So if the player is a bit on the xcoord
					return 0;
				}
				if(hitarray[3]!= 0 && hitarray[3]<=(ycoord - game->level.entities[ycoord][xcoord].explosion.spread[2])){
					return 0;
				}
				return 2;
			}
		}
	}
	else{
		/*Op dezelfde y*/
		if(verschil_y1==0 || verschil_y2 == 0){
			if(verschil_x1<0){  //als xcoord-x <0 is x groter dan xcoord -> rechts
				if(square(verschil_x1)<=square(game->level.entities[ycoord][xcoord].explosion.spread[1])){
					return 2;
				}

			}
			else{
				if(verschil_x1<=game->level.entities[ycoord][xcoord].explosion.spread[0]){
					return 2;
				}
			}
			if(verschil_x2 !=game->level.level_info.width){
				if(verschil_x2<0){
					if(square(verschil_x2) <= square(game->level.entities[ycoord][xcoord].explosion.spread[1])){
						return 2;
					}
				}
				else{
					if(verschil_x2 <= game->level.entities[ycoord][xcoord].explosion.spread[0]){
						return 2;
					}
				}
			}
		}



		/*Op dezelfde x*/
		if(verschil_x1==0 || verschil_x2 == 0){
			if(verschil_y1<0){ //als ycoord-y<0 dan is y groter of meer naar beneden dan ycoord -> south
				if(square(verschil_y1)<=square(game->level.entities[ycoord][xcoord].explosion.spread[3])){
					return 2;
				}
			}
			else{
				if(verschil_y1<=game->level.entities[ycoord][xcoord].explosion.spread[2]){
					return 2;
				}
			}


			if(verschil_y2 != game->level.level_info.height){
				if(verschil_y2<0){
					if(square(verschil_y2 )<=square(game->level.entities[ycoord][xcoord].explosion.spread[3])){
						return 2;
					}
				}
				else{
					if(verschil_y2 <= game->level.entities[ycoord][xcoord].explosion.spread[2]){
						return 2;
					}

				}
			}
		}

	}

	return 0;


}

int get_up_tile(int position){
	return position - position % TILE_SIZE +TILE_SIZE;
}

int collided (int x, int y, Game* game,int player, int enemy, int * xbonus, int * ybonus){
	/*
	Returns 0 if no collision
	Returns 1 if the player collided and needs to go back (so collided with object and keeps standing still)
	Returns 2 if the player must die
	Returns 3 if player encountered a Powerup
	Returns 4,5,6... - ... if Enemy number 0,1,2,... collided
	*/

	/*The int player is a boolean to see if there is a player or an enemy*/
	/*The int enemy gives the number of the enemy in the array, -1 if player*/

	int rij=y/TILE_SIZE;
	int kolom=x/TILE_SIZE;

	if(enemy_collision(game,game->player.x,game->player.y,game->player.orientation)==1 && player ==1){  //Player encountered an enemy -> he is dead
		enemy_collision(game,game->player.x,game->player.y,game->player.orientation); //TODO: weghalen: dit was voor debuggen
		return 2;
	}
	if(player==1){
		if(game->level.entities[rij][kolom].type==OBSTACLE){
			return 1;
		}
		if(game->level.entities[rij][kolom].type==POWERUP){
			*xbonus = kolom * TILE_SIZE;
			*ybonus = rij * TILE_SIZE;
			return 3;
		}
	}

	if(player==0){
		if(enemy_collision(game,game->enemies[enemy].x,game->enemies[enemy].y,game->enemies[enemy].move_direction)==1){			
			return enemy+4;
		}

		if(game->level.entities[rij][kolom].type==OBSTACLE ){
			return enemy+4;
		}

		if(game->level.entities[rij][kolom].type==BOMB){
			return enemy+4;
		}

	}



	return 0;



}

int enemy_collision(Game * game, int x, int y, int direction){

	/*Returns one if there is an enemy there, 0 if not*/
	int i;

	for(i=0;i<game->level.level_info.nr_of_enemies;i++){
		if(game->enemies[i].is_dead==0){
			int leftp, rightp, upperp, underp;
			int lefte, righte, uppere,undere;

			get_edges(x,y,&leftp,&rightp,&upperp,&underp);
			get_edges(game->enemies[i].x,game->enemies[i].y, &lefte, &righte, &uppere, &undere);

			if(direction!= -1 && (get_direction(direction)!=game->enemies[i].move_direction || (x==game->player.x && y==game->player.y && direction == (int) game->player.orientation)) && game->enemies[i].is_dead==0){
				if(edge_overlap(leftp,rightp,upperp,underp,lefte,righte,uppere,undere)==1){
					return 1;
				}
			}
			if(direction==-1){
				int x1,x2,x3,x4;
				int y1,y2,y3,y4;

				x1 = get_tile(game->enemies[i].x);
				if(game->enemies[i].x % TILE_SIZE != 0)
					x2 = get_up_tile(game->enemies[i].x);
				else 
					x2 = -1;

				x3 = get_tile(x);
				if(x % TILE_SIZE != 0)
					x4 = get_up_tile(game->enemies[i].x);
				else 
					x4 = -1;

				y1 = get_tile(game->enemies[i].y);
				if(game->enemies[i].y % TILE_SIZE != 0)
					y2 = get_up_tile(game->enemies[i].y);
				else 
					y2 = -1;

				y3 = get_tile(y);
				if(y % 30 != 0)
					y4 = get_up_tile(game->enemies[i].y);
				else 
					y4 = -1;

				if((x1==x3 && y1==y3)  || (x1 == x4 && y1 == y3) || (x2 == x3 && y1 == y3) || (x2 == x4 && y1 == y3 && x2 != -1 ) ){
					return 1;
				}
				else if((x1==x3 && y1==y4)  || (x1 == x4 && y1 == y4) || (x2 == x3 && y1 == y4) || (x2 == x4 && y1 == y4 && x2 != -1) ){
					return 1;
				}
				else if((x1==x3 && y2==y3)  || (x1 == x4 && y2 == y3) || (x2 == x3 && y2 == y3) || (x2 == x4 && y2 == y3 && x2 != -1) ){
					return 1;
				}
				else if((x1==x3 && y2==y4 && y2 !=-1)  || (x1 == x4 && y2 == y4 && y2 != -1) || (x2 == x3 && y2 == y4 && y2 != -1) || (x2 == x4 && y2 == y4 && x2 != -1 && y2 != -1) ){
					return 1;
				}
			}


		}
	}

	return 0;
}

void get_edges(int x, int y, int * leftedge, int * rightedge, int * upperedge, int * underedge){
	*leftedge=x;
	*upperedge=y;
	*rightedge=x+TILE_SIZE;
	*underedge=y+TILE_SIZE;
}

int edge_overlap(int l1, int r1, int u1, int un1, int l2, int r2, int u2, int un2){
	int leftoverlap=0, rightoverlap=0,marge1=0,marge2=0;;
	if((l1<=r2 && l1>=l2)){
		leftoverlap=1;
		if((r2-l1) < COLLISION_DISTANCE || (l1-l2)<COLLISION_DISTANCE){
			marge1=1;
		}
	} 
	if(l2<=r1 && l2>=l1){
		leftoverlap=1;
		if((r1-l2)<COLLISION_DISTANCE || (l2-l1)<COLLISION_DISTANCE){
			marge1=1;
		}
	}
	if(r2>=l1 && r2<=r1){
		leftoverlap=1;
		if((r2-l1)<COLLISION_DISTANCE || (r1-r2)<COLLISION_DISTANCE){
			marge1=1;
		}
	}
	if(r1>=l2 && r1<=r2){
		leftoverlap=1;	
		if((r1-l2)<COLLISION_DISTANCE || (r2-r1)<COLLISION_DISTANCE){
			marge1=1;
		}
	}

	if(un1>=u2 && un1<=un2){
		rightoverlap=1;
		if((un1-u2)<COLLISION_DISTANCE || (un2-un1)<COLLISION_DISTANCE){
			marge2=1;
		}
	}
	if(un2>=u1 && un2<=un1){
		rightoverlap=1;	
		if((un2-u1)<COLLISION_DISTANCE || (un1-un2)<COLLISION_DISTANCE){
			marge2=1;
		}
	}
	if(u2<=un1 && u2>=u1){
		rightoverlap=1;
		if((un1-u2)<COLLISION_DISTANCE || (u2-u1)<COLLISION_DISTANCE){
			marge2=1;
		}
	}
	if(u1<=un2 && u1>=u2){
		rightoverlap=1;
		if((un2-u1)<COLLISION_DISTANCE || (u1-u2)<COLLISION_DISTANCE){
			marge2=1;
		}
	}



	if(leftoverlap==1 && rightoverlap==1 && (marge1==0 || marge2==0)){
		return 1;
	}
	else{
		return 0;
	}

}

void collision_detection(Game * game, int * xbonus, int * ybonus, int * col1, int * col2){
	/*
	Returns 0 if no collision
	Returns 1 if the player collided and needs to go back (so collided with object and keeps standing still)
	Returns 2 if the player must die
	Returns 3 if player encountered a Powerup
	Returns 4,5,6... - ... if Enemy number 0,1,2,... collided
	*/

	int collisionnr=0;
	/*Player collision*/
	int direction = game->player.orientation;
	int x1=get_tile(game->player.x);
	int y1=get_tile(game->player.y);
	int x2 =-1,y2 =-1;

	*col1=collided(x1,y1,game,1,-1, xbonus, ybonus); 
	/*If the player is between tiles*/
	if((game->player.x % TILE_SIZE !=0 || game->player.y % TILE_SIZE != 0 ) ){
		if(game->player.x %TILE_SIZE!=0){
			x2=get_up_tile(game->player.x);
			y2=y1;
		}
		if(game->player.y % TILE_SIZE!=0){
			y2=get_up_tile(game->player.y);
			if(x2==-1){
				x2=x1;
			}
		}
		*col2=collided(x2,y2,game,1,-1,xbonus,ybonus);
	}

	if(*col2==0 && *col1 ==0 && x1!=-1 && y2!=-1){
		*col2 = collided(x1,y2,game,1,-1,xbonus,ybonus);
		if(*col2==0){
			*col2 = collided(x2,y1,game,1,-1,xbonus,ybonus);
		}
	}

}

void get_next_position(Game* game, int * x, int * y, int direction,int player,int boss){
	/*Returns the next position the player or enemy will have*/
	int i;
	int x_bomb=*x, y_bomb=*y;
	if(player==1){
		i=PLAYER_MOVEMENT_INCREMENT;
	}
	else{
		if(boss==0){
			i=ENEMY_MOVEMENT_INCREMENT;
		}
		else{
			i=BOSS_MOVEMENT_INCREMENT;
		}
	}

	switch (direction){
	case 0:*y-=i; break; //NORTH
	case 1:*x+=i; break; //EAST
	case 2:*y+=i; break; //SOUTH
	case 3:*x-=i; break; //WEST
	}

	if(player==1){
		switch(direction){
		case 0: y_bomb = *y-TILE_SIZE; break;
		case 1: x_bomb = *x+TILE_SIZE; break;
		case 2: y_bomb = *y+TILE_SIZE; break;
		case 3: x_bomb = *x-TILE_SIZE; break;
		}

		if( game->level.entities[y_bomb/TILE_SIZE][x_bomb/TILE_SIZE].type==BOMB){
			get_previous_position(x,y,direction,player,boss);
		}
	}
}

void get_previous_position(int * x, int * y, int direction,int player,int boss){
	/*Returns the next position the player or enemy will have*/
	int i;

	if(player==1){
		i=PLAYER_MOVEMENT_INCREMENT;
	}
	else{
		if(boss==0){
			i=ENEMY_MOVEMENT_INCREMENT;
		}
		else{
			i=BOSS_MOVEMENT_INCREMENT;
		}
	}

	switch (direction){
	case 0: *y+=i; break; //NORTH
	case 1: *x-=i; break; //EAST
	case 2: *y-=i; break; //SOUTH
	case 3: *x+=i; break; //WEST
	}

}

int get_tile(int position){
	return position - position % TILE_SIZE;
}

ORIENTATION get_direction(int dir){
	ORIENTATION o;
	switch(dir){
	case 0: o = NORTH; break;
	case 1: o = EAST; break;
	case 2: o = SOUTH; break;
	case 3: o = WEST; break;
	}

	return o;
}

void get_random_position(int * y, int * x, ORIENTATION * direction, Game * game){
	
	int number_of_empty_spaces = (int) floor((1-game->level.level_info.fill_ratio)*game->level.level_info.height*game->level.level_info.width);
	int random_x =rand() % game->level.level_info.height; //prevent out of bounds
	int random_y=rand()%game->level.level_info.width;
	int random=rand()%number_of_empty_spaces;
	int d= rand()%4;
	int stop=0;


	int sum= random_x+random_y;
	while(!stop){

	if(sum !=2 && sum!=3 && game->level.entities[random_x][random_y].type==EMPTY_SPACE){
	//!=2 and !=3 to prevent trapping in upper corner
	*x=random_x*TILE_SIZE;
	*y=random_y*TILE_SIZE;
	*direction=get_direction(d);
	stop=1;
	}
	else{
	 random_x =rand() % game->level.level_info.height; //prevent out of bounds
	 random_y=rand()%game->level.level_info.width;
	 sum=random_x+random_y;
	}
		}


}

void create_player(Game * game){
	game->player.current_bomb_power=INIT_BOMB_POWER; 
	game->player.is_dead=0;  //He is not dead
	game->player.orientation=SOUTH;  
	game->player.remaining_bombs=INIT_NUMBER_BOMBS;
	game->player.x=TILE_SIZE;  //Left upper corner
	game->player.y=TILE_SIZE;
}

void create_enemies(Game * game){
	int i;
	int boss = game->level.level_info.spawn_boss;

	/*Malloc memory for enemies*/
	game->enemies=(Enemy *) malloc(game->level.level_info.nr_of_enemies*sizeof(Enemy));
	if(game->enemies == NULL){
        printf("ERROR: Allocation failed, insufficient memory?\n");
        exit(0);}


	for(i=0;i<game->level.level_info.nr_of_enemies;i++){
		game->enemies[i].is_dead=0; //not dead
		game->enemies[i].frozen=0;


		if(boss){
			game->enemies[i].is_boss=1;
			boss=0;  //Only one boss
		}
		else{
			game->enemies[i].is_boss=0;
		}

		/*Get the position and direction of the enemy*/
		get_random_position(&game->enemies[i].x,&game->enemies[i].y,&game->enemies[i].move_direction,game);

		/*Change the number of lives*/
		if(game->enemies[i].is_boss){
			game->enemies[i].remaining_lives=3;  //Boss has three lives
		}
		else{
			game->enemies[i].remaining_lives=1;	//normal enemy has one life
		}

	}

	//Number of enemies hasn't changed
	game->enemies_left=game->level.level_info.nr_of_enemies;
}

int enough_bombs(Game * game){
	/*Returns 1 if bomb can be placed
	Returns 0 if bomb cannot be placed*/

	if(game->player.remaining_bombs>0){
		return 1;
	}
	else return 0;
}

void init_game(Game * game, int level_nr) 
{
	game->game_over=0;
	if(game->score<0){
		game->score=0;
	}
	game->level.level_info=generate_level_info(level_nr);
	init_level(&game->level,game->level.level_info);
	gui_set_enemies_left(game->level.level_info.nr_of_enemies);
	render_level(&game->level);
	create_player(game);
	create_enemies(game);
	gui_start_timer();

}

void do_game_loop(Game * game, int start_score)
{
	//TODO basic first code is given here. This code may, and should, be changed.
	Event ev;
	int stop=0;


	while(!game->game_over) {

		gui_game_event(&ev);

		if(ev.type==EVENT_TIMER){
			update_game(game);
			render_game(game);
			

		}


		if(ev.type==EVENT_KEY){ 

			check_game_input(game,ev.keyEvent.key);
			ev.keyEvent.key=0; //set to zero to prevent double input

		}



		/*If the player is dead -> GAME OVER*/
		if(game->player.is_dead==1){
			game->game_over=1;
			gui_set_game_over();
			render_game(game);
			
			break;
		}

		// If player all enemies dead and player alive -> level finished!
		if(game->enemies_left==0 && game->player.is_dead==0){
			ev.keyEvent.key=0;
			gui_set_finished_level(game->level.level_info.level_nr,game->score);
			gui_draw_buffer();
			
			break;
		}



	}
}

void wait_for_space() 
{
	Event ev;
	int key ;
	int toets=0;

	while(!toets){
		gui_game_event(&ev);
		key = ev.keyEvent.key;
		toets=compare(key,(int)pow(2.0,4));
	}
}

int compare(int key, int value){

	return key&value;

}

void check_game_input(Game * game, int key) 
{
	int toets,i;		

	if(key==0){
		game->input.hasMoved=1;
		return;
	}
	toets=compare(key,(int)pow(2.0,5));
	if(toets==(int)pow(2.0,5)){
		game->game_over=1;
		destroy_game(game);
		gui_clean();
	}


	else{	for(i=0;i<4;i++){
		if(i<4){ //set corrosponding element in array to 1 if key was pressed
			toets=compare(key,(int)pow(2.0,i));
			if(toets==(int)pow(2.0,i)) { 
				game->input.moves[i]=1;
				game->input.hasMoved=0;
			}
			else game->input.moves[i]=0;

		}
	}

	if(i==4){
		toets = compare(key,(int)pow(2.0,4));
		if(toets==16) { 
			game->input.dropBomb=1;

		}
	}}



}

void draw_enemies(Game * game){
	int i;
	for(i=0;i<game->level.level_info.nr_of_enemies;i++){   
		if(game->enemies[i].is_dead==0){
			gui_add_enemy(&game->enemies[i]);
		}
	}
}

void update_game(Game * game)
{
	//TODO this may be changed and/or extended
	int vlag1, vlag2; 
	int xbonus, ybonus;

	do_player_movement(game);
	do_enemy_ai(game);


	/*Collision detection*/
	collision_detection(game, &xbonus, &ybonus, &vlag1, &vlag2);

	/*Bonus items*/
	if(vlag1==3 || vlag2==3){
		process_bonus_items(game,xbonus,ybonus);
	//	printf("\n kracht : %d \n",game->player.current_bomb_power);
	}

	/*Gamer dead*/
	if(vlag1==2 || vlag2 == 2){
		game->player.is_dead=1;   //TO DO: opnieuw uncommenten
		//printf("\n PLAYER IS DEAD!!!!");
	}

	/*Set bombs*/
	do_bombs(game);

	render_map(&game->level);

	/*Draw player and enemies*/
	gui_add_player(&game->player);
	draw_enemies(game);

}

void render_game(Game * game){	
	int  vlag,i;



	for(ycoord=1;ycoord<game->level.level_info.height-1;ycoord++){ // walls don't have to be filed this is taken care of in movement
		for(xcoord=1;xcoord<game->level.level_info.width-1;xcoord++){ // walls don't have to be filed this is taken care of in movement


			/*draw powerups*/
			if(game->level.entities[ycoord][xcoord].type==POWERUP ){

				gui_add_powerup(&game->level.entities[ycoord][xcoord].powerup);

			}

			/*draw bombs*/
			if(game->level.entities[ycoord][xcoord].obstacle.type==BOMB ){

				process_bombs(game);

			}	

			/*explosions*/
			if(game->level.entities[ycoord][xcoord].obstacle.type==EXPLOSION ){

				do_explosion(game);
				vlag=collision_explosie(game,1,-1);
				if(vlag==2){ 
					game->player.is_dead=1;   //TO DO: opnieuw uncommenten
					// printf("\n PLAYER IS DEAD!!!!");
				}

				for(i=0;i<game->level.level_info.nr_of_enemies;i++){
					if(game->enemies[i].is_dead==0){
						vlag=collision_explosie(game,0,i);

						if(vlag==2 && same_explosion==0){
							same_explosion=1;
							/*If the enemy is a boss*/
							if(game->enemies[i].is_boss==1){
								if(game->enemies[i].remaining_lives>0){
									game->enemies[i].remaining_lives--;
									
								}

								if(game->enemies[i].remaining_lives==0){
									
									game->enemies[i].is_dead=1;
									game->enemies_left--;
									set_game_score_boss(game);
								}

							}
							/*if the enemy is not a boss*/
							else{
								
								game->enemies[i].is_dead=1;	
								game->enemies_left--;
								set_game_score_enemy(game);

							}
						}

					}
				}
			}



		}

	}
	gui_set_bombs_left(game->player.remaining_bombs);
	gui_set_enemies_left(game->enemies_left);
	gui_draw_buffer();
	
}

ORIENTATION determine_direction(Game * game){
	int i;
	int keys=0; 

	for(i=0;i<4;i++){
		if(game->input.moves[i]==1){
			keys++;
		}
	}

	if(keys>1){
		for(i=0;i<4;i++){
			ORIENTATION temp;
			if(i==0){
				temp = get_direction(0);
			}
			else if(i==1){
				temp =  get_direction(2);
			}
			else if(i==2){
				temp = get_direction(1);
			}
			else{
				temp = get_direction(3);
			}
			if(game->input.moves[i]==1 && temp!=game->player.orientation){
				return temp;
			}
		}
	}
	else{
		for(i=0;i<4;i++){
			if(game->input.moves[i]==1){
				if(i==0){
					return get_direction(0);
				}
				else if(i==1){
					return get_direction(2);
				}
				else if(i==2){
					return get_direction(1);
				}
				else{
					return get_direction(3);
				}
			}
		}
	}


	/*If failed-> just go north*/
	return NORTH;


}

void do_player_movement(Game * game)
{
	int vlag1=0, vlag2=0;
	int keys=0;
	int moved=0;
	int  xbonus=0;


		/*
		determine position -> see if next position is empty -> if not handle it accordingly
		
		*/

	/*NORTH*/
	if(game->input.moves[0]==1 && game->input.hasMoved==0 && moved ==0){  
	
		game->player.orientation = determine_direction(game);
		get_next_position(game,&game->player.x,&game->player.y,game->player.orientation,1,0);
		moved=1;
		collision_detection(game,&xbonus,&xbonus, &vlag1, &vlag2);
	}


	if ((vlag1==1 || vlag2==1) && moved==1){
		get_previous_position(&game->player.x,&game->player.y,game->player.orientation,1,0);
		vlag1=0;
		vlag2=0;
		moved=0;
	}


	/*SOUTH*/
	if(game->input.moves[1]==1 && game->input.hasMoved==0 && moved == 0){  
		moved=1;
		game->player.orientation=determine_direction(game);
		get_next_position(game,&game->player.x,&game->player.y,game->player.orientation,1,0);
		collision_detection(game,&xbonus,&xbonus, &vlag1, &vlag2);
	}


	if ((vlag1==1 || vlag2==1) && moved==1){
		get_previous_position(&game->player.x,&game->player.y,game->player.orientation,1,0);
		vlag1=0;
		vlag2=0;
		moved=0;
	}

	/*EAST*/
	if(game->input.moves[2]==1 && game->input.hasMoved==0 && moved==0){ 
		game->player.orientation=determine_direction(game);
		get_next_position(game,&game->player.x,&game->player.y,game->player.orientation,1,0);
		moved=1;
		collision_detection(game,&xbonus,&xbonus, &vlag1, &vlag2);
	}


	if ((vlag1==1 || vlag2==1) && moved==1){
		get_previous_position(&game->player.x,&game->player.y,game->player.orientation,1,0);
		vlag1=0;
		vlag2=0;
		moved=0;
	}

	/*WEST*/
	if(game->input.moves[3]==1 && game->input.hasMoved==0 && moved==0){  
		game->player.orientation=determine_direction(game);
		get_next_position(game,&game->player.x,&game->player.y,game->player.orientation,1,0);
		moved=1;
		collision_detection(game,&xbonus,&xbonus, &vlag1, &vlag2);
	}


	if ((vlag1==1 || vlag2==1) && moved==1){
		get_previous_position(&game->player.x,&game->player.y,game->player.orientation,1,0);
		vlag1=0;
		vlag2=0;
		moved=0;
	}

}

void do_enemy_ai(Game * game)
{
	int i;
	int frozen =0;



	for(i=0;i<game->level.level_info.nr_of_enemies;i++){	
		if(game->enemies[i].is_dead==0){
			if(game->enemies[i].frozen!=1){
				if(game->enemies[i].is_boss==0 ){
					enemy2[i]++;
					if(enemy2[i]==BOSS_MOVEMENT_INCREMENT){   //BOSS has to move twice as fast
						enemy2[i]=0;
						enemy_crosspoint_ai(game,i);
					}
				}
				else{
					enemy_crosspoint_ai(game,i);
				}
			}
			else{
				frozen=1;
			}
		}
	}

	if(frozen==1){
		freeze_enemies(game);
	}

}

void freeze_enemies(Game * game){
	int i;
	ticks++;

	if(ticks==FREEZE_DURATION){

		for(i=0;i<game->level.level_info.nr_of_enemies;i++){
			game->enemies[i].frozen=0;
		}

		ticks=0;
	}

}

void enemy_crosspoint_ai(Game * game, int i){
 
        /*The enemy who is at position enemies[i] will move as following:
        When he reaches a crosspoint he will choose a random direction and start to move in that direction*/
        ORIENTATION direction;
        int x1,x2;
        int y1,y2;
        int col;
        int xbonus=0;
        int d;
        int j;
 
        /*If enemy is at crossroad -> get random direction*/
        if(game->enemies[i].x % TILE_SIZE == 0 && game->enemies[i].y % TILE_SIZE == 0){
                d=rand() %4;
                direction = get_direction(d);
        }
        else{
                direction = game->enemies[i].move_direction;
        }
 
        for(j=0;j<game->player.current_bomb_power+1;j++){
                if(out_of_bounds(game->enemies[i].x/TILE_SIZE+j,game->enemies[i].y/TILE_SIZE,game)==0){
                        if(game->level.entities[game->enemies[i].y/TILE_SIZE][(game->enemies[i].x)/TILE_SIZE+j].type==BOMB || game->level.entities[game->enemies[i].y/TILE_SIZE][(game->enemies[i].x)/TILE_SIZE+j].type==EXPLOSION){  //if there is a bomb east
                                direction = WEST;
                        }
                }
                if(out_of_bounds(game->enemies[i].x/TILE_SIZE-j,game->enemies[i].y/TILE_SIZE,game)==0){
                        if(game->level.entities[game->enemies[i].y/TILE_SIZE][(game->enemies[i].x)/TILE_SIZE-j].type==BOMB ||game->level.entities[game->enemies[i].y/TILE_SIZE][(game->enemies[i].x)/TILE_SIZE-j].type==EXPLOSION){  //if there is a bomb west
                                direction = EAST;
                        }
                }
                if(out_of_bounds(game->enemies[i].x/TILE_SIZE, game->enemies[i].y/TILE_SIZE -j, game)==0 ){
                        if(game->level.entities[game->enemies[i].y/TILE_SIZE-j][game->enemies[i].x/TILE_SIZE].type==BOMB || game->level.entities[game->enemies[i].y/TILE_SIZE-j][game->enemies[i].x/TILE_SIZE].type==EXPLOSION ){ //if there is a bomb north
                                direction = SOUTH;
                        }
                }
                if(out_of_bounds(game->enemies[i].x/TILE_SIZE, game->enemies[i].y/TILE_SIZE +j, game) == 0 ){
                        if(game->level.entities[game->enemies[i].y/TILE_SIZE+j][game->enemies[i].x/TILE_SIZE].type==BOMB|| game->level.entities[game->enemies[i].y/TILE_SIZE+j][game->enemies[i].x/TILE_SIZE].type==EXPLOSION){ //if there is a bomb south
                                direction = NORTH;
                        }
                }
 
 
        }
 
 
 
        game->enemies[i].move_direction=direction;
        get_next_position(game,&game->enemies[i].x,&game->enemies[i].y,game->enemies[i].move_direction,0,game->enemies[i].is_boss);
 
        /*Collision detection*/
        x1=get_tile(game->enemies[i].x);
        y1=get_tile(game->enemies[i].y);
        x2 =-1,y2 =-1;
 
        col=collided(x1,y1,game,0,i,&xbonus,&xbonus);
 
        /*If the Enemy is between tiles*/
        if((game->enemies[i].x % TILE_SIZE !=0 || game->enemies[i].y % TILE_SIZE != 0 ) && col ==0){
                if(game->enemies[i].x %TILE_SIZE!=0){
                        x2=get_up_tile(game->enemies[i].x);
                        y2=y1;
                }
                if(game->enemies[i].y % TILE_SIZE!=0){
                        y2=get_up_tile(game->enemies[i].y);
                        if(x2==-1){
                                x2=x1;
                        }
                }
                col=collided(x2,y2,game,0,i,&xbonus,&xbonus);
        }
 
        if(col==0 && x1!=-1 && y2!=-1){
                col = collided(x1,y2,game,0,i,&xbonus,&xbonus);
                if(col==0){
                        col = collided(x2,y1,game,0,i,&xbonus,&xbonus);
                }
        }
 
        /*If the enemy collided*/
        if(col>=4){
                get_previous_position(&game->enemies[i].x,&game->enemies[i].y,game->enemies[i].move_direction,0,game->enemies[i].is_boss);
                game->enemies[i].move_direction = get_direction(rand() %4);
        }
 
}

void process_bonus_items(Game * game, int x , int y)
{
	int i;
	/*Extra Bomb*/
	if(game->level.entities[y/TILE_SIZE][x/TILE_SIZE].powerup.powerup_type==EXTRA_BOMB){
		game->player.remaining_bombs++;	
	} 
	/*Extra Power*/
	else if (game->level.entities[y/TILE_SIZE][x/TILE_SIZE].powerup.powerup_type==EXTRA_POWER){


		if(PLAYER_MAX_POWER>game->player.current_bomb_power) { 
			game->player.current_bomb_power++;
		}
		else {
			game->player.current_bomb_power=PLAYER_MAX_POWER;

		}
	}
	else{
		for(i=0;i<game->level.level_info.nr_of_enemies;i++){
			game->enemies[i].frozen=1;  
		}
	}

	/*Make empty space*/
	game->level.entities[y/TILE_SIZE][x/TILE_SIZE].type=EMPTY_SPACE;
	game->level.entities[y/TILE_SIZE][x/TILE_SIZE].empty_space.type=EMPTY_SPACE;
	game->level.entities[y/TILE_SIZE][x/TILE_SIZE].empty_space.x=x;
	game->level.entities[y/TILE_SIZE][x/TILE_SIZE].empty_space.y=y;

}

POWERUP_TYPE get_powerup(int powerup){
	POWERUP_TYPE p;

	switch(powerup){
	case 0: p=EXTRA_POWER; break;
	case 1: p=EXTRA_BOMB; break;
	case 2: p=FREEZE_ENEMIES; break;

	}
	return p;
}

int out_of_bounds(int xtile, int ytile, Game * game){
	if(xtile<0 || ytile< 0 || xtile>= game->level.level_info.width || ytile >= game->level.level_info.height){
		return 1;
	}
	else return 0;
}

void update_spread(Game * game){
	int power = game->level.entities[ycoord][xcoord].explosion.power;
	int i;

	/*
	
	
	
	
	*/


	/*LEFT*/
	for(i=xcoord;i>xcoord-power-1;i--){
		if(out_of_bounds(i,ycoord,game)==0){
			if(game->level.entities[ycoord][i].type==OBSTACLE ){
				if(game->level.entities[ycoord][i].obstacle.is_destructable==0){
					game->level.entities[ycoord][xcoord].explosion.spread[0]=xcoord-i-1;
					break;
				}
				else{
					game->level.entities[ycoord][xcoord].explosion.spread[0]=xcoord-i;
					break;
				}
			}
			else if(enemy_collision(game,i*TILE_SIZE,ycoord*TILE_SIZE,-1)==1){
				game->level.entities[ycoord][xcoord].explosion.spread[0]=xcoord-i;
				break;
			}
		}
	}

	/*RIGHT*/
	for(i=xcoord;i<xcoord+power+1;i++){
		if(out_of_bounds(i,ycoord,game)==0){
			if(game->level.entities[ycoord][i].type==OBSTACLE){
				if(game->level.entities[ycoord][i].obstacle.is_destructable==0){
					game->level.entities[ycoord][xcoord].explosion.spread[1]=i-xcoord-1;
					break;
				}
				else{
					game->level.entities[ycoord][xcoord].explosion.spread[1]=i-xcoord;
					break;
				}
			}
			else if(enemy_collision(game,i*TILE_SIZE,ycoord*TILE_SIZE,-1)==1){
				game->level.entities[ycoord][xcoord].explosion.spread[1]=i-xcoord;
				break;
			}
		}
	}

	/*DOWN*/
 	for(i=ycoord;i<ycoord+power+1;i++){
		if(out_of_bounds(xcoord,i,game)==0){
			if(game->level.entities[i][xcoord].type==OBSTACLE){
				if(game->level.entities[i][xcoord].obstacle.is_destructable==0){
					game->level.entities[ycoord][xcoord].explosion.spread[3]=i-ycoord-1;
					break;
				}
				else{
					game->level.entities[ycoord][xcoord].explosion.spread[3]=i-ycoord;
					break;
				}
			}
			else if(enemy_collision(game,xcoord*TILE_SIZE,i*TILE_SIZE,-1)==1){
				game->level.entities[ycoord][xcoord].explosion.spread[3]=i-ycoord;
				break;
			}
		}
	}

	/*UP*/
	for(i=ycoord;i>ycoord-power-1;i--){
		if(out_of_bounds(xcoord,i,game)==0){
			if(game->level.entities[i][xcoord].type==OBSTACLE){
				if(game->level.entities[i][xcoord].obstacle.is_destructable==0){
					game->level.entities[ycoord][xcoord].explosion.spread[2]=ycoord-i-1;
					break;
				}
				else{
					game->level.entities[ycoord][xcoord].explosion.spread[2]=ycoord-i;
					break;
				}
			}
			else if(enemy_collision(game,xcoord*TILE_SIZE,i*TILE_SIZE,-1)==1){
				game->level.entities[ycoord][xcoord].explosion.spread[2]=ycoord-i;
				break;
			}
		}
	}


}

void explode (Game* game){
	int i;
	int power = game->level.entities[ycoord][xcoord].explosion.power;
	int links,rechts,boven ,onder;


	links = game->level.entities[ycoord][xcoord].explosion.spread[0];
	rechts = game->level.entities[ycoord][xcoord].explosion.spread[1];
	boven = game->level.entities[ycoord][xcoord].explosion.spread[2];
	onder = game->level.entities[ycoord][xcoord].explosion.spread[3];



	gui_add_explosion_tile(xcoord*TILE_SIZE,ycoord*TILE_SIZE);

	/*destruct object and add explosion tiles in 4 directions according to spread */
	for (i=xcoord;i>xcoord-links-1;i--){   //LEFT

		destruct_object(game,i,ycoord);

		gui_add_explosion_tile(i*TILE_SIZE,ycoord*TILE_SIZE);

	}
	for (i=xcoord;i<xcoord+rechts+1;i++){  //RIGHT

		destruct_object(game,i,ycoord);

		gui_add_explosion_tile(i*TILE_SIZE,ycoord*TILE_SIZE);


	}
	for (i=ycoord;i<ycoord+onder+1;i++){  //UP

		destruct_object(game,xcoord,i);

		gui_add_explosion_tile(xcoord*TILE_SIZE,i*TILE_SIZE);

	}
	for (i=ycoord;i>ycoord-boven-1;i--){   //DOW+N

		destruct_object(game,xcoord,i);

		gui_add_explosion_tile(xcoord*TILE_SIZE,i*TILE_SIZE);

	}



}

void destruct_object(Game * game,int x, int y){

	if(game->level.entities[y][x].type==OBSTACLE){

		/*Destructable object*/
		if(game->level.entities[y][x].obstacle.is_destructable==1){
			/*if Powerup*/
			if(game->level.entities[y][x].obstacle.powerup==1){
				POWERUP_TYPE p = get_powerup(rand() %3);
				game->level.entities[y][x].type=POWERUP;
				game->level.entities[y][x].powerup.type=POWERUP;
				game->level.entities[y][x].powerup.powerup_type=p;

			}
			else{
				game->level.entities[y][x].type=EMPTY_SPACE;
			}
		}
		else{
			return;
		}
	}
	else if(game->level.entities[y][x].type==BOMB){
		game->level.entities[y][x].bomb.ticks_left=1;

	}
}

void do_explosion(Game* game){

	game->level.entities[ycoord][xcoord].explosion.ticks_left--;


	if(game->level.entities[ycoord][xcoord].explosion.ticks_left>0){
		explode(game);
	}
	else{
		game->player.remaining_bombs++;
		game->level.entities[ycoord][xcoord].type=EMPTY_SPACE;
	}
}

void create_explosion(Game* game){
	int i;
	game->level.entities[ycoord][xcoord].type=EXPLOSION;
	game->level.entities[ycoord][xcoord].explosion.x=xcoord;
	game->level.entities[ycoord][xcoord].explosion.y=ycoord;
	game->level.entities[ycoord][xcoord].explosion.ticks_left=EXPLOSION_TICKS;

	game->level.entities[ycoord][xcoord].explosion.power=game->player.current_bomb_power;

	for(i=0;i<4;i++){

		game->level.entities[ycoord][xcoord].explosion.spread[i]=game->level.entities[ycoord][xcoord].explosion.power; 

	}
	update_spread(game);

}

void process_bombs(Game * game){


	game->level.entities[ycoord][xcoord].bomb.ticks_left--;

	if(game->level.entities[ycoord][xcoord].bomb.ticks_left==0 ){

		create_explosion(game);
		
		do_explosion(game);

	}
	else {
		same_explosion=0;
		gui_add_bomb(&game->level.entities[ycoord][xcoord].bomb);

	}
}

void destroy_game(Game* game){
	free(game->enemies);
	destroy_level(&game->level);
	
}

void set_game_score_enemy(Game* game){
	int score= BASE_SCORE_ENEMY-gui_get_timer_score();

	if(score>0){
		game->score+=score;
	}

}

void set_game_score_boss(Game* game){

	game->score+=SCORE_BOSS;

}