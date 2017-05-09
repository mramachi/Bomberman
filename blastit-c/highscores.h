#ifndef H_HIGHSCORE_TABLE
#define H_HIGHSCORE_TABLE

#define HIGHSCORE_FILE "highscores.dat"
#define MAX_HIGHSCORE_ENTRIES 5

void load_highscores();

void enter_highscore(int score);

void check_highscore_entry(int score);

void display_highscores();

char ** get_highscores(int * number);

void get_scores();

void save_highscores();

#endif