#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "level.h"
#include "gui.h"
#include "highscores.h"

#ifdef _WIN32
#include <crtdbg.h>
#endif

/*
(optional) parameters:
	- Number of the level to start in.
	- Random seed for level generation.
*/
int main(int argc, char *argv[]) 
{
	int total_score = 0;	
	Game game;
	int end_of_game = 0;

	/* Initialize level number */
	int level = argc >= 2 ? atoi(argv[1]) : 1;

	/* Initialize random seed */
	int seed = argc == 3 ? atoi(argv[2]) : (int)time(NULL);
	
	
#ifdef _WIN32
	//_CrtSetBreakAlloc(180);
#endif

	srand(seed);
	printf("Seed is %d...\n\n", seed);

	
	
	

	/*initialize the gui*/
	gui_initialize();


	/* START TEST*/

	level=1;
	
	/* Eind TEST*/
	while(!end_of_game) {
		/* Initialize the next game instance */
		init_game(&game, level);
		
		/* Start the actual game loop */
		do_game_loop(&game, total_score);
		total_score += game.score;

		/* Check if player cleared the level */
		if(!game.game_over)
			level++;
		else
			end_of_game = 1;

		/* Clean up */
		destroy_game(&game);

		/* Ask for user input */
		wait_for_space();
	}

	gui_clean();
	printf("\nYour total score is %d\n", total_score);
	
	/* highscores */
	load_highscores();
	check_highscore_entry(total_score);
	display_highscores();
	save_highscores();

#ifdef _WIN32
	getchar();getchar();
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}
