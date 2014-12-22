#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <memory.h>
#include "Console.h"

char Screen[HEIGHT][WIDTH];
char HpScoreLifeItem[WIDTH];
char Score;
int Stage;

#define TRUE					1
#define MAX_STAGE			2

// �÷��̾� ����� ���� 
#define PLAYER_SIZE          5
char PlayerUnit[PLAYER_SIZE+1] = "-#^#-";

struct ObjectInfo{
	int x, y;
	int LiveFlag;
	int HP;
	int ShotType;
};

//��ũ�帮��Ʈ�� �÷��̾� ��� ���� �����ϱ�.
typedef struct PlayerInfoLinkedList{
	struct ObjectInfo PI;
	struct PlayerInfoLinkedList *pNext;
} PlayerLL;

PlayerLL *pHead = NULL;
int PlayerCount;


// �� ����� ���� 
#define ENEMY_SIZE         3
char EnemyUnit[ENEMY_SIZE+1] = "@-@";

struct EnemyInfo{
	ObjectInfo EI;
	int MoveFlag;
	int StartX;
};

#define MAX_ENEMY      30
struct EnemyInfo      Enemy[MAX_ENEMY];

// �Ѿ� ����

#define P_SHOT			0
#define E_SHOT			1
#define SUPER_SHOT		2
char PlayerShot = '|';
char EnemyShot = '*';
char SuperShot = ':';

struct ShotInfo{
	int x, y;
	int Type;
	int UseFlag;
};

#define MAX_SHOT			40
struct ShotInfo         Shot[MAX_SHOT];

struct ItemInfo{
	int x, y;
	int Type;
	int UseFlag;
};


#define POWERUP_ITEM		0
#define LIFE_ITEM		0
#define BOMB_ITEM		0
char PowerUpItem = '#';
char LifeItem = '��';
char BombItem = '��';
int ItemSpeed = 0;

#define MAX_ITEM			10
struct ItemInfo			Item[MAX_ITEM];

// �Լ�
void InitialObject();
void DrawPlayer();
void DrawEnemy();
void Draw();
void DrawScreenClear();

void EnemyAction();
void PlayerAction();
void CreateShot(int Type, int x, int y);
void DrawShot();
void ShotAction();
void CheckCrush();
int CheckClear();

void CreateItem(int Type, int x, int y);
void DrawItem();
void ItemAction();

// ��ũ�帮��Ʈ
void AddPlayerLife();
void DrawPlayerLife();
void ClearHeap();
void NewStart();
void DrawHpScoreOfPlayer();

void main()
{
	Initial();
	srand((unsigned int)time(NULL));

	MoveCursor(30, 12);
	printf("Stage ����ȭ��.. �ڵ����� �����մϴ�.!\n");
	Sleep(1000);

	// Stage = 0 could be changed by User
	for(Stage = 0; Stage < MAX_STAGE; Stage++){

		InitialObject();

		while(1){
			EnemyAction();
			PlayerAction();
			ShotAction();
			ItemAction();
			CheckCrush();
			Draw();
			if( CheckClear() ) break;

			Sleep(70 - Stage*10);
		}

		if(pHead->PI.LiveFlag == 0){
			// game over
			break;
		}
	}

	ClearHeap();
}

int CheckClear()
{
	if( pHead->PI.LiveFlag == 0){
		if( pHead->pNext == NULL){
			MoveCursor( 36, 12 );
			printf("���� ����!!");
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
	printf("���� ��� ����!");
	Sleep(1000);

	return 1;
	
}

void CheckCrush()
{

	int i, j;
	for(i=0; i < MAX_SHOT; i++){
		if( Shot[i].UseFlag ){
			if( Shot[i].Type == P_SHOT || Shot[i].Type == SUPER_SHOT ){
				for( j=0; j < MAX_ENEMY; j++){
					if( Enemy[j].EI.LiveFlag ){
						if( Shot[i].x >= Enemy[j].EI.x - ENEMY_SIZE / 2 &&
							Shot[i].x <= Enemy[j].EI.x + ENEMY_SIZE / 2 &&
							Shot[i].y == Enemy[j].EI.y ){
								Shot[i].UseFlag = 0;
								Enemy[j].EI.HP--;
								if (Enemy[j].EI.HP <= 0){
									Enemy[j].EI.LiveFlag = 0;
									Score++;
									if( rand() % 100 < 70 ) CreateItem( POWERUP_ITEM, Enemy[j].EI.x, Enemy[j].EI.y );
								}
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
						if(pHead->PI.HP <= 0){
							pHead->PI.LiveFlag = 0;
						}

				}
			}
		}
	}

	for(i=0; i < MAX_ITEM; i++){
		if( Item[i].UseFlag ) {
			if( Item[i].x >= pHead->PI.x - PLAYER_SIZE / 2 &&
				Item[i].x <= pHead->PI.x + PLAYER_SIZE / 2 &&
				Item[i].y == pHead->PI.y ){
					Item[i].UseFlag = 0;
					if(Item[i].Type == POWERUP_ITEM){
						pHead->PI.ShotType++;
					}
			}
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
	if( GetAsyncKeyState( VK_CONTROL ) & 0x8000 )
	{
		if(pHead->PI.ShotType == 0){
			CreateShot( P_SHOT, pHead->PI.x, pHead->PI.y );
		}else if(pHead->PI.ShotType >= 1){
			CreateShot( SUPER_SHOT, pHead->PI.x, pHead->PI.y );
		}

	}
}

void EnemyAction()
{
	int i;
	if(Stage == 0){
		for(i=0;i < MAX_ENEMY;i++){
			if(Enemy[i].EI.LiveFlag){
				if( Enemy[i].MoveFlag ) Enemy[i].EI.x++;
				else Enemy[i].EI.x--;

				if( abs(Enemy[i].StartX - Enemy[i].EI.x) > 13 ){
					Enemy[i].EI.y++;
					Enemy[i].MoveFlag = !Enemy[i].MoveFlag;
				}

				if( rand() % 100 < 1 ) CreateShot( E_SHOT, Enemy[i].EI.x, Enemy[i].EI.y );
			}
		}
	}else if(Stage == 1){
		for(i=0;i < MAX_ENEMY;i++){
			if(Enemy[i].EI.LiveFlag){

				if(Enemy[i].MoveFlag == 3){
					Enemy[i].EI.x++;
					Enemy[i].EI.y++;
					if( abs(Enemy[i].StartX - Enemy[i].EI.x) > 7){
						Enemy[i].MoveFlag = 2;
					}
				}else if(Enemy[i].MoveFlag == 2){
					Enemy[i].EI.x--;
					Enemy[i].EI.y -= 1;
					if( Enemy[i].StartX - Enemy[i].EI.x == 0){
						Enemy[i].MoveFlag = 1;
					}
				}else if(Enemy[i].MoveFlag == 1){
					Enemy[i].EI.y += 3;
					Enemy[i].MoveFlag = 3;
				}

				if(Enemy[i].EI.y >= HEIGHT - 1){
					Enemy[i].EI.y = 5;
				}

				if( rand() % 100 < 5 ) CreateShot( E_SHOT, Enemy[i].EI.x, Enemy[i].EI.y );
			}
		}
	}
}

void InitialObject()
{
	int i, x, y;

	// for player intialize
	int j;

	if(Stage == 0){
		x = 17, y = 1;
		for(i=0;i < MAX_ENEMY;i++){
			if(0 == i % 10){
				x = 17;
				y++;
			}
			Enemy[i].EI.x = x;
			Enemy[i].EI.y = y;
			Enemy[i].EI.LiveFlag = 1;
			Enemy[i].EI.HP= 2;
			Enemy[i].MoveFlag = 1;
			Enemy[i].StartX = x;

			x += 5;
		}
	}else if(Stage == 1){
		x = 17, y = 3;
		for(i=0;i < MAX_ENEMY;i++){
			if(0 == i % 10){
				x = 17;
				y++;
			}
			Enemy[i].EI.x = x;
			Enemy[i].EI.y = y;
			Enemy[i].EI.LiveFlag = 1;
			Enemy[i].EI.HP= 3;
			Enemy[i].MoveFlag = 3;
			Enemy[i].StartX = x;

			x += 5;
		}
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
	pHead->PI.ShotType = 0;
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
	DrawItem();
	DrawHpScoreOfPlayer();
	DrawPlayerLife();

	for(i=0;i < HEIGHT;i++){
		MoveCursor(0, i);
		printf(Screen[i]);
	}

	// Draw Bottom Info
	MoveCursor(0, 24);
	printf(HpScoreLifeItem);
}

void DrawScreenClear()
{
	int i;
	for(i=0;i < HEIGHT;i++){
		memset(Screen[i],' ', WIDTH);
		Screen[i][WIDTH - 1] = NULL;
	}

}

void CreateShot(int Type, int x, int y)
{
	int i,j;
	int NumberOfShot = 1;

	if(Type == E_SHOT || Type == P_SHOT){
		for( i=0; i < MAX_SHOT; i++){
			if( Shot[i].UseFlag == 0 ){
				Shot[i].UseFlag = 1;
				Shot[i].x = x;
				Shot[i].y = y;
				Shot[i].Type = Type;

				return;
			}
		}
	}else{
		for(j=0; j < pHead->PI.ShotType; j++){
			for( i=0; i < MAX_SHOT; i++){
				if( Shot[i].UseFlag == 0 ){
					Shot[i].UseFlag = 1;
					if(j == 0){
						Shot[i].x = x;
					}else if(j % 2 == 1){
						Shot[i].x = x+NumberOfShot;
					}else if(j % 2 == 0){
						Shot[i].x = x-NumberOfShot;
						NumberOfShot++;
					}
					Shot[i].y = y;
					Shot[i].Type = Type;

					break;
					//return;
				}
			}
		}
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
			} else if( Shot[i].Type == SUPER_SHOT ){
				Shot[i].y--;
				if( Shot[i].y < 0 ) Shot[i].UseFlag = 0;
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
			else if( Shot[i].Type == SUPER_SHOT ) Screen[ Shot[i].y ][ Shot[i].x ] = SuperShot;
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

	// strcat���� -#^#- �� -#^#- -#^#- ������ �׸� �� ������
	// ȭ���� ũ��� ���Ͽ� -#^#- x 3�� ������� �����.

	int i;
	int x=60;


	for(i=0;i < PLAYER_SIZE; i++){
		HpScoreLifeItem[x] = PlayerUnit[i];
		x++;
	}
	HpScoreLifeItem[x] = 'x';
	x++;
	// ���ڸ� ���ڷ� ����ϱ����ؼ� '0'�� �ƽ�Ű�ڵ带 ���ϰ� PlayerCount�� ���Ѵ�.
	HpScoreLifeItem[x] = '0' + PlayerCount;
}

void DrawHpScoreOfPlayer()
{
	int x = 5;

	HpScoreLifeItem[x] = 'H';
	x++;
	HpScoreLifeItem[x] = 'P';
	x++;
	HpScoreLifeItem[x] = ' ';
	x++;
	HpScoreLifeItem[x] = '0' + pHead->PI.HP;
	x++;
	HpScoreLifeItem[x] = ' ';
	x++;
	HpScoreLifeItem[x] = '/';
	x++;
	HpScoreLifeItem[x] = ' ';
	x++;
	HpScoreLifeItem[x] = 'S';
	x++;
	HpScoreLifeItem[x] = 'c';
	x++;
	HpScoreLifeItem[x] = 'o';
	x++;
	HpScoreLifeItem[x] = 'r';
	x++;
	HpScoreLifeItem[x] = 'e';
	x++;
	HpScoreLifeItem[x] = ' ';
	if (Score){
		x++;
		HpScoreLifeItem[x] = '0' + Score/1000;
		x++;
		HpScoreLifeItem[x] = '0' + Score%1000/100;
		x++;
		HpScoreLifeItem[x] = '0' + Score%1000%100/10;
		x++;
		HpScoreLifeItem[x] = '0' + Score%1000%100%10;
	}
}

void NewStart()
{
	pHead->PI.x = 39;
	pHead->PI.y = 19;
	pHead->PI.LiveFlag = 1;
	pHead->PI.HP = 3;
	pHead->PI.ShotType = 0;
}

void CreateItem(int Type, int x, int y)
{
	int i;
	for( i=0; i < MAX_ITEM; i++){
		if( Item[i].UseFlag == 0 ){
			Item[i].UseFlag = 1;
			Item[i].x = x;
			Item[i].y = y;
			Item[i].Type = Type;

			return;
		}
	}
}

void DrawItem()
{
	int i;
	for(i=0;i < MAX_ITEM;i++){
		if( Item[i].UseFlag ){
			if( Item[i].Type == POWERUP_ITEM ) Screen[ Item[i].y ][ Item[i].x ] = PowerUpItem;
			else if( Item[i].Type == LIFE_ITEM ) Screen[ Item[i].y ][ Item[i].x ] = LifeItem;
			else if( Item[i].Type == BOMB_ITEM ) Screen[ Item[i].y ][ Item[i].x ] = BombItem;
		}
	}
}


void ItemAction()
{
	int i;
	if(ItemSpeed ==1){
		for(i=0;i < MAX_SHOT;i++){
			if( Item[i].UseFlag ){
				if( Item[i].Type == POWERUP_ITEM ||
					Item[i].Type == LIFE_ITEM ||
					Item[i].Type == BOMB_ITEM ){

						Item[i].y++;
						if( Item[i].y >= HEIGHT ) Item[i].UseFlag = 0;
				}
			}
		}
		ItemSpeed = 0;
	}else if(ItemSpeed == 0){
		ItemSpeed = 1;
	}
}

/*
TODO
1. HP
 (1) LiveFlag�� ���ڷ� �ٲٰ� LiveFlag�� ���̴� ������ �Ѵ�.
     ���� HP�� 2, �÷��̾��� HP�� 3�̴�.
 (2) Linked List�� ����ؼ� ����� ������ �����Ѵ�.
void AddPlayerLife();
void DrawPlayerLife();
void ClearHeap();
void NewStart();

2. SCORE

Make Score variable;
Value will be changed in CheckCrush() function
Draw in DrawHpScore

3. Next Stage

�������� ��� int Stage;
�������� ���� printf ���
for���� while ���� ���� �������� ���� �� sleep
�������� ����
if all enemy died, do next stage

***�������� ���� ���ݾ� ���ؾ��ϴ� ��***
InitialObject - Initial Location, HP
EnemyAction - Action
ShotAction - Action
CreateShot - Action
CheckCrush - MAX_ENEMY SIZE
CheckClear - MAX_ENEMY SIZE
DrawEnemy - MAX_ENEMY SIZE
While Sleep Speed
�ϴ� SIZE��Ʈ�� �ٲ��� �ʴ´�.

4. Item (��� 3�� �̻�)

CreateItem(); // It's similar with CreateShot
struct ItemInfo();
DrawItem();
ItemAction();
CheckCrush(); - Item �κ� üũ�ϱ�

SuperShot, Bomb, Hp
  a. SuperShot (Change Shot type)

5. Boss

2 �ο���� �����
�Ҹ�?

*/