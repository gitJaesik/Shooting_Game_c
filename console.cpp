#include <windows.h>
#include <stdio.h>
#include "Console.h"

HANDLE hConsole;

void Initial()
{
	CONSOLE_CURSOR_INFO Cursor;

	Cursor.bVisible = FALSE;
	Cursor.dwSize = 1;

	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorInfo(hConsole, &Cursor);
}

void MoveCursor(int x, int y)
{
	COORD Coord;
	Coord.X = x;
	Coord.Y = y;

	SetConsoleCursorPosition(hConsole, Coord);
}

void ClearScreen()
{
	int x, y;

	for(y=0; y < HEIGHT; y++){
		MoveCursor(0, y);
		for(x=0; x < WIDTH; x++){
			printf("%c", ' ');
		}
	}
}