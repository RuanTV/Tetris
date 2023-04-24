#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <time.h>
#include <Windows.h>

#define ROWS 20
#define COLS 20

#define SPACE_COUNT 40
#define TOP_SPACE_COUNT 3
#define TETRIS_COUNT 0, 15

typedef enum Keys {
	KEY_LEFT = 75, KEY_DOWN = 80, KEY_RIGHT = 77, KEY_UP = 72
} Keys;

typedef struct Point {
	int rows;
	int cols;
} Point;

void DrawGameboard();

void DrawGameboardCollision();

void DrawGameboardTemp();

void InitGameboard();

void InitTetrominos();

void SpawnTetromino(int rows, int cols, int value, Point oldPosition);

void SpawnTetrominoCollision(int rows, int cols, int value, Point oldPosition);

void SpawnTetrominoTemp(int rows, int cols, int value, Point oldPosition);

int TestCollision(Point nextPosition, int value); // 1 -> collision 0 -> no collision

int TestCollisionRotate(Point currentPosition, int value, int nextValue); // 1 -> collision 0 -> no collision

void GotoPosition(int rows, int cols);

void MoveTetromino(Point* currentPosition, int* currentValue, int input);

void ClearPosition(Point position, int value);

void ClearPositionTemp(Point position, int value);

void UpdateCurrentTetromino(Point* currentPosition, int currentValue);

void GetPatternByValue(int value, int* i, int* j, int* startI, int* startJ, int* flag);

int GetRandomInt(int min, int max);

int GetRandomIntEven();

void PrintSpaces();

void PrintTopSpaces();

void ResetClock();

int Rotate(Point currentPosition, int* value); // success -> 1 failed -> 0

void TrySpawn(Point nextPosition, int value);

int DetectGameOver();

void PrintInterface();

void AddScore(int value);

void RemoveLine();

void Reset();

void BorderDecoration();

clock_t start, end;
double gameClock;

char tetromino[16][4][10];

char gameboard[ROWS][COLS];
char gameboardCollision[ROWS][COLS];
char gameboardTemp[ROWS][COLS];
char gameboardTemp2[ROWS][COLS];

Point tetrominoPattern[4];

int score = 0;

Point nextPositionTetris;
int nextValue, generateNewTetris = 0;

int main()
{
	srand((unsigned int)time(NULL));

	Point currentPosition = { 0 };
	int currentValue = 0;


	InitTetrominos();

	InitGameboard(gameboard);

	DrawGameboard();

	currentValue = GetRandomInt(TETRIS_COUNT);
	currentPosition.rows = 0;
	currentPosition.cols = GetRandomIntEven();

	int spawnI = 0, spawnJ = 0, startSpawnI = 0, startSpawnJ = 0, flagSpawn = 0;
	GetPatternByValue(currentValue, &spawnI, &spawnJ, &startSpawnI, &startSpawnJ, &flagSpawn);
	if (startSpawnI == 1)
		currentPosition.rows--;

	SpawnTetromino(currentPosition.rows, currentPosition.cols, currentValue, currentPosition);
	GotoPosition(25, 0);

	int input = 0, gameRunning = 1;

	PrintInterface();

	nextValue = GetRandomInt(TETRIS_COUNT);
	nextPositionTetris.cols = GetRandomIntEven();
	nextPositionTetris.rows = 0;
	generateNewTetris = 1;

	start = clock();
	while (gameRunning)
	{
		Sleep(10);

		if (_kbhit())
		{
			input = _getch();

			MoveTetromino(&currentPosition, &currentValue, input);
		}

		if (generateNewTetris)
		{
			nextValue = GetRandomInt(TETRIS_COUNT);
			nextPositionTetris.cols = GetRandomIntEven(); //4 - 10
			nextPositionTetris.rows = 0;
			generateNewTetris = 0;
			PrintInterface();
		}

		if (gameClock > 1.0)
		{
			MoveTetromino(&currentPosition, &currentValue, KEY_DOWN);

			ResetClock();
		}

		end = clock();
		gameClock = ((double)(end - start)) / CLOCKS_PER_SEC;

		GotoPosition(22, 0);
	}
}

void RemoveLine()
{
	int fill = 1, fillCount = 0;

	int fillRows[ROWS] = { -1 };

	for (int i = 0; i < ROWS; i++)
	{
		fill = 1;
		for (int j = 0; j < COLS; j += 2)
		{
			if (gameboard[i][j] == '.')
			{
				fill = 0;
			}
		}

		if (fill)
		{
			score += 100;
			for (int j = 0; j < COLS; j++)
			{
				gameboard[i][j] = '.';
				gameboardCollision[i][j] = '.';
			}

			fillRows[fillCount] = i;
			fillCount++;
		}
	}

	if (fillCount)
	{
		for (int i = 0; i < ROWS; i++)
		{
			for (int j = 0; j < COLS; j++)
			{
				gameboardCollision[i][j] = '.';
			}
		}


		int lineToMove = fillRows[0] - 1;
		int targetLine = (fillRows[0] + fillCount) - 1;
		int n = ROWS;

		for (int i = n; i > 0; i--)
		{

			for (int j = 0; j < COLS; j++)
			{
				gameboard[targetLine][j] = gameboard[lineToMove][j];
				gameboard[lineToMove][j] = '.';
			}

			lineToMove--;
			targetLine--;
			if (lineToMove < 0)
			{
				break;
			}
		}

		for (int i = 0; i < ROWS; i++)
		{
			for (int j = 0; j < COLS; j++)
			{
				GotoPosition(i + TOP_SPACE_COUNT, j + SPACE_COUNT);
				printf("%c", gameboard[i][j]);
			}

			for (int k = 0; k < COLS; k++)
			{
				if (!(k % 2) && gameboard[i][k] == '.')
				{
					GotoPosition(i + TOP_SPACE_COUNT, k + SPACE_COUNT);
					printf(" ");
				}

			}
		}
		for (int i = 0; i < ROWS; i++)
		{
			for (int j = 0; j < COLS; j++)
			{
				if (gameboard[i][j] != '.')
					gameboardCollision[i][j] = 'o';
			}
		}
		Sleep(10);
	}
}

void ResetClock()
{
	start = clock();
}

int GetRandomInt(int min, int max)
{
	if (!(min + max))
		return 0;
	return min + rand() % (max - min);
}

int GetRandomIntEven()
{
	int min = 1, max = 5;

	int value = min + rand() % (max - min);
	switch (value)
	{
	case 1:
		return 6;
	case 2:
		return 6;
	case 3:
		return 8;
	default:
		return 8;
	}
}

void GetPatternByValue(int value, int* i, int* j, int* startI, int* startJ, int* flag)
{
	switch (value)
	{
	case 0:
		*i = 2; *j = 6;
		*startI = 0; *startJ = 0;
		*flag = 0;
		break;
	case 1:
		*i = 3; *j = 6;
		*startI = 0; *startJ = 2;
		*flag = 1;
		break;
	case 2:
		*i = 3; *j = 6;
		*startI = 1; *startJ = 0;
		*flag = 4;
		break;
	case 3:
		*i = 3; *j = 6;
		*startI = 0; *startJ = 2;
		*flag = 1;
		break;

	case 4:
		*i = 2; *j = 6;
		*startI = 0; *startJ = 0;
		*flag = 0;
		break;
	case 5:
		*i = 3; *j = 6;
		*startI = 0; *startJ = 2;
		*flag = 1;
		break;
	case 6:
		*i = 3; *j = 6;
		*startI = 1; *startJ = 0;
		*flag = 4;
		break;
	case 7:
		*i = 3; *j = 4;
		*startI = 0; *startJ = 0;
		*flag = 2;
		break;

	case 8:
		*i = 4; *j = 4;
		*startI = 0; *startJ = 2;
		*flag = 3;
		break;
	case 9:
		*i = 2; *j = 8;
		*startI = 1; *startJ = 0;
		*flag = 4;
		break;
	case 10:
		*i = 4; *j = 4;
		*startI = 0; *startJ = 2;
		*flag = 3;
		break;
	case 11:
		*i = 2; *j = 8;
		*startI = 1; *startJ = 0;
		*flag = 4;
		break;

	case 12:
		*i = 3; *j = 6;
		*startI = 0; *startJ = 2;
		*flag = 1;
		break;
	case 13:
		*i = 3; *j = 6;
		*startI = 1; *startJ = 0;
		*flag = 4;
		break;
	case 14:
		*i = 3; *j = 4;
		*startI = 0; *startJ = 0;
		*flag = 2;
		break;
	case 15:
		*i = 2; *j = 6;
		*startI = 0; *startJ = 0;
		*flag = 0;
		break;

	default:
		break;
	}
}

int TestCollisionRotate(Point currentPosition, int value, int nextValue)
{
	int maxI = 3, maxJ = 6, startI = 0, startJ = 0, flag = 0;

	GetPatternByValue(value, &maxI, &maxJ, &startI, &startJ, &flag);

	for (int i = startI; i < maxI; i++)
	{
		for (int j = startJ; j < maxJ; j++)
		{
			if (gameboardTemp[currentPosition.rows + i][currentPosition.cols + j] != '.')
			{
				if (gameboardCollision[currentPosition.rows + i][currentPosition.cols + j] == 'o')
				{
					return 1;
				}
			}
		}
	}
	return 0;
}

void PrintInterface()
{
	GotoPosition(0 + TOP_SPACE_COUNT, COLS + SPACE_COUNT + 10);
	printf("SCORE: %d          ", score);

	GotoPosition(3 + TOP_SPACE_COUNT, COLS + SPACE_COUNT + 10);
	printf("MOVE LEFT  => LEFT ARROW  ");

	GotoPosition(4 + TOP_SPACE_COUNT, COLS + SPACE_COUNT + 10);
	printf("MOVE RIGHT => RIGHT ARROW ");

	GotoPosition(5 + TOP_SPACE_COUNT, COLS + SPACE_COUNT + 10);
	printf("ROTATE     => UP ARROW ");



	GotoPosition(10 + TOP_SPACE_COUNT, COLS + SPACE_COUNT + 10);
	for (int i = 0; i < 4; i++)
	{
		GotoPosition(10 + i + TOP_SPACE_COUNT, COLS + SPACE_COUNT + 10);
		for (int j = 0; j < 8; j++)
		{
			if (tetromino[nextValue][i][j] == '.')
				printf(" ");
			else
				printf("%c", tetromino[nextValue][i][j]);
		}
	}
}

int TestCollision(Point nextPosition, int value)
{
	int maxI = 3, maxJ = 6, startI = 0, startJ = 0, flag = 0;

	GetPatternByValue(value, &maxI, &maxJ, &startI, &startJ, &flag);

	SpawnTetrominoTemp(nextPosition.rows, nextPosition.cols, value, nextPosition);

	for (int i = startI; i < maxI; i++)
	{
		for (int j = startJ; j < maxJ; j++)
		{
			if (gameboardTemp[nextPosition.rows + i][nextPosition.cols + j] != '.')
			{
				if (gameboardCollision[nextPosition.rows + i][nextPosition.cols + j] == 'o')
				{
					ClearPositionTemp(nextPosition, value);

					return 1;
				}
			}
		}
	}

	ClearPositionTemp(nextPosition, value);

	return 0;
}


void UpdateCurrentTetromino(Point* currentPosition, int currentValue)
{

	MoveTetromino(currentPosition, &currentValue, KEY_DOWN);

}

void SpawnTetrominoTemp(int rows, int cols, int value, Point oldPosition)
{
	int maxI = 3, maxJ = 6, startI = 0, startJ = 0, flag = 0;

	GetPatternByValue(value, &maxI, &maxJ, &startI, &startJ, &flag);


	for (int i = startI; i < maxI; i++)
	{
		for (int j = startJ; j < maxJ; j++)
		{
			if (gameboardTemp[i + rows][j + cols] == '.')
			{
				gameboardTemp[i + rows][j + cols] = tetromino[value][i][j];
			}
		}
	}
}

void Reset()
{
	InitGameboard();
	system("cls");
	GotoPosition(0, 0);
	DrawGameboard();
	int spawnI = 0, spawnJ = 0, startSpawnI = 0, startSpawnJ = 0, flagSpawn = 0;
	GetPatternByValue(nextValue, &spawnI, &spawnJ, &startSpawnI, &startSpawnJ, &flagSpawn);
	if (startSpawnI == 1)
		nextPositionTetris.rows--;
	SpawnTetromino(nextPositionTetris.rows, nextPositionTetris.cols, nextValue, nextPositionTetris);
	score = 0;
	ResetClock();
}

void AddScore(int value)
{
	if (value >= 4 && value <= 7)
	{
		score += 4;
	}
	if (value >= 8 && value <= 11)
	{
		score += 5;
	}
	if (value >= 12 && value <= 15)
	{
		score += 6;
	}
	else
	{
		score += 3;
	}

	PrintInterface();
}

void MoveTetromino(Point* currentPosition, int* currentValue, int input)
{
	Point oldPosition = { currentPosition->rows, currentPosition->cols };
	int spawnNewTetromino = 0;
	Point nextPosition = oldPosition;

	int oldCurrentValue = *currentValue;
	int hasRotate = 0;

	int marginI = 3, marginJ = 6, startMarginI = 0, startMarginJ = 0, flag = 0;
	GetPatternByValue(*currentValue, &marginI, &marginJ, &startMarginI, &startMarginJ, &flag);

	if (input == KEY_UP)
	{
		hasRotate = Rotate(*currentPosition, currentValue);
	}

	if (input == KEY_DOWN)
	{
		nextPosition.rows++;

		if (currentPosition->rows < ROWS - marginI)
		{
			currentPosition->rows++;

			if (currentPosition->rows == ROWS - marginI)
			{
				spawnNewTetromino = 1;
			}
		}
	}
	if (input == KEY_RIGHT)
	{

		if (currentPosition->cols < COLS - marginJ)
		{
			nextPosition.cols += 2;
			if (TestCollision(nextPosition, *currentValue))
			{
				return;
			}

			currentPosition->cols += 2;
		}
	}
	if (input == KEY_LEFT)
	{
		GetPatternByValue(*currentValue, &marginI, &marginJ, &startMarginI, &startMarginJ, &flag);

		if (currentPosition->cols > 1 || (startMarginJ == 2 && currentPosition->cols >= 0))
		{
			nextPosition.cols -= 2;
			if (TestCollision(nextPosition, *currentValue))
			{
				return;
			}

			currentPosition->cols -= 2;
		}

	}

	if (hasRotate)
		ClearPosition(oldPosition, oldCurrentValue);
	else
		ClearPosition(oldPosition, *currentValue);

	SpawnTetromino(currentPosition->rows, currentPosition->cols, *currentValue, oldPosition);

	int maxI = 3, maxJ = 6, startI = 0, startJ = 0, flag2 = 0;
	GetPatternByValue(*currentValue, &maxI, &maxJ, &startI, &startJ, &flag2);
	if (maxI == 3 && currentPosition->rows == ROWS - 3)
	{
		spawnNewTetromino = 1;
	}
	else if (currentPosition->rows == COLS - 4 && (*currentValue == 8 || *currentValue == 10))
	{
		spawnNewTetromino = 1;
	}

	if (KEY_DOWN)
	{
		nextPosition.rows++;
		if (TestCollision(nextPosition, *currentValue))
		{
			spawnNewTetromino = 1;
		}
	}

	if (spawnNewTetromino)
	{
		AddScore(*currentValue);

		SpawnTetrominoCollision(currentPosition->rows, currentPosition->cols, *currentValue, oldPosition);
		generateNewTetris = 1;
		*currentValue = nextValue;
		currentPosition->cols = nextPositionTetris.cols;
		currentPosition->rows = 0;
		generateNewTetris = 1;

		int spawnI = 0, spawnJ = 0, startSpawnI = 0, startSpawnJ = 0, flagSpawn = 0;
		GetPatternByValue(*currentValue, &spawnI, &spawnJ, &startSpawnI, &startSpawnJ, &flagSpawn);
		if (startSpawnI == 1)
			currentPosition->rows--;

		RemoveLine();

		SpawnTetromino(currentPosition->rows, currentPosition->cols, *currentValue, oldPosition);

		TrySpawn(*currentPosition, *currentValue);
	}

}

void BorderDecoration()
{
	for (int i = 0; i < ROWS; i++)
	{
		GotoPosition(i + TOP_SPACE_COUNT, SPACE_COUNT - 2);
		printf("<!");
	}
	GotoPosition(ROWS + TOP_SPACE_COUNT, SPACE_COUNT - 2);
	printf("<!");
	for (int i = 0; i < COLS; i++)
	{
		GotoPosition(ROWS + TOP_SPACE_COUNT, SPACE_COUNT + i);
		printf("=");
		if (!(i % 2))
		{
			GotoPosition(ROWS + TOP_SPACE_COUNT + 1, SPACE_COUNT + i);
			printf("\\/");
		}
	}


	for (int i = 0; i < ROWS; i++)
	{
		GotoPosition(i + TOP_SPACE_COUNT, SPACE_COUNT + COLS);
		printf("!>");
	}
	GotoPosition(ROWS + TOP_SPACE_COUNT, SPACE_COUNT + COLS);
	printf("!>");
}

void TrySpawn(Point nextPosition, int value)
{

	if (TestCollision(nextPosition, value))
	{
		SpawnTetromino(nextPosition.rows, nextPosition.cols, value, nextPosition);

		int input = 0;
		GotoPosition(ROWS + TOP_SPACE_COUNT + 3, SPACE_COUNT);
		printf("Game Over! Press UP ARROW to play again");
		GotoPosition(ROWS + TOP_SPACE_COUNT + 4, SPACE_COUNT);
		printf("           Press LEFT ARROW to exit");
		while (1)
		{
			if (_kbhit())
			{
				input = _getch();

				if (input == KEY_UP)
				{
					Reset();
					break;
				}
				else if (input == KEY_LEFT)
				{
					exit(0);
				}

			}
		}
	}
}

void ClearPositionTemp(Point position, int value)
{
	int maxI = 3, maxJ = 6, startI = 0, startJ = 0, flag = 0;

	GetPatternByValue(value, &maxI, &maxJ, &startI, &startJ, &flag);

	for (int i = startI; i < maxI; i++)
	{
		for (int j = startJ; j < maxJ; j++)
		{
			gameboardTemp[position.rows + i][position.cols + j] = '.';
		}
	}
}

void ClearPosition(Point position, int value)
{
	int maxI = 3, maxJ = 6, startI = 0, startJ = 0, flag = 0;

	GetPatternByValue(value, &maxI, &maxJ, &startI, &startJ, &flag);


	for (int i = startI; i < maxI; i++)
	{
		for (int j = startJ; j < maxJ; j++)
		{
			if (gameboardCollision[position.rows + i][position.cols + j] != 'o')
			{
				GotoPosition(position.rows + i + TOP_SPACE_COUNT, position.cols + j + SPACE_COUNT);
				printf(".");
				gameboard[position.rows + i][position.cols + j] = '.';
				GotoPosition(0, 0);
			}
		}
		for (int k = startJ; k < maxJ; k++)
		{
			if (!(k % 2) && gameboard[i + position.rows][k + position.cols] == '.')
			{
				GotoPosition(i + position.rows + TOP_SPACE_COUNT, k + position.cols + SPACE_COUNT);
				printf(" ");
			}
		}
	}
}

void SpawnTetromino(int rows, int cols, int value, Point oldPosition)
{
	int maxI = 3, maxJ = 6, startI = 0, startJ = 0, flag = 0;

	GetPatternByValue(value, &maxI, &maxJ, &startI, &startJ, &flag);


	for (int i = startI; i < maxI; i++)
	{
		for (int j = startJ; j < maxJ; j++)
		{
			GotoPosition(i + rows + TOP_SPACE_COUNT, j + cols + SPACE_COUNT);

			if (gameboard[i + rows][j + cols] == '.')
			{
		
				if (tetromino[value][i][j] == '.' && !(j % 2))
					printf(" ");
				else if (tetromino[value][i][j] == '.')
					printf(".");
				else
					printf("%c", tetromino[value][i][j]);
				
				gameboard[i + rows][j + cols] = tetromino[value][i][j];

			}
		}

	}

	GotoPosition(0, 0);
}

void SpawnTetrominoCollision(int rows, int cols, int value, Point oldPosition)
{
	int maxI = 3, maxJ = 6, startI = 0, startJ = 0, flag = 0;

	GetPatternByValue(value, &maxI, &maxJ, &startI, &startJ, &flag);

	for (int i = startI; i < maxI; i++)
	{
		for (int j = startJ; j < maxJ; j++)
		{
			if (gameboardCollision[i + rows][j + cols] == '.' && tetromino[value][i][j] != '.')
			{
				gameboardCollision[i + rows][j + cols] = 'o';
			}
		}
	}
}

void InitTetrominos()
{
	// 2 x 6   start i: 0, start j: 0
	strcpy(tetromino[0][0], "[][]....");
	strcpy(tetromino[0][1], "..[][]..");
	strcpy(tetromino[0][2], "........");
	strcpy(tetromino[0][3], "........");

	// 3 x 6   start i: 0, start j: 2
	strcpy(tetromino[1][0], "....[]..");
	strcpy(tetromino[1][1], "..[][]..");
	strcpy(tetromino[1][2], "..[]....");
	strcpy(tetromino[1][3], "........");

	// 3 x 6   start i: 1, start j: 0
	strcpy(tetromino[2][0], "........");
	strcpy(tetromino[2][1], "[][]....");
	strcpy(tetromino[2][2], "..[][]..");
	strcpy(tetromino[2][3], "........");

	// 3 x 6   start i: 0, start j: 1
	strcpy(tetromino[3][0], "....[]..");
	strcpy(tetromino[3][1], "..[][]..");
	strcpy(tetromino[3][2], "..[]....");
	strcpy(tetromino[3][3], "........");

	// ===============================

	// 2 x 6   start i: 0, start j: 0
	strcpy(tetromino[4][0], "..[]....");
	strcpy(tetromino[4][1], "[][][]..");
	strcpy(tetromino[4][2], "........");
	strcpy(tetromino[4][3], "........");

	// 3 x 6   start i: 0, start j: 2
	strcpy(tetromino[5][0], "..[]....");
	strcpy(tetromino[5][1], "..[][]..");
	strcpy(tetromino[5][2], "..[]....");
	strcpy(tetromino[5][3], "........");

	// 3 x 6   start i: 1, start j: 0
	strcpy(tetromino[6][0], "........");
	strcpy(tetromino[6][1], "[][][]..");
	strcpy(tetromino[6][2], "..[]....");
	strcpy(tetromino[6][3], "........");

	// 3 x 4   start i: 0, start j: 0
	strcpy(tetromino[7][0], "..[]....");
	strcpy(tetromino[7][1], "[][]....");
	strcpy(tetromino[7][2], "..[]....");
	strcpy(tetromino[7][3], "........");

	// ===============================

	// 4 x 8   start i: 0, start j: 2
	strcpy(tetromino[8][0], "..[]....");
	strcpy(tetromino[8][1], "..[]....");
	strcpy(tetromino[8][2], "..[]....");
	strcpy(tetromino[8][3], "..[]....");

	// 4 x 8   start i: 1, start j: 0
	strcpy(tetromino[9][0], "........");
	strcpy(tetromino[9][1], "[][][][]");
	strcpy(tetromino[9][2], "........");
	strcpy(tetromino[9][3], "........");

	// 4 x 8   start i: 0, start j: 2
	strcpy(tetromino[10][0], "..[]....");
	strcpy(tetromino[10][1], "..[]....");
	strcpy(tetromino[10][2], "..[]....");
	strcpy(tetromino[10][3], "..[]....");

	// 4 x 8   start i: 1, start j: 0
	strcpy(tetromino[11][0], "........");
	strcpy(tetromino[11][1], "[][][][]");
	strcpy(tetromino[11][2], "........");
	strcpy(tetromino[11][3], "........");

	// ===============================

	// 3 x 6   start i: 0, start j: 2
	strcpy(tetromino[12][0], "..[]....");
	strcpy(tetromino[12][1], "..[]....");
	strcpy(tetromino[12][2], "..[][]..");
	strcpy(tetromino[12][3], "........");

	// 3 x 6   start i: 1, start j: 0
	strcpy(tetromino[13][0], "........");
	strcpy(tetromino[13][1], "[][][]..");
	strcpy(tetromino[13][2], "[]......");
	strcpy(tetromino[13][3], "........");

	// 3 x 4   start i: 0, start j: 0
	strcpy(tetromino[14][0], "[][]....");
	strcpy(tetromino[14][1], "..[]....");
	strcpy(tetromino[14][2], "..[]....");
	strcpy(tetromino[14][3], "........");

	// 2 x 6   start i: 0, start j: 0
	strcpy(tetromino[15][0], "....[]..");
	strcpy(tetromino[15][1], "[][][]..");
	strcpy(tetromino[15][2], "........");
	strcpy(tetromino[15][3], "........");
}

int Rotate(Point currentPosition, int* value)
{
	int cancelRotate = 0;

	int maxI = 3, maxJ = 6, startI = 0, startJ = 0, flag = 0;

	GetPatternByValue(*value, &maxI, &maxJ, &startI, &startJ, &flag);

	if (currentPosition.cols < 0 && flag == 1)
	{
		return 0;
	}
	else if (currentPosition.cols == COLS - 4 && flag == 2)
	{
		return 0;
	}
	else if (((currentPosition.cols == COLS - 4) || (currentPosition.cols < 0)) && flag == 3)
	{
		return 0;
	}
	else if (currentPosition.rows == -1 && flag == 4)
	{
		return 0;
	}
	else if (currentPosition.cols == COLS - 6 && (*value == 8 || *value == 10))
	{
		return 0;
	}
	else if (currentPosition.rows == COLS - 3 && (*value == 9 || *value == 11))
	{
		return 0;
	}

	int currentValue = *value;

	if (currentValue <= 3)
	{
		(*value)++;
		if (*value > 3)
		{
			*value = 0;
		}
	}
	if (currentValue >= 4 && currentValue <= 7)
	{
		(*value)++;
		if (*value > 7)
		{
			*value = 4;
		}
	}
	if (currentValue >= 8 && currentValue <= 11)
	{
		(*value)++;
		if (*value > 11)
		{
			*value = 8;
		}
	}
	if (currentValue >= 12 && currentValue <= 15)
	{
		(*value)++;
		if (*value > 15)
		{
			*value = 12;
		}
	}

	SpawnTetrominoTemp(currentPosition.rows, currentPosition.cols, *value, currentPosition);

	if (TestCollisionRotate(currentPosition, *value, *value))
	{
		cancelRotate = 1;
	}

	ClearPositionTemp(currentPosition, *value);

	if (cancelRotate)
	{
		*value = currentValue;
		return 0;
	}
	else
	{
		return 1;
	}

}

int DetectGameOver()
{
	int gameOver = 1;
	for (int i = 0; i < ROWS; i++)
	{
		gameOver = 1;
		for (int j = 0; j < COLS; j += 2)
		{
			if (gameboard[i][j] == '.')
			{
				gameOver = 0;
			}
		}
		if (gameOver)
		{
			return 1;
		}
	}
	return 0;
}

void InitGameboard()
{
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			gameboard[i][j] = '.';
		}
	}
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			gameboardCollision[i][j] = '.';
		}
	}
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			gameboardTemp[i][j] = '.';
		}
	}
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			gameboardTemp2[i][j] = '.';
		}
	}
}

void DrawGameboard()
{
	PrintTopSpaces();
	for (int i = 0; i < ROWS; i++)
	{
		PrintSpaces();
		for (int j = 0; j < COLS; j++)
		{
			printf("%c", gameboard[i][j]);
		}

		for (int k = 0; k < COLS; k++)
		{
			if (!(k % 2) && gameboard[i][k] == '.')
			{
				GotoPosition(i + TOP_SPACE_COUNT, k + SPACE_COUNT);
				printf(" ");
			}
				
		}

		printf("\n");
	}
	BorderDecoration();
}

void DrawGameboardCollision()
{
	GotoPosition(25, 0);
	printf("\n\n -----------\n");
	for (int i = 0; i < ROWS; i++)
	{
		PrintSpaces();
		for (int j = 0; j < COLS; j++)
		{
			printf("%c", gameboardCollision[i][j]);
		}
		printf("\n");
	}
	GotoPosition(0, 0);
}

void GotoPosition(int rows, int cols)
{
	COORD pos = { cols, rows };
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(console, pos);
}

void DrawGameboardTemp()
{
	GotoPosition(25, 50);
	printf("\n\n -----------\n");
	for (int i = 0; i < ROWS; i++)
	{
		PrintSpaces();
		for (int j = 0; j < COLS; j++)
		{
			printf("%c", gameboardTemp[i][j]);
		}
		printf("\n");
	}
	GotoPosition(0, 0);
}

void PrintSpaces()
{
	for (int i = 0; i < SPACE_COUNT; i++)
		printf(" ");
}

void PrintTopSpaces()
{
	for (int i = 0; i < TOP_SPACE_COUNT; i++)
		printf("\n");
}
