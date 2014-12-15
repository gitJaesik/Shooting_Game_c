#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <time.h>
#include <windows.h>
#include "Console.h"


char Screen[HEIGHT][WIDTH];

#define PLAYER_SIZE			5
#define ENEMY_SIZE			3

char PlayerUnit[PLAYER_SIZE+1] = "-#^#-";
char EnemyUnit[ENEMY_SIZE+1] = "@-@";

struct PlayerInfo{
	int x, y;
	int LiveFlag;
};

struct EnemyInfo{
	int x, y;
	int LiveFlag;
};


struct PlayerInfo Player;
struct EnemyInfo Enemy[3][10];

void Draw();
void DrawPlayer();
void InitialObject();
void DrawEnemy();

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
	DrawEnemy();
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

void DrawEnemy()
{
	int p,q;
	int x,y;
	int i;

	for(p=0; p < 3; p++){
		for(q=0; q < 10; q++){
			x = Enemy[p][q].x - ENEMY_SIZE/2;
			y = Enemy[p][q].y;

			if( y < 0 || y >= HEIGHT){
				printf("enemy location error\n");
				return;
			}
			
			for(i=0; i < ENEMY_SIZE; i++){
				if( x >=0 && x < WIDTH-1) Screen[y][x] = EnemyUnit[i];
				x++;
			}

		}
	}
}

void InitialObject()
{
	// Player
	Player.x = 39;
	Player.y = 19;
	Player.LiveFlag = 1;

	// Enemy
	int EnemyInitX = 13;
	int EnemyInitY = 2;

	int i, j;
	for(i=0; i < 3; i++){
		for(j=0; j < 10; j++){
			Enemy[i][j].x = EnemyInitX + j*5;
			Enemy[i][j].y = EnemyInitY + i*1;
		}
	}
}