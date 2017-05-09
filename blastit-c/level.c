
# define MIN_HEIGHT 10
#define MIN_WIDTH 10
#define MIN_FILL 0.2
#define MIN_nr_enemies 2
#define NUMBER_OF_BOSSES 10
#define MAX_NUMBER_OF_LEVELS 100


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "level.h"
#include "gui.h"
#include "blast_it.h"
#include "game.h"



void define_realm(int level_nr,LevelInfo* level_info){
	if(level_nr<20){
		level_info->realm=CLASSIC;
	}
	else if (level_nr<40){
		level_info->realm=ICE;
	}
	else if (level_nr<60){
		level_info->realm=WAREHOUSE;
	}

	else if (level_nr<80){
		level_info->realm=DESERT;
	}
	else level_info->realm=PORTAL;



}

void obastacles_invoegen(Level* level, LevelInfo* level_info){

	int random_x,random_y,aantal,number_of_powerups;

	aantal=level->level_info.fill_ratio*level->level_info.height*level->level_info.width; //aantal objecten
	number_of_powerups=level->level_info.bonus_spawn_ratio*aantal;

	while(aantal>0){

		random_x = rand()%level_info->width;
		random_y = rand()%level_info->height;


		if(random_x%2!=0 || random_y%2!=0){
			if(level->entities[random_y][random_x].type==EMPTY_SPACE && ((random_x+random_y)!=2) && ((random_x+random_y)!=3)){ //Upper left corner no obstacles so player can move
				aantal--;
				level->entities[random_y][random_x].type=OBSTACLE;
				level->entities[random_y][random_x].obstacle.is_destructable=1;
				level->entities[random_y][random_x].obstacle.type=OBSTACLE;
				level->entities[random_y][random_x].obstacle.x=random_x*TILE_SIZE;
				level->entities[random_y][random_x].obstacle.y=random_y*TILE_SIZE;

				if(number_of_powerups>0){
					level->entities[random_y][random_x].obstacle.powerup=1;
				}
				else{
					level->entities[random_y][random_x].obstacle.powerup=0;
				}

			}

		}
	}
}

int is_bonus_found(Level * level, LevelInfo * level_info){
	return 0;
}

void hindernissen_invoegen (Level* level, LevelInfo* level_info){
	int i;
	int j;
	int aantal_blokken= 2*level_info->height+2*level_info->width - 4 + level_info->height/2 + level_info->width/2; // WRS ANDER AANTAL EERSTE GOK VOOR TESTEN



	for(i=0;i<level_info->height;i++){
		for(j=0;j<level_info->width;j++){




			//maas maken
			if(j%2==0 && i%2==0 ){ 
				level->entities[i][j].type=OBSTACLE;
				level->entities[i][j].obstacle.type=OBSTACLE;
				level->entities[i][j].obstacle.is_destructable=0;
				level->entities[i][j].obstacle.x=j*TILE_SIZE;
				level->entities[i][j].obstacle.y=i*TILE_SIZE;
			}


			//randen volledig obstakels maken zodat ze er niet doorkunenn

			else if ((i == 0) || (i == level->level_info.height-1) || (j == 0) || (j == level->level_info.width-1)) {
				level->entities[i][j].type=OBSTACLE;
				level->entities[i][j].obstacle.type=OBSTACLE;
				level->entities[i][j].obstacle.is_destructable=0;
				level->entities[i][j].obstacle.x=j*TILE_SIZE;
				level->entities[i][j].obstacle.y=i*TILE_SIZE;

			}


			else{  //Empty space aanvullen

				level->entities[i][j].type=EMPTY_SPACE;
				level->entities[i][j].empty_space.type=EMPTY_SPACE;
				level->entities[i][j].empty_space.x=j*TILE_SIZE;
				level->entities[i][j].empty_space.y=i*TILE_SIZE;

			}



		}	
	}




}

LevelInfo generate_level_info(int level_nr)
{

	LevelInfo level_info;
	int deler;
	level_info.level_nr=level_nr;

	/*We doen maximum 100 levels*/
	level_info.width=(int) MIN_WIDTH+floor((double)level_nr/10)*8+1; //aanpassen van size van veld   REKENING HOUDEN MET TITLESIZE?
	level_info.height=MIN_HEIGHT+floor((double)level_nr/10)*6+1; //AANGEPAST NAAR 6
	define_realm(level_nr,&level_info);
	level_info.nr_of_enemies=(int) MIN_nr_enemies+floor((double)level_nr/12); //beginnen bij 2, +1 per ~10 level, eindigen op 10
	
	level_info.fill_ratio=MIN_FILL+floor((double)level_nr/10)*0.07; //beginnen bij MIN_FILL en per ~10 level 0,07 er bij
	deler=(int) floor((double) MAX_NUMBER_OF_LEVELS / NUMBER_OF_BOSSES);

	if(level_nr%deler==0){
		level_info.spawn_boss=1;
	}
	else level_info.spawn_boss=0;

	level_info.bonus_spawn_ratio=1-level_info.fill_ratio; //kan negatief worden maar is niet erg

	return level_info;
}

void init_level(Level * level, LevelInfo level_info)
{
	int i;

	level->level_info=level_info;
	

	//aanmaken matrix
	level->entities =(Entity**) malloc(level_info.height*sizeof(Entity*));
	if(level->entities == NULL){
        printf("ERROR: Allocation failed, insufficient memory?\n");
        exit(0);}

	for(i=0;i<level_info.height;i++){
		level->entities[i]=(Entity*) malloc(level_info.width*sizeof(Entity));
	
	if(level->entities[i] == NULL){
        printf("ERROR: Allocation failed, insufficient memory?\n");
        exit(0);}

	
	
	}

	//zijkanten vullen met indestructable objecten
	hindernissen_invoegen(level, & level_info);

	//obstacles invoegen met destructable objecten
	obastacles_invoegen(level,&level_info);
}

void render_map(Level* level){

	int total_score = 0;	

	for(ycoord=1;ycoord<level->level_info.height;ycoord++){  //randen worden niet getekend want die staan er standaard al
		for(xcoord=1;xcoord<level->level_info.width;xcoord++){

			if(level->entities[ycoord][xcoord].obstacle.type==OBSTACLE ){

				if(xcoord+ycoord!=3){
					if(xcoord+ycoord!=2){ //linkerbovenhoek vrijhouden zodat mannetje kan bewegen
						gui_add_obstacle(&level->entities[ycoord][xcoord].obstacle);

					}
				}
			}


		}

	}
}

void render_level(Level * level){
	int total_score = 0;	

	
	gui_set_level_info(&level->level_info);
	gui_draw_buffer();


}

void destroy_level(Level * level)
{
	int i;
	for(i=0;i<level->level_info.height;i++){
		free(level->entities[i]);
	}
	free(level->entities);
}


