#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <memory.h>
#include "Console.h"

char Screen[HEIGHT][WIDTH];

#define TRUE					1


// 플레이어 비행기 관련 
#define PLAYER_SIZE          5
char PlayerUnit[PLAYER_SIZE+1] = "-#^#-";

struct PlayerInfo{
	int x, y;
	int LiveFlag;
};

//링크드리스트로 플레이어 목숨 갯수 구현하기.
typedef struct PlayerInfoLinkedList{
	struct PlayerInfo PI;
	struct PlayerInfoLinkedList *pNext;
} PlayerLL;

PlayerLL *pHead = NULL;
PlayerLL *currentPlayer = NULL;
int PlayerCount;

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
void Draw();
void EnemyAction();
void PlayerAction();
void CreateShot(int Type, int x, int y);
void DrawShot();
void ShotAction();
void CheckCrush();
int CheckClear();

// 링크드리스트
void AddPlayerLife();
void DrawPlayerLife();
void ClearHeap();


void main()
{
	Initial();
	InitialObject();
	srand((unsigned int)time(NULL));

	while(1){
		EnemyAction();
		PlayerAction();
		ShotAction();

		Draw();

		CheckCrush();

		if( CheckClear() ) break;

		Sleep(70);
	}

	ClearHeap();
}

int CheckClear()
{
	if( Player.LiveFlag == 0 ){
		MoveCursor( 36, 12 );
		printf("게임 오버!!");
		Sleep(1000);

		return 1;
	}

	int i;
	for(i=0; i < MAX_ENEMY; i++){
		if( Enemy[i].LiveFlag ) return 0;
	}

	MoveCursor(35, 12);
	printf("적은 모두 격추!");
	Sleep(1000);

	return 1;
}

void CheckCrush()
{
	int i, j;
	for(i=0; i < MAX_SHOT; i++){
		if( Shot[i].UseFlag ){
			if( Shot[i].Type == P_SHOT ){
				for( j=0; j < MAX_ENEMY; j++){
					if( Enemy[j].LiveFlag ){
						if( Shot[i].x >= Enemy[j].x - ENEMY_SIZE / 2 &&
							Shot[i].x <= Enemy[j].x + ENEMY_SIZE / 2 &&
							Shot[i].y == Enemy[j].y ){
								Shot[i].UseFlag = 0;
								Enemy[j].LiveFlag = 0;
								break;
						}
					}
				}
			} else if( Shot[i].Type == E_SHOT ) {
				if( Shot[i].x >= Player.x - PLAYER_SIZE / 2 &&
					Shot[i].x <= Player.x + PLAYER_SIZE / 2 &&
					Shot[i].y == Player.y ){
						Shot[i].UseFlag = 0;
						Player.LiveFlag = 0;
				}
			}
		}
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

void InitialObject()
{
	int i, x, y;

	// for player intialize
	int j;

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


	// Player Part
	Player.x = 39;
	Player.y = 19;
	Player.LiveFlag = 1;

	// LinkedListPart
	for(j=0; j < 3; j++){
		AddPlayerLife();
	}

}

void AddPlayerLife(){

	PlayerLL *pTemp = (PlayerLL *)malloc( sizeof(PlayerLL) );
	pTemp->pNext = NULL;

	if( pHead == NULL) pHead = pTemp;
	else{
		PlayerLL *p = pHead;

		while( p->pNext != NULL)
			p = p->pNext;
		p->pNext = pTemp;
	}
}

void ClearHeap(){
	PlayerLL *p = pHead;

	while(p != NULL){
		PlayerLL *pTemp = p;
		p = p->pNext;
		free( pTemp );
	}

	pHead = NULL;

	printf("All heap is deleted");
}

void GetPlayerLife(){
	PlayerLL *p = pHead;
	PlayerCount = 0;

	while(p != NULL){
		PlayerCount++;
		p = p->pNext;
	}

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
	DrawPlayerLife();

	for(i=0;i < HEIGHT;i++){
		MoveCursor(0, i);
		printf(Screen[i]);
	}
}

void ShotAction()
{
	int i;
	for(i=0;i < MAX_SHOT;i++){
		if( Shot[i].UseFlag ){
			if( Shot[i].Type == P_SHOT ){
				Shot[i].y--;
				if( Shot[i].y < 0 ) Shot[i].UseFlag = 0;
			} else if( Shot[i].Type == E_SHOT ){
				Shot[i].y++;
				if( Shot[i].y >= HEIGHT ) Shot[i].UseFlag = 0;
			}
		}
	}
}

void DrawShot()
{
	int i;
	for(i=0;i < MAX_SHOT;i++){
		if( Shot[i].UseFlag ){
			if( Shot[i].Type == P_SHOT ) Screen[ Shot[i].y ][ Shot[i].x ] = PlayerShot;
			else if( Shot[i].Type == E_SHOT ) Screen[ Shot[i].y ][ Shot[i].x ] = EnemyShot;
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
	int i;
	int x = Player.x - PLAYER_SIZE / 2;
	int y = Player.y;

	if (y < 0 || y >= HEIGHT) return;

	for(i=0;i < PLAYER_SIZE; i++){
		if(x >= 0 && x < WIDTH ) Screen[y][x] = PlayerUnit[i];
		x++;
	}
}

void DrawPlayerLife()
{
	GetPlayerLife();

	// strcat으로 -#^#- 을 -#^#- -#^#- 여러대 그릴 수 있지만
	// 화면의 크기로 인하여 -#^#- x 3의 방법으로 만든다.

	int i;
	int x=50;

	for(i=0;i < PLAYER_SIZE; i++){
		Screen[23][x] = PlayerUnit[i];
		x++;
	}
	Screen[23][x] = 'x';
	x++;
	// 숫자를 문자로 출력하기위해서 '0'의 아스키코드를 구하고 PlayerCount와 더한다.
	Screen[23][x] = '0' + PlayerCount;
}
/*
TODO
1. HP
 (1) LiveFlag를 숫자로 바꾸고 LiveFlag를 줄이는 식으로 한다.
     적의 HP는 2, 플레이어의 HP는 3이다.
 (2) Linked List를 사용해서 비행기 갯수를 조절한다.

2. SCORE
3. Next Stage
4. Item (적어도 3개 이상)
5. Boss

2 인용게임 만들기
소리?

*/