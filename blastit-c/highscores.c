#include "highscores.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char ** highscores;
int * scores;
int number_of_highscores;

unsigned long get_file_length(FILE * file);
void allocation_error(char * c);
char * int_to_string(int score);

char ** get_highscores(int * number){
	*number=number_of_highscores;
	return highscores;
}

void allocation_error(char * c){
	printf("\nERROR: Allocation %s failed, insufficient memory?\n",c);
	exit(EXIT_FAILURE);
}

void load_highscores(){

	/*De file gaat in deze vorm zijn:
	Van hoge naar lage score:
	score - naam - datum - tijd (telkens met een tab tussen)
	dan tussen de highscores is het telkens een nieuwe lijn.*/

	unsigned long filelength,i;
	FILE * file = fopen(HIGHSCORE_FILE,"rb");
	char * text;
	int j=0;
	int length=1;
	number_of_highscores=0;

	/*See if file opened*/
	if(!file){
		printf("\nERROR: File couldn't open");
		exit(EXIT_FAILURE);
	}

	/*Get filelength*/
	filelength=get_file_length(file);

	/*Allocate memory for text*/
	text = (char *) malloc(sizeof(char)*(filelength+1));
	/*See if allocation worked*/
	if(text==NULL){
		allocation_error("text");
	}

	/*Allocate memory for highscores*/
	highscores=(char **) malloc(1*sizeof(char *));
	/*See if allocation worked*/
	if(highscores==NULL){
		allocation_error("highscores");
	}
	highscores[0]=(char *) malloc(1*sizeof(char));
	/*See if allocation worked*/
	if(highscores[0]==NULL){
		allocation_error("highscores[0]");
	}

	/*Read from file*/
	fread(text,1,filelength,file);

	/*Terminate text*/
	text[filelength]='\0';

	/*Put text in array*/
	for(i=0;i<filelength;i++){
		if(text[i]!='\0'){
			if(text[i]!='\n'){
				/*Realloc highscores*/
				highscores= (char **) realloc(highscores,sizeof(char *)*(j+2));  //amounts of highscores +1
				/*See if allocation worked*/
				if(highscores==NULL){
					allocation_error("highscores realloc");
				}

				/*Realloc highscores[j]*/
				highscores[j]=(char *) realloc(highscores[j],sizeof(char)*length);
				/*See if allocation worked*/
				if(highscores[j]==NULL){
					allocation_error("highscores realloc");
				}


				highscores[j][length-1]=text[i];
				length++;
			}
			else{
				highscores[j][length-1]='\n';
				/*Terminate string*/
				highscores[j][length]='\0';
				j++; //new row
				length=1; //length back to normal

				/*malloc highscores[j]*/
				highscores[j]=(char *) malloc(1*sizeof(char));;
				/*See if allocation worked*/
				if(highscores[j]==NULL)
					allocation_error("highscores[j]");
			}
		}
		else{
			j++;
			/*Terminate*/
			highscores[j]='\0';
			break;
		}
	}
	if(text[0]=='\0'){
		number_of_highscores=0;
	}
	else{
		number_of_highscores=j;
	}

	/*free text*/
	free(text);

	/*Close the file*/
	fclose(file);



}

unsigned long get_file_length(FILE * file){
	unsigned long filelength;

	/*Seek end of file*/
	fseek(file,0,SEEK_END);

	/*Get current filepointer*/
	filelength=ftell(file);

	/*Seek back beginning of file*/
	fseek(file,0,SEEK_SET);

	return filelength;
}

void get_scores(){
	int i,j=0,k;

	if(number_of_highscores!=0){
		/*Allocate memory for scores*/
		scores=(int *) malloc(sizeof(int)*number_of_highscores);
		/*See if allocation worked*/
		if(scores==NULL)
			allocation_error("Scores");


		for(i=0;i<number_of_highscores;i++){
			char * score;

			while(highscores[i][j]!='\t'){
				j++;
			}

			/*reallocate memory for score*/
			score = (char *) malloc((j+1)*sizeof(char));
			/*See if allocation worked*/
			if(score==NULL)
				allocation_error("score");

			/*Copy to score*/
			for(k=0;k<j;k++){
				score[k]=highscores[i][k];
			}

			/*Terminate scores*/
			score[j]='\0';

			/*Score to int*/
			scores[i]=atoi(score);

			free(score);
		}
	}

}

void check_highscore_entry(int score)
{
	int i;

	get_scores();

	if(number_of_highscores<MAX_HIGHSCORE_ENTRIES){
		enter_highscore(score);
		return;
	}
	else{
		for(i=0;i<number_of_highscores;i++){
			if(score>scores[i]){
				enter_highscore(score);
				return;
			}
		}
	}


}

char * get_highscore(int score){
	char * timestring;
	char * new_score;
	char * name;
	char * new_highscore;
	time_t current_time;
	int length;


	/*Get time*/
	current_time = time(NULL);
	if (current_time == ((time_t)-1)){
		printf("\nFailure to compute the current time.");
		exit(EXIT_FAILURE);
	}

	/*To string*/
	timestring = ctime(&current_time);
	/*See if it worked*/
	if (timestring == NULL){
		printf("\nFailure to convert the current time.");
		exit(EXIT_FAILURE);
	}


	/*Change number of highscores*/
	if(number_of_highscores<MAX_HIGHSCORE_ENTRIES){
		number_of_highscores++;
	}

	/*Enter highscore*/
	name=(char *) malloc(sizeof(char)*(20));  //a first name of more than 20 characters is just ridiculous
	/*See if allocation worked*/
	if(name==NULL)
		allocation_error("name");
	printf("\nEnter your first name: ");
	scanf("%s",name);

	/*Allocate memory for new score*/
	new_score = (char *) malloc(sizeof(char)*sizeof(score));
	/*See if allocation worked*/
	if(new_score==NULL){
		allocation_error("new score");
	}

	/*Get score*/
	new_score=int_to_string(score);

	/*Allocate memory for new highscore*/
	length=strlen(name)+strlen(timestring)+strlen(new_score)+2;
	new_highscore = (char *) malloc(sizeof(char)*(length)); //2 is for 2 tabs
	/*See if allocation worked*/
	if(new_highscore==NULL){
		allocation_error("new highscore");
	}

	
	
	strcpy(new_highscore,new_score);
	strcat(new_highscore,"\t");
	strcat(new_highscore,name);
	strcat(new_highscore,"\t");
	strcat(new_highscore,timestring);

	free(new_score);
	free(name);
	free(timestring);
	

	return new_highscore;
}

char * int_to_string(int score){
	char * buffer= (char *) malloc(sizeof(char)*100000);

	_itoa(score, buffer,10);
	return buffer;
}

void enter_highscore(int score){
	int i,j;
	char * temp_highscore;
	char * temp;
	char * new_highscore;



	for(i=0;i<number_of_highscores;i++){
		/*See where score fits*/
		if(scores[i]<score){

			new_highscore=get_highscore(score);

			/*Allocate memory for temp_highscore*/
			temp_highscore = (char *) malloc(sizeof(char)*strlen(new_highscore));
			/*See if allocation worked*/
			if(temp_highscore==NULL)
				allocation_error("Temp_highscore");

			/*Allocate memory for temp*/
			temp = (char *) malloc(sizeof(char)*strlen(new_highscore));
			/*See if allocation worked*/
			if(temp==NULL){
				allocation_error("temp");
			}

			strcpy(temp_highscore,highscores[i]);
			strcpy(highscores[i],new_highscore);

			/*Make sure the rest is in place*/
			if(i+1<number_of_highscores){
				for(j=i+1;j<number_of_highscores;j++){
					/*realloc temp*/
					temp =(char *) realloc(temp,sizeof(char)*strlen(highscores[j]));
					/*See if allocation worked*/
					if(temp==NULL)
						allocation_error("temp");
					if(j==number_of_highscores-1){
						highscores[j]=(char * ) malloc(sizeof(char)*strlen(temp_highscore));
						/*See if allocation worked*/
						if(highscores[j]==NULL)
							allocation_error("highscores[j]");
					}
					strcpy(temp,highscores[j]);
					strcpy(highscores[j],temp_highscore);

					/*realloc temp_highscore*/
					temp_highscore=(char *) realloc(temp_highscore,sizeof(char)*strlen(temp));
					/*see if allocation worked*/
					if(temp_highscore==NULL){
						allocation_error("temp_highscore");
					}
					strcpy(temp_highscore,temp);

					free(temp);
				}
				break;

			}
			else{
				break;
			}
		}
		else if(scores[i]>=score && i==number_of_highscores-1){
			temp_highscore=get_highscore(score);
			highscores[i+1]=(char * ) malloc(sizeof(char)*strlen(temp_highscore));
			/*See if allocation worked*/
			if(highscores[i+1]==NULL)
				allocation_error("highscores[j]");
			highscores[i+1]=temp_highscore;
			break;

			free(temp_highscore);
		}
	}

	if(number_of_highscores==0){
		new_highscore=get_highscore(score);
		highscores[0]=new_highscore;
	}

}

void display_highscores()
{
	int i;

	printf("\tHIGHSCORES: \t");
	for(i=0;i<number_of_highscores;i++){
		printf("\n %d: %s %s",i+1,"\t",highscores[i]);
	}

}

void save_highscores()
{

	char * points;
	int length=0,i;

	/*open file*/
	FILE * file = fopen(HIGHSCORE_FILE,"wb");	
	/*See if file opened*/
	if(!file){
		printf("\nERROR: file couldn't open");
		exit(EXIT_FAILURE);
	}

	/*Make one char * */
	for(i=0;i<number_of_highscores;i++){
		length+=strlen(highscores[i]);
	}
	points = (char * ) malloc(sizeof(char)*(length));
	/*See if allocation worked*/
	if(points==NULL)
		allocation_error("points");

	strcpy(points,highscores[0]);

	for(i=1;i<number_of_highscores;i++){
		strcat(points,highscores[i]);
	}

	

	/*Write to file*/
	if(fputs(points,file)==EOF){
		printf("\nERROR: couldn't write to file");
		exit(EXIT_FAILURE);
	}

	/*Close file*/
	fclose(file);

	/*free global variables*/
	free(scores);
	free(highscores);
}

