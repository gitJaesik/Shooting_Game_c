#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <memory.h>
#include "Console.h"

char Screen[HEIGHT][WIDTH];

// 플레이어 비행기 관련 
#define PLAYER_SIZE          5
char PlayerUnit[PLAYER_SIZE+1] = "-#^#-";

struct PlayerInfo{
	int x, y;
	int LiveFlag;
};

struct PlayerInfo      Player;

// 적 비행기 관련 
#define ENEMY_SIZE         3
char EnemyUnit[ENEMY_SIZE+1] = "@-@";

struct EnemyInfo{
	int x, y;   
	int LiveFlag;
	int MoveFlag;
	int StartX;
};

#define MAX_ENEMY      30
struct EnemyInfo      Enemy[MAX_ENEMY];

// 총알 관련

#define P_SHOT         0
#define E_SHOT         1

#define SHOT_SIZE		1
char PlayerShot = '|';
char EnemyShot = '*';

struct ShotInfo{
	int x, y;
	int Type;
	int UseFlag;
};

#define MAX_SHOT      20
struct ShotInfo         Shot[MAX_SHOT];



// 함수
void InitialObject();
void DrawPlayer();
void DrawEnemy();
void DrawShot();
void Draw();
void EnemyAction();
void PlayerAction();
void ShotAction();
void CreateShot(int Type, int x, int y);
void CheckCrash();


void main()
{
	Initial();
	InitialObject();
	srand((unsigned int)time(NULL));

	while(1){
		EnemyAction();
		PlayerAction();
		ShotAction();
		CheckCrash();

		Draw();

		Sleep(100);
	}
}

void CreateShot(int Type, int x, int y)
{
	int i;
	for( i=0; i < MAX_SHOT; i++){
		if( Shot[i].UseFlag == 0 ){
			Shot[i].UseFlag = 1;
			Shot[i].x = x;
			Shot[i].y = y;
			Shot[i].Type = Type;

			return;
		}
	}
}

void PlayerAction()
{
	if( GetAsyncKeyState( VK_UP ) & 0x8000 ){
		Player.y--;
		if( Player.y < 0 ) Player.y = 0;
	}
	if( GetAsyncKeyState( VK_DOWN ) & 0x8000 ){
		Player.y++;
		if( Player.y >= HEIGHT ) Player.y = HEIGHT-1;
	}
	if( GetAsyncKeyState( VK_LEFT ) & 0x8000 ){
		Player.x -= 2;
		if( Player.x < PLAYER_SIZE / 2 ) Player.x = PLAYER_SIZE / 2;
	}
	if( GetAsyncKeyState( VK_RIGHT ) & 0x8000 ){
		Player.x += 2;
		if( Player.x >= WIDTH - 1 - PLAYER_SIZE / 2 ) Player.x = WIDTH - 2 - PLAYER_SIZE / 2;
	}
	if( GetAsyncKeyState( VK_CONTROL ) & 0x8000 ) CreateShot( P_SHOT, Player.x, Player.y );
}

void EnemyAction()
{
	int i;
	for(i=0;i < MAX_ENEMY;i++){
		if(Enemy[i].LiveFlag){
			if( Enemy[i].MoveFlag ) Enemy[i].x++;
			else Enemy[i].x--;

			if( abs(Enemy[i].StartX - Enemy[i].x) > 13 ){
				Enemy[i].y++;
				Enemy[i].MoveFlag = !Enemy[i].MoveFlag;
			}

			if( rand() % 100 < 2 ) CreateShot( E_SHOT, Enemy[i].x, Enemy[i].y );
		}
	}
}

void ShotAction()
{
	int i;
	for(i=0;i < MAX_SHOT;i++){
		if(Shot[i].UseFlag){

			if(Shot[i].Type == E_SHOT){
				if(Shot[i].y >= HEIGHT) Shot[i].UseFlag = 0;
				else Shot[i].y++;
			}else{
				if(Shot[i].y <= 0) Shot[i].UseFlag = 0;
				else Shot[i].y--;
			}
		}
	}
}

void InitialObject()
{
	int i, x, y;

	x = 17, y = 1;
	for(i=0;i < MAX_ENEMY;i++){
		if(0 == i % 10){
			x = 17;
			y++;
		}
		Enemy[i].x = x;
		Enemy[i].y = y;
		Enemy[i].LiveFlag = 1;
		Enemy[i].MoveFlag = 1;
		Enemy[i].StartX = x;

		x += 5;
	}

	Player.x = 39;
	Player.y = 19;
	Player.LiveFlag = 1;
}

void Draw()
{
	int i;
	for(i=0;i < HEIGHT;i++){
		memset(Screen[i],' ', WIDTH);
		Screen[i][WIDTH - 1] = NULL;
	}

	DrawPlayer();
	DrawEnemy();
	DrawShot();

	for(i=0;i < HEIGHT;i++){
		MoveCursor(0, i);
		printf(Screen[i]);
	}
}

void DrawShot()
{
	int x, y, i, j;
	for(i=0; i < MAX_SHOT; i++){

		x = Shot[i].x - SHOT_SIZE / 2;
		y = Shot[i].y;

		if(y < 0 || y >= HEIGHT) continue;

		for(j=0; j < SHOT_SIZE ;j++){
			if(x >=0 && x < WIDTH){
				if(Shot[i].Type == P_SHOT){
					Screen[y][x] = PlayerShot;
				}else{
					Screen[y][x] = EnemyShot;
			 }
		 }
			x++;
		}
	}
}

void DrawEnemy()
{
	int x, y, i, j;
	for(i=0;i < MAX_ENEMY;i++){
		if(Enemy[i].LiveFlag){
			x = Enemy[i].x - ENEMY_SIZE / 2;
			y = Enemy[i].y;

			if(y < 0 || y >= HEIGHT) continue;

			for(j=0;j < ENEMY_SIZE;j++){
				if(x >=0 && x < WIDTH) Screen[y][x] = EnemyUnit[j];
				x++;
			}
		}
	}
}

void DrawPlayer()
{
	//if(Player.LiveFlag == 1){
	int i;
	int x = Player.x - PLAYER_SIZE / 2;
	int y = Player.y;

	if (y < 0 || y >= HEIGHT) return;

	for(i=0;i < PLAYER_SIZE; i++){
		if(x >= 0 && x < WIDTH ) Screen[y][x] = PlayerUnit[i];
		x++;
	}
	//}
}

// CheckCrash
void CheckCrash(){
	int i, j, k, l;
	// Check between EnemyShot & Player
	for(i=0; i < MAX_SHOT; i++){
		if(Shot[i].UseFlag){
			if(Shot[i].Type == E_SHOT && Shot[i].y == Player.y && abs(Shot[i].x -Player.x) <= PLAYER_SIZE / 2){
				Player.LiveFlag = 0;
				Shot[i].UseFlag = 0;

				MoveCursor(0, 24);
				printf("End of Game");
				// Printf Game over
			}
		}
	}
	// Check between PlayerShot & Enemy
	for(j=0; j < MAX_SHOT; j++){
		for(k=0; k < MAX_ENEMY; k++){
			if(Shot[j].UseFlag && Enemy[k].LiveFlag){
				if(Shot[j].Type == P_SHOT && Shot[j].y == Enemy[k].y && abs(Shot[j].x - Enemy[k].x) <= ENEMY_SIZE / 2){
					Enemy[k].LiveFlag = 0;
					Shot[j].UseFlag = 0;
					break;
				}
			}
		}
	}
	// Check between Player & Enemy
	for(l=0; l < MAX_ENEMY; l++){
		if(Enemy[l].x == Player.x && Enemy[l].y == Player.y){
			Player.LiveFlag = 0;

			MoveCursor(0, 24);
			printf("End of Game");
		}
	}
}