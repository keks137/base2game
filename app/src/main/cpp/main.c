#include "tile.h"
#include "state.h"
#include "constants.h"
#include <raylib.h>
#include "funny_math.h"
#include "raylib.h"
#include "save.h"
#include "sound.h"
#include <inttypes.h>
#include <memory.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <raymob.h>
// #include <time.h>

enum {
	c_lightgray = 0,
	c_dusty_purple = 16,
	c_orange = 2,
	c_pink = 8,
	c_red = 4,
	c_babyblue = 32,
	c_blue = 64,
	c_ocean = 128,
	c_jade = 256,
	c_green = 512,
	c_lime = 1024,
	c_yellow = 2048,
} Colors;

Tile initTile = {
	.val = 0,
	.scale = 1.0,
	.anim = ANIMNONE,
};
#define MAX_GESTURE_STRINGS 20

unsigned int Screen_Width = 100;
unsigned int Screen_Height = 100;
unsigned int Gamebox_Width = 0;
unsigned int Gamebox_Height = 0;
unsigned int Gamebox_X = 100;
unsigned int Gamebox_Y = 100;
unsigned int Cell_Width = 0;
unsigned int Cell_Height = 0;

unsigned int randomSeed = 47060;
Vector2 touchPosition = { 0, 0 };
Rectangle touchArea = { 0, 0, 0, 0 };

bool Game_Over = false;

int gesturesCount = 0;
char gestureStrings[MAX_GESTURE_STRINGS][32];

int currentGesture = GESTURE_NONE;
int lastGesture = GESTURE_NONE;

enum { d_left,
       d_down,
       d_up,
       d_right } Directions;

Color map_color(int val)
{
	switch (val) {
	case c_lightgray:
		return LIGHTGRAY;
	case c_dusty_purple:
		return (Color){ 160, 81, 149, 255 };
	case c_orange:
		return ORANGE;
	case c_red:
		return RED;
	case c_pink:
		return PINK;
	case c_green:
		return GREEN;
	case c_lime:
		return LIME;
	case c_babyblue:
		return (Color){ 137, 207, 240, 255 };
	case c_blue:
		return BLUE;
	case c_ocean:
		return (Color){ 0, 0, 128, 255 };
	case c_jade:
		return (Color){ 81, 160, 126, 255 };
	case c_yellow:
		return YELLOW;
	default:
		return PURPLE;
	};
}

typedef struct Pos {
	int x;
	int y;
} Pos;

typedef struct Fpos {
	float x;
	float y;
} Fpos;

typedef struct Movement {
	Fpos pos;
	Fpos start;
	Pos targ;
	Fpos targpos;
	float prog;
	bool moving;
} Movement;

typedef struct Moveable {
	bool left;
	bool right;
	bool up;
	bool down;
	bool any;
} Moveable;

//Tile gameGrid[GRID_COLS][GRID_ROWS] = { 0 };

static Pos emptyTiles[GRID_ROWS * GRID_COLS] = { 0 };
unsigned int emptyCount = GRID_ROWS * GRID_COLS;

int moveList[19];

Moveable isMoveable = { true, true, true, true };

float moveSpeed = 1.00;

void SpawnRandomTile();
void setScreenSizes();

static inline float lerp(float v0, float v1, float elapsed)
{
	return v0 + (v1 - v0) * elapsed * moveSpeed;
}
static inline float absValue(float x)
{
	return x < 0 ? -x : x;
}

// static inline float timed_lerp(float v0, float v1, float elapsed, float
// duration) {
//     float t = elapsed / duration;
//     t = fmaxf(0.0f, fminf(1.0f, t));  // Clamp
//     return v0 + (v1 - v0) * t;
// }

void initTiles()
{
	for (int x = 0; x < GRID_COLS; x++) {
		for (int y = 0; y < GRID_ROWS; y++) {
			g_gameState.gameGrid[x][y] = initTile;
		}
	}
}

void resetEmptyTiles()
{
	for (int i; i < emptyCount; i++) {
		g_gameState.gameGrid[emptyTiles[i].x][emptyTiles[i].y] = initTile;
	}
}

void getEmptyTiles()
{
	emptyCount = 0;
	memset(emptyTiles, 0, sizeof(emptyTiles));

	// [0] = number of empty, rest = indeces

	for (int y = 0; y < GRID_ROWS; y++) {
		for (int x = 0; x < GRID_COLS; x++) {
			if (g_gameState.gameGrid[x][y].val == 0) {
				emptyTiles[emptyCount].x = x;
				emptyTiles[emptyCount].y = y;
				emptyCount++;
			}
		}
	}
	resetEmptyTiles();
}

void getMovesLeft()
{
	for (int y = 0; y < GRID_ROWS; y++) {
		for (int x = 0; x < GRID_COLS; x++) {
			if (g_gameState.gameGrid[x][y].val != 0) {
				int val = g_gameState.gameGrid[x][y].val;
				if (g_gameState.gameGrid[x - 1][y].val == val && x - 1 >= 0) {
					val *= 2;
					g_gameState.gameGrid[x][y].val = 0;
					g_gameState.gameGrid[x - 1][y].val = val;
				} else if (g_gameState.gameGrid[x - 1][y].val == 0 &&
					   x - 1 >= 0) {
					g_gameState.gameGrid[x][y].val = 0;
					g_gameState.gameGrid[x - 1][y].val = val;
					y--;
				}
			}
		}
	}
}

void getMoveList(int dir)
{
	moveList[0] = 0;
	switch (dir) {
	case d_left:
		getMovesLeft();
		break;
	case d_down:
		break;
	case d_up:
		break;
	case d_right:
		break;
	}
}

void UpdateAnimations(float delta)
{
	// puts("=============================================");
	for (int y = 0; y < GRID_ROWS; y++) {
		for (int x = 0; x < GRID_COLS; x++) {
			if (g_gameState.gameGrid[x][y].anim.current == ANIMSPAWNING) {
				g_gameState.gameGrid[x][y].anim.prog += moveSpeed * 5 * delta;

				// bezier
				float t = g_gameState.gameGrid[x][y].anim.prog;
				g_gameState.gameGrid[x][y].scale = 3 * t * t - 2 * t * t * t;

				if (g_gameState.gameGrid[x][y].anim.prog >= 1.0) {
					g_gameState.gameGrid[x][y].scale = 1.0;
					g_gameState.gameGrid[x][y].anim.current = ANIMNONE;
					g_gameState.gameGrid[x][y].anim.prog = 0.0;
				}
			}
			// puts("=============================================");
			// printf("x: %i y:%i scale: %f", x, y, g_gameState.gameGrid[x][y].scale);
		}
	}
}

void DrawGameGrid(float delta)
{
	for (int y = 0; y < GRID_ROWS; y++) {
		for (int x = 0; x < GRID_COLS; x++) {
			Tile currentTile = g_gameState.gameGrid[x][y];
			int val = currentTile.val;
			if (val == 0) {
				// DrawRectangle(Gamebox_X + x * Cell_Width,
				//               Gamebox_Y + y * Cell_Height, Cell_Width,
				//               Cell_Height, map_color(val));
			} else {
				Fpos tilePos = (Fpos){ .x = Gamebox_X + x * Cell_Width,
						       .y = Gamebox_Y + y * Cell_Height };
				// printf("X1: %f\n", tilePos.x);
				// printf("Y1: %f\n", tilePos.y);

				// Fpos target;
				// target.x = Gamebox_X + x * Cell_Width;
				// target.y = Gamebox_Y + y * Cell_Height;

				// tilePos.x = lerp(tilePos.x, target.x, delta);
				// tilePos.y = lerp(tilePos.y, target.y, delta);

				// if (absValue(tilePos.x - target.x) < 0.5f ||
				//     absValue(tilePos.y - target.y) < 0.5f) {
				//     tilePos = target;
				// }
				//  tilePos.x = lerp(tilePos.x, Gamebox_X + x * Cell_Width,
				//  delta);

				// tilePos.x = Gamebox_X + x * Cell_Width;
				// printf("X2: %f\n", tilePos.x);
				// printf("Y2: %f\n", tilePos.y);
				if (currentTile.scale < 1.0) {
					DrawRectangle(
						tilePos.x +
							(Cell_Width * (1.0 - currentTile.scale)) / 2,
						tilePos.y +
							(Cell_Height * (1.0 - currentTile.scale)) / 2,
						Cell_Width * currentTile.scale,
						Cell_Height * currentTile.scale, map_color(val));

				} else {
					DrawRectangle(tilePos.x, tilePos.y, Cell_Width, Cell_Height,
						      map_color(val));

					char number[30];
					sprintf(number, "%d", val);
					int fontSize = (Cell_Width) / MAX_DIGITS;
					int textWidth = MeasureText(number, fontSize);
					int posX = x + (Cell_Width - textWidth) / 2;
					int posY = y + (Cell_Width - fontSize) / 2;
					DrawText(number, Gamebox_X + x * Cell_Width + posX,
						 Gamebox_Y + y * Cell_Height + posY, fontSize,
						 WHITE);
				}
				// g_gameState.gameGrid[x][y].mov.pos = tilePos;
			}
		}
	}
}

void MoveRight()
{
	for (int y = 0; y < GRID_ROWS; y++) {
		int writePos = GRID_COLS - 1;
		int prevVal = -1;
		for (int x = GRID_COLS - 1; x >= 0; x--) {
			if (g_gameState.gameGrid[x][y].val != 0) {
				if (prevVal == -1) {
					prevVal = g_gameState.gameGrid[x][y].val;
				} else if (prevVal == g_gameState.gameGrid[x][y].val) {
					g_gameState.gameGrid[writePos][y].val = prevVal * 2;
#ifdef SOUNDS_ON
					PlaySfxPb(SFX_MERGE, 0.1);
#endif //SOUNDS_ON
					prevVal = -1;
					writePos--;
				} else {
					g_gameState.gameGrid[writePos][y].val = prevVal;
					prevVal = g_gameState.gameGrid[x][y].val;
					writePos--;
				}
			}
		}
		if (prevVal != -1) {
			g_gameState.gameGrid[writePos][y].val = prevVal;
			writePos--;
		}
		// Fill remaining cells with 0
		while (writePos >= 0) {
			g_gameState.gameGrid[writePos][y].val = 0;
			writePos--;
		}
	}
}

void MoveLeft()
{
	for (int y = GRID_ROWS - 1; y >= 0; y--) {
		int writePos = 0;
		int prevVal = -1;
		for (int x = 0; x < GRID_COLS; x++) {
			if (g_gameState.gameGrid[x][y].val != 0) {
				if (prevVal == -1) {
					prevVal = g_gameState.gameGrid[x][y].val;
				} else if (prevVal == g_gameState.gameGrid[x][y].val) {
					g_gameState.gameGrid[writePos][y].val = prevVal * 2;
#ifdef SOUNDS_ON
					PlaySfxPb(SFX_MERGE, 0.1);
#endif //SOUNDS_ON
					// g_gameState.gameGrid[x][y].mov.targ = (Pos){writePos, y};
					// g_gameState.gameGrid[x][y].mov.moving = true;
					// g_gameState.gameGrid[x][y].mov.prog = 0.0;
					prevVal = -1;
					writePos++;
				} else {
					g_gameState.gameGrid[writePos][y].val = prevVal;
					prevVal = g_gameState.gameGrid[x][y].val;
					writePos++;
				}
			}
		}
		if (prevVal != -1) {
			g_gameState.gameGrid[writePos][y].val = prevVal;
			writePos++;
		}
		// Fill remaining cells with 0
		while (writePos < GRID_COLS) {
			g_gameState.gameGrid[writePos][y].val = 0;
			writePos++;
		}
	}
}

void MoveUp()
{
	for (int x = GRID_COLS - 1; x >= 0; x--) {
		int writePos = 0;
		int prevVal = -1;
		for (int y = 0; y < GRID_ROWS; y++) {
			if (g_gameState.gameGrid[x][y].val != 0) {
				if (prevVal == -1) {
					prevVal = g_gameState.gameGrid[x][y].val;
				} else if (prevVal == g_gameState.gameGrid[x][y].val) {
					g_gameState.gameGrid[x][writePos].val = prevVal * 2;
#ifdef SOUNDS_ON
					PlaySfxPb(SFX_MERGE, 0.1);
#endif //SOUNDS_ON
					prevVal = -1;
					writePos++;
				} else {
					g_gameState.gameGrid[x][writePos].val = prevVal;
					prevVal = g_gameState.gameGrid[x][y].val;
					writePos++;
				}
			}
		}
		if (prevVal != -1) {
			g_gameState.gameGrid[x][writePos].val = prevVal;
			writePos++;
		}
		// Fill remaining cells with 0
		while (writePos < GRID_ROWS) {
			g_gameState.gameGrid[x][writePos].val = 0;
			writePos++;
		}
	}
}

void MoveDown()
{
	for (int x = 0; x < GRID_COLS; x++) {
		int writePos = GRID_ROWS - 1;
		int prevVal = -1;
		for (int y = GRID_ROWS - 1; y >= 0; y--) {
			if (g_gameState.gameGrid[x][y].val != 0) {
				if (prevVal == -1) {
					prevVal = g_gameState.gameGrid[x][y].val;
				} else if (prevVal == g_gameState.gameGrid[x][y].val) {
					g_gameState.gameGrid[x][writePos].val = prevVal * 2;
#ifdef SOUNDS_ON
					PlaySfxPb(SFX_MERGE, 0.1);
#endif //SOUNDS_ON
					prevVal = -1;
					writePos--;
				} else {
					g_gameState.gameGrid[x][writePos].val = prevVal;
					prevVal = g_gameState.gameGrid[x][y].val;
					writePos--;
				}
			}
		}
		if (prevVal != -1) {
			g_gameState.gameGrid[x][writePos].val = prevVal;
			writePos--;
		}
		// Fill remaining cells with 0
		while (writePos >= 0) {
			g_gameState.gameGrid[x][writePos].val = 0;
			writePos--;
		}
	}
}

void getGesture()
{
	lastGesture = currentGesture;
	currentGesture = GetGestureDetected();
	touchPosition = GetTouchPosition(0);

	if (CheckCollisionPointRec(touchPosition, touchArea) &&
	    (currentGesture != GESTURE_NONE)) {
		if (currentGesture != lastGesture) {
			// Store gesture string
			switch (currentGesture) {
			case GESTURE_TAP:
				TextCopy(gestureStrings[gesturesCount], "GESTURE TAP");
				break;
			case GESTURE_DOUBLETAP:
				TextCopy(gestureStrings[gesturesCount], "GESTURE DOUBLETAP");
				break;
			case GESTURE_HOLD:
				TextCopy(gestureStrings[gesturesCount], "GESTURE HOLD");
				break;
			case GESTURE_DRAG:
				TextCopy(gestureStrings[gesturesCount], "GESTURE DRAG");
				break;
			case GESTURE_SWIPE_RIGHT:
				TextCopy(gestureStrings[gesturesCount], "GESTURE SWIPE RIGHT");
				break;
			case GESTURE_SWIPE_LEFT:
				TextCopy(gestureStrings[gesturesCount], "GESTURE SWIPE LEFT");
				break;
			case GESTURE_SWIPE_UP:
				TextCopy(gestureStrings[gesturesCount], "GESTURE SWIPE UP");
				break;
			case GESTURE_SWIPE_DOWN:
				TextCopy(gestureStrings[gesturesCount], "GESTURE SWIPE DOWN");
				break;
			case GESTURE_PINCH_IN:
				TextCopy(gestureStrings[gesturesCount], "GESTURE PINCH IN");
				break;
			case GESTURE_PINCH_OUT:
				TextCopy(gestureStrings[gesturesCount], "GESTURE PINCH OUT");
				break;
			default:
				break;
			}

			gesturesCount++;

			// Reset gestures strings
			if (gesturesCount >= MAX_GESTURE_STRINGS) {
				for (int i = 0; i < MAX_GESTURE_STRINGS; i++)
					TextCopy(gestureStrings[i], "\0");

				gesturesCount = 0;
			}
		}
	}
}

bool leftInput()
{
	return IsKeyPressed(KEY_H) || IsKeyPressed(KEY_LEFT) ||
	       IsKeyPressed(KEY_A) || currentGesture == GESTURE_SWIPE_LEFT;
}
bool rightInput()
{
	return IsKeyPressed(KEY_L) || IsKeyPressed(KEY_RIGHT) ||
	       IsKeyPressed(KEY_D) || currentGesture == GESTURE_SWIPE_RIGHT;
}
bool upInput()
{
	return IsKeyPressed(KEY_K) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) ||
	       currentGesture == GESTURE_SWIPE_UP;
}
bool downInput()
{
	return IsKeyPressed(KEY_J) || IsKeyPressed(KEY_DOWN) ||
	       IsKeyPressed(KEY_S) || currentGesture == GESTURE_SWIPE_DOWN;
}

void autoMovement()
{
	// printf("Automove: %i\n", autoMove);
	if (isMoveable.left) {
		MoveLeft();
	} else if (isMoveable.down) {
		MoveDown();
	}
	SpawnRandomTile();
}

bool moveableUp()
{
	for (int x = 0; x < GRID_COLS; x++) {
		int prevVal = g_gameState.gameGrid[x][0].val;
		for (int y = 1; y < GRID_ROWS; y++) {
			int cellVal = g_gameState.gameGrid[x][y].val;
			if (cellVal == 0) {
				prevVal = 0;
				continue;
			}
			if (prevVal == 0 || prevVal == cellVal) {
				return true;
			}
			prevVal = cellVal;
		}
	}
	return false;
}
bool moveableDown()
{
	for (int x = 0; x < GRID_COLS; x++) {
		int prevVal = g_gameState.gameGrid[x][GRID_ROWS - 1].val;
		for (int y = GRID_ROWS - 2; y >= 0; y--) {
			int cellVal = g_gameState.gameGrid[x][y].val;
			if (cellVal == 0) {
				prevVal = 0;
				continue;
			}
			if (prevVal == 0 || prevVal == cellVal) {
				return true;
			}
			prevVal = cellVal;
		}
	}
	return false;
}
bool moveableLeft()
{
	for (int y = 0; y < GRID_ROWS; y++) {
		int prevVal = g_gameState.gameGrid[0][y].val;
		for (int x = 1; x < GRID_COLS; x++) {
			int cellVal = g_gameState.gameGrid[x][y].val;
			if (cellVal == 0) {
				prevVal = 0;
				continue;
			}
			if (prevVal == 0 || prevVal == cellVal) {
				return true;
			}
			prevVal = cellVal;
		}
	}
	return false;
}
bool moveableRight()
{
	for (int y = 0; y < GRID_ROWS; y++) {
		int prevVal = g_gameState.gameGrid[GRID_COLS - 1][y].val;
		for (int x = GRID_COLS - 2; x >= 0; x--) {
			int cellVal = g_gameState.gameGrid[x][y].val;
			if (cellVal == 0) {
				prevVal = 0;
				continue;
			}
			if (prevVal == 0 || prevVal == cellVal) {
				return true;
			}
			prevVal = cellVal;
		}
	}
	return false;
}

void processMoveable()
{
	isMoveable.up = moveableUp();
	isMoveable.down = moveableDown();
	isMoveable.left = moveableLeft();
	isMoveable.right = moveableRight();
	isMoveable.any = isMoveable.up || isMoveable.down || isMoveable.left ||
			 isMoveable.right;

	/*
    printf("moveableUp: %i\n", isMoveable.up);
    printf("moveableDown: %i\n", isMoveable.down);
    printf("moveableLeft: %i\n", isMoveable.left);
    printf("moveableRight: %i\n", isMoveable.right);
    printf("moveableany: %i\n", isMoveable.any);
    */
}

void processInput()
{
	getGesture();

	bool somethingMoved = false;
	if (isMoveable.any) {
		// PrepareMoveAnimation();
		if (rightInput() && isMoveable.right) {
			MoveRight();
			somethingMoved = true;
		} else if (leftInput() && isMoveable.left) {
			MoveLeft();
			somethingMoved = true;
		} else if (upInput() && isMoveable.up) {
			MoveUp();
			somethingMoved = true;
		} else if (downInput() && isMoveable.down) {
			MoveDown();
			somethingMoved = true;
		}
	}
	if (IsKeyPressed(KEY_F12)) {
		setScreenSizes();
	}

	if (IsKeyPressed(KEY_O)) {
		loadReload(&g_gameState);
	}
	if (IsKeyPressed(KEY_I) || IsKeyPressed(KEY_VOLUME_UP)) {
		saveForReload(&g_gameState);
	}

	if (somethingMoved) {
		//PlaySfxPb(SFX_MOVE, 0.1);
		SpawnRandomTile();
	}
	if (IsKeyDown(KEY_VOLUME_DOWN) || IsKeyDown(KEY_P)) {
		autoMovement();
	}
}

unsigned int getScore()
{
	unsigned int score = 0;
	for (int x = 0; x < GRID_COLS; x++) {
		for (int y = 0; y < GRID_ROWS; y++) {
			score += g_gameState.gameGrid[x][y].val;
		}
	}
	return score;
}

void drawScore()
{
	int gameScore = getScore();
	char scoreText[30];

	sprintf(scoreText, "SCORE: %d", gameScore);

	// int fontSize = Gamebox_Y - (Gamebox_Y / 2);
	int fontSize = 40;
	int textWidth = MeasureText(scoreText, fontSize);
	int posX = (Screen_Width - textWidth) / 2;
	int posY = (Gamebox_Y - fontSize) / 2;
	DrawText(scoreText, posX, posY, fontSize, WHITE);
}

void SpawnRandomTile()
{
	getEmptyTiles();
	// printf("Empty: %i\n", emptyCount);
	if (emptyCount == 0) {
		return;
	}
	int cellIndex = GetRandomValue(0, emptyCount - 1);

	int cellValue = 2;
	if (GetRandomValue(1, 10) == 10) {
		cellValue = 4;
	}

	unsigned int posX = emptyTiles[cellIndex].x;
	unsigned int posY = emptyTiles[cellIndex].y;
	g_gameState.gameGrid[posX][posY].val = cellValue;
	g_gameState.gameGrid[posX][posY].anim.current = ANIMSPAWNING;
}

void setScreenSizes()
{
	Screen_Width = GetScreenWidth();
	Screen_Height = GetScreenHeight();
	printf("ScreenWidth1: %i\n", Screen_Width);

	Gamebox_Width = Screen_Width - Screen_Width / 10;
	Gamebox_Height = Screen_Height - Screen_Height / 10;
	printf("BoxWidth1: %i\n", Gamebox_Width);

	if (Gamebox_Width > Gamebox_Height) {
		Gamebox_Width = Gamebox_Height;
	} else {
		Gamebox_Height = Gamebox_Width;
	}
	printf("BoxWidth2: %i\n", Gamebox_Width);

	Cell_Width = Gamebox_Width / GRID_COLS;
	Cell_Height = Gamebox_Height / GRID_ROWS;

	Gamebox_X = Screen_Width / 2 - Gamebox_Width / 2;
	Gamebox_Y = Screen_Height / 2 - Gamebox_Height / 2;

	// touchArea.x = 220;
	// touchArea.y = 10;
	// touchArea.width = Screen_Width - 230.0f;
	// touchArea.height = Screen_Height - 20.0f;
	touchArea =
		(Rectangle){ Gamebox_X, Gamebox_Y, Gamebox_Width, Gamebox_Height };
}

void drawAllTiles()
{
	for (int x = 0; x < GRID_COLS; x++) {
		for (int y = 0; y < GRID_ROWS; y++) {
			g_gameState.gameGrid[x][y].val = powerOfTwo(x + y * GRID_COLS);
		}
	}
}

void resetGame()
{
	Game_Over = false;
	initTiles();
	SpawnRandomTile();
	SpawnRandomTile();
}

void handleGameOver(bool update)
{
	static int finalScore = 0;
	static int highScore = 0;

	if (update) {
		finalScore = getScore();
		highScore = LoadHighScore();

	} else {
		char *gameOverText = "You Lose!";
		int fontSize = Gamebox_Height / 5;
		int textWidth = MeasureText(gameOverText, fontSize);
		int posX = (Screen_Width - textWidth) / 2;
		int posY = (Screen_Height - fontSize) / 2;
		DrawText(gameOverText, posX, posY, fontSize, WHITE);

		char highScoreText[256];
		snprintf(highScoreText, sizeof(highScoreText), "Hi: %i\n", highScore);
		// snprintf(hiBuf*sizeof(char),
		//          sizeof(highScoreText) - strlen(highScoreText), "Hi: %i",
		//          highScore);
		int hi_fontSize = Gamebox_Height / 10;
		int hi_textWidth = MeasureText(highScoreText, hi_fontSize);
		int hi_posX = (Screen_Width - hi_textWidth) / 2;
		int hi_posY = (Gamebox_Height + Gamebox_Y + Screen_Height / 32);
		DrawText(highScoreText, hi_posX, hi_posY, hi_fontSize, WHITE);

		if (finalScore > highScore) {
			SaveHighScore(finalScore);
		}

		if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_DOUBLETAP)) {
			resetGame();
		}
	}
}

void processGameOver()
{
	if (!isMoveable.any && Game_Over == false) {
		Game_Over = true;
		handleGameOver(true);
	}
}

void initState()
{
	g_gameState.grid_rows = GRID_ROWS;
	g_gameState.grid_cols = GRID_COLS;
}
void stopCallback()
{
	TraceLog(LOG_INFO, "Stopped");
	if (!saveForReload(&g_gameState)) {
		TraceLog(LOG_ERROR, "Couldn't saveForReload");
	}
}
void pauseCallback()
{
	TraceLog(LOG_INFO, "Paused");
	if (!saveForReload(&g_gameState)) {
		TraceLog(LOG_ERROR, "Couldn't saveForReload");
	}
}
int main()
{
	setScreenSizes(); // init to something
	InitWindow(0, 0, "2048");
	TraceLog(LOG_INFO, "Time: %f", GetTime());

	//SetRandomSeed(randomSeed); // InitWindow() already sets random seed

	InitAudioDevice();

#ifdef SOUNDS_ON
	LoadAllSounds();
#endif //SOUNDS_ON

	setScreenSizes(); // set actual values

	SetTargetFPS(60);

	// drawAllTiles();

	if (!g_gameState.valid) {
		initTiles();
		initState();
	}
	InitStorage();

	SpawnRandomTile();
	SpawnRandomTile();

	InitCallBacks();
	loadReload(&g_gameState);
	SetOnStopCallBack(&stopCallback);
	SetOnPauseCallBack(&pauseCallback);

	while (!WindowShouldClose()) {
		float delta = GetFrameTime();
		// printf("delta: %f\n", delta);

		processMoveable();

		processInput();

		// autoMovement();

		processGameOver();

		UpdateAnimations(delta);

		BeginDrawing();

		ClearBackground(DARKGRAY);
		DrawRectangle(Gamebox_X, Gamebox_Y, Cell_Width * GRID_COLS,
			      Cell_Height * GRID_ROWS, map_color(0));

		DrawText(TextFormat("FPS: %i", (int)(1.0f / delta)), 10, 10, 20, WHITE);
		DrawGameGrid(delta);

		drawScore();

		if (Game_Over) {
			handleGameOver(false);
		}

		EndDrawing();
	}

#ifdef SOUNDS_ON
	UnloadAllSounds();
#endif //SOUNDS_ON
	CloseAudioDevice();
	CloseWindow();
}
