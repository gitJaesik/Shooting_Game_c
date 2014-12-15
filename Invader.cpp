#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <time.h>
#include <windows.h>
#include "Console.h"


char Screen[HEIGHT][WIDTH];

#define PLAYER_SIZE			5
char PlayerUnit[PLAYER_SIZE+1] = "-#^#-";

struct PlayerInfo{
	int x, y;
	int LiveFlag;
};

struct PlayerInfo Player;

void Draw();
void DrawPlayer();
void InitialObject();


void main(void){

	Initial();
	InitialObject();
	srand((unsigned int)time(NULL));

	while(1){
		Draw();
	}
}



void Draw()
{
	int i;
	for(i=0; i < HEIGHT; i++){
		memset( Screen[i], ' ', WIDTH);
		Screen[i][WIDTH-1] = NULL;
	}

	DrawPlayer();
	//DrawEnemy();
	//DrawShot();

	for(i=0; i < HEIGHT; i++){
		MoveCursor(0, i);
		printf( Screen[i] );
	}
}

void DrawPlayer()
{
	int x = Player.x - PLAYER_SIZE/2;
	int y = Player.y;

	if( y < 0 || y >= HEIGHT ) return;

	int i;
	for(i=0; i < PLAYER_SIZE; i++){
		if( x >= 0 && x < WIDTH-1 ) Screen[y][x] = PlayerUnit[i];

		x++;
	}
}

void InitialObject()
{
	Player.x = 39;
	Player.y = 19;
	Player.LiveFlag = 1;

}