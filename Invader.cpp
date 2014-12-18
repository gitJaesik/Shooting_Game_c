#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <memory.h>
#include "Console.h"

char Screen[HEIGHT][WIDTH];
char HpScoreLifeItem[WIDTH];

#define TRUE					1


// 플레이어 비행기 관련 
#define PLAYER_SIZE          5
char PlayerUnit[PLAYER_SIZE+1] = "-#^#-";

struct ObjectInfo{
	int x, y;
	int LiveFlag;
	int HP;
};

//링크드리스트로 플레이어 목숨 갯수 구현하기.
typedef struct PlayerInfoLinkedList{
	struct ObjectInfo PI;
	struct PlayerInfoLinkedList *pNext;
} PlayerLL;

PlayerLL *pHead = NULL;
int PlayerCount;


// 적 비행기 관련 
#define ENEMY_SIZE         3
char EnemyUnit[ENEMY_SIZE+1] = "@-@";

struct EnemyInfo{
	ObjectInfo EI;
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
void NewStart();
void DrawPlayerHP();

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
	if( pHead->PI.LiveFlag == 0){
		if( pHead->pNext == NULL){
			MoveCursor( 36, 12 );
			printf("게임 오버!!");
			Sleep(1000);
			return 1;
		} else {
			PlayerLL *pTemp = pHead;
			pHead = pHead->pNext;
			free(pTemp);
			NewStart();
		}
	}

	int i;
	for(i=0; i < MAX_ENEMY; i++){
		if( Enemy[i].EI.LiveFlag ) return 0;
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
					if( Enemy[j].EI.LiveFlag ){
						if( Shot[i].x >= Enemy[j].EI.x - ENEMY_SIZE / 2 &&
							Shot[i].x <= Enemy[j].EI.x + ENEMY_SIZE / 2 &&
							Shot[i].y == Enemy[j].EI.y ){
								Shot[i].UseFlag = 0;
								Enemy[j].EI.LiveFlag = 0;
								break;
						}
					}
				}
			} else if( Shot[i].Type == E_SHOT ) {
				if( Shot[i].x >= pHead->PI.x - PLAYER_SIZE / 2 &&
					Shot[i].x <= pHead->PI.x + PLAYER_SIZE / 2 &&
					Shot[i].y == pHead->PI.y ){
						Shot[i].UseFlag = 0;
						pHead->PI.HP--;
						if(pHead->PI.HP == 0){
							pHead->PI.LiveFlag = 0;
						}

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
		pHead->PI.y--;
		if( pHead->PI.y < 0 ) pHead->PI.y = 0;
	}
	if( GetAsyncKeyState( VK_DOWN ) & 0x8000 ){
		pHead->PI.y++;
		if( pHead->PI.y >= HEIGHT ) pHead->PI.y = HEIGHT-1;
	}
	if( GetAsyncKeyState( VK_LEFT ) & 0x8000 ){
		pHead->PI.x -= 2;
		if( pHead->PI.x < PLAYER_SIZE / 2 ) pHead->PI.x = PLAYER_SIZE / 2;
	}
	if( GetAsyncKeyState( VK_RIGHT ) & 0x8000 ){
		pHead->PI.x += 2;
		if( pHead->PI.x >= WIDTH - 1 - PLAYER_SIZE / 2 ) pHead->PI.x = WIDTH - 2 - PLAYER_SIZE / 2;
	}
	if( GetAsyncKeyState( VK_CONTROL ) & 0x8000 ) CreateShot( P_SHOT, pHead->PI.x, pHead->PI.y );
}

void EnemyAction()
{
	int i;
	for(i=0;i < MAX_ENEMY;i++){
		if(Enemy[i].EI.LiveFlag){
			if( Enemy[i].MoveFlag ) Enemy[i].EI.x++;
			else Enemy[i].EI.x--;

			if( abs(Enemy[i].StartX - Enemy[i].EI.x) > 13 ){
				Enemy[i].EI.y++;
				Enemy[i].MoveFlag = !Enemy[i].MoveFlag;
			}

			if( rand() % 100 < 2 ) CreateShot( E_SHOT, Enemy[i].EI.x, Enemy[i].EI.y );
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
		Enemy[i].EI.x = x;
		Enemy[i].EI.y = y;
		Enemy[i].EI.LiveFlag = 1;
		Enemy[i].MoveFlag = 1;
		Enemy[i].StartX = x;

		x += 5;
	}



	// Player Life part
	for(j=0; j < 3; j++){
		// Always, pHead'll be current player.
		AddPlayerLife();
	}

	pHead->PI.x = 39;
	pHead->PI.y = 19;
	pHead->PI.LiveFlag = 1;
	pHead->PI.HP = 3;
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

	memset(HpScoreLifeItem,' ', WIDTH);
	HpScoreLifeItem[WIDTH - 2] = NULL;

	DrawPlayer();
	DrawEnemy();
	DrawShot();
	DrawPlayerHP();
	DrawPlayerLife();

	for(i=0;i < HEIGHT;i++){
		MoveCursor(0, i);
		printf(Screen[i]);
	}

	// Draw Bottom Info
	MoveCursor(0, 24);
	printf(HpScoreLifeItem);
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
		if(Enemy[i].EI.LiveFlag){
			x = Enemy[i].EI.x - ENEMY_SIZE / 2;
			y = Enemy[i].EI.y;

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
	int x = pHead->PI.x - PLAYER_SIZE / 2;
	int y = pHead->PI.y;

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
		HpScoreLifeItem[x] = PlayerUnit[i];
		x++;
	}
	HpScoreLifeItem[x] = 'x';
	x++;
	// 숫자를 문자로 출력하기위해서 '0'의 아스키코드를 구하고 PlayerCount와 더한다.
	HpScoreLifeItem[x] = '0' + PlayerCount;
}

void DrawPlayerHP()
{
	int x = 10;

	HpScoreLifeItem[x] = 'H';
	x++;
	HpScoreLifeItem[x] = 'P';
	x++;
	HpScoreLifeItem[x] = ' ';
	x++;
	HpScoreLifeItem[x] = '0' + pHead->PI.HP;
}

void NewStart()
{
	pHead->PI.x = 39;
	pHead->PI.y = 19;
	pHead->PI.LiveFlag = 1;
	pHead->PI.HP = 3;
}

/*
TODO
1. HP
 (1) LiveFlag를 숫자로 바꾸고 LiveFlag를 줄이는 식으로 한다.
     적의 HP는 2, 플레이어의 HP는 3이다.
 (2) Linked List를 사용해서 비행기 갯수를 조절한다.
void AddPlayerLife();
void DrawPlayerLife();
void ClearHeap();
void NewStart();

2. SCORE
3. Next Stage
4. Item (적어도 3개 이상)
5. Boss

2 인용게임 만들기
소리?

*/