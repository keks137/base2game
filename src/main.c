#include "constants.h"
#include "funny_math.h"
#include <memory.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
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
Vector2 touchPosition = {0, 0};
Rectangle touchArea = {0, 0, 0, 0};

bool Game_Over = false;

int gesturesCount = 0;
char gestureStrings[MAX_GESTURE_STRINGS][32];

int currentGesture = GESTURE_NONE;
int lastGesture = GESTURE_NONE;

enum { d_left, d_down, d_up, d_right } Directions;

Color map_color(int power) {
    switch (power) {
    case c_lightgray:
        return LIGHTGRAY;
    case c_dusty_purple:
        return (Color){160, 81, 149, 255};
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
        return (Color){137, 207, 240, 255};
    case c_blue:
        return BLUE;
    case c_ocean:
        return (Color){0, 0, 128, 255};
    case c_jade:
        return (Color){81, 160, 126, 255};
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

typedef struct Anim {
    int dx;
    int dy;
    float t;
} Anim;

typedef struct Tile {
    int val;
    Anim *anim;
    bool merged;
} Tile;

Tile gameGrid[GRID_COLS][GRID_ROWS] = {0};
static Pos emptyTiles[GRID_ROWS * GRID_COLS] = {0};
unsigned int emptyCount = GRID_ROWS * GRID_COLS;

int moveList[19];

bool somethingMoved = false;

float moveSpeed = 1.0;

void SpawnRandomTile();
void setScreenSizes();

static inline float lerp(float v0, float v1, int elapsed) {
    return (v0 - v1) * elapsed * moveSpeed;
}

void getEmptyTiles() {
    emptyCount = 0;
    memset(emptyTiles, 0, sizeof(emptyTiles));

    // [0] = number of empty, rest = indeces

    for (int y = 0; y < GRID_ROWS; y++) {
        for (int x = 0; x < GRID_COLS; x++) {
            if (gameGrid[x][y].val == 0) {
                emptyTiles[emptyCount].x = x;
                emptyTiles[emptyCount].y = y;
                emptyCount++;
            }
        }
    }
}

void getMovesLeft() {
    for (int y = 0; y < GRID_ROWS; y++) {
        for (int x = 0; x < GRID_COLS; x++) {
            if (gameGrid[x][y].val != 0) {
                int val = gameGrid[x][y].val;
                if (gameGrid[x - 1][y].val == val && x - 1 >= 0) {
                    val *= 2;
                    gameGrid[x][y].val = 0;
                    gameGrid[x - 1][y].val = val;
                } else if (gameGrid[x - 1][y].val == 0 && x - 1 >= 0) {
                    gameGrid[x][y].val = 0;
                    gameGrid[x - 1][y].val = val;
                    y--;
                }
            }
        }
    }
}

void getMoveList(int dir) {
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

void DrawGameGrid() {
    for (int y = 0; y < GRID_ROWS; y++) {
        for (int x = 0; x < GRID_COLS; x++) {
            int val = gameGrid[x][y].val;
            DrawRectangle(Gamebox_X + x * Cell_Width,
                          Gamebox_Y + y * Cell_Height, Cell_Width, Cell_Height,
                          map_color(val));
            char number[30];

            sprintf(number, "%d", val);
            if (*number == '0') {

            } else {
                int fontSize = (Cell_Width) / MAX_DIGITS;
                int textWidth = MeasureText(number, fontSize);
                int posX = x + (Cell_Width - textWidth) / 2;
                int posY = y + (Cell_Width - fontSize) / 2;
                DrawText(number, Gamebox_X + x * Cell_Width + posX,
                         Gamebox_Y + y * Cell_Height + posY, fontSize, WHITE);
            }
        }
    }
}

void MoveRight() {
    for (int y = 0; y < GRID_ROWS; y++) {
        int writePos = GRID_COLS - 1;
        int prevVal = -1;
        for (int x = GRID_COLS - 1; x >= 0; x--) {
            if (gameGrid[x][y].val != 0) {
                if (prevVal == -1) {
                    prevVal = gameGrid[x][y].val;
                } else if (prevVal == gameGrid[x][y].val) {
                    gameGrid[writePos][y].val = prevVal * 2;
                    prevVal = -1;
                    writePos--;
                    somethingMoved = true;
                } else {
                    gameGrid[writePos][y].val = prevVal;
                    prevVal = gameGrid[x][y].val;
                    writePos--;
                    somethingMoved = true;
                }
            }
        }
        if (prevVal != -1) {
            gameGrid[writePos][y].val = prevVal;
            writePos--;
            somethingMoved = true;
        }
        // Fill remaining cells with 0
        while (writePos >= 0) {
            gameGrid[writePos][y].val = 0;
            writePos--;
        }
    }
}

void MoveLeft() {
    for (int y = GRID_ROWS - 1; y >= 0; y--) {
        int writePos = 0;
        int prevVal = -1;
        for (int x = 0; x < GRID_COLS; x++) {
            if (gameGrid[x][y].val != 0) {
                if (prevVal == -1) {
                    prevVal = gameGrid[x][y].val;
                } else if (prevVal == gameGrid[x][y].val) {
                    gameGrid[writePos][y].val = prevVal * 2;
                    prevVal = -1;
                    writePos++;
                    somethingMoved = true;
                } else {
                    gameGrid[writePos][y].val = prevVal;
                    prevVal = gameGrid[x][y].val;
                    writePos++;
                    somethingMoved = true;
                }
            }
        }
        if (prevVal != -1) {
            gameGrid[writePos][y].val = prevVal;
            writePos++;
            somethingMoved = true;
        }
        // Fill remaining cells with 0
        while (writePos < GRID_ROWS) {
            gameGrid[writePos][y].val = 0;
            writePos++;
        }
    }
}

void MoveUp() {
    for (int x = GRID_COLS - 1; x >= 0; x--) {
        int writePos = 0;
        int prevVal = -1;
        for (int y = 0; y < GRID_ROWS; y++) {
            if (gameGrid[x][y].val != 0) {
                if (prevVal == -1) {
                    prevVal = gameGrid[x][y].val;
                } else if (prevVal == gameGrid[x][y].val) {
                    gameGrid[x][writePos].val = prevVal * 2;
                    prevVal = -1;
                    writePos++;
                    somethingMoved = true;
                } else {
                    gameGrid[x][writePos].val = prevVal;
                    prevVal = gameGrid[x][y].val;
                    writePos++;
                    somethingMoved = true;
                }
            }
        }
        if (prevVal != -1) {
            gameGrid[x][writePos].val = prevVal;
            writePos++;
            somethingMoved = true;
        }
        // Fill remaining cells with 0
        while (writePos < GRID_ROWS) {
            gameGrid[x][writePos].val = 0;
            writePos++;
        }
    }
}

void MoveDown() {
    for (int x = 0; x < GRID_COLS; x++) {
        int writePos = GRID_ROWS - 1;
        int prevVal = -1;
        for (int y = GRID_ROWS - 1; y >= 0; y--) {
            if (gameGrid[x][y].val != 0) {
                if (prevVal == -1) {
                    prevVal = gameGrid[x][y].val;
                } else if (prevVal == gameGrid[x][y].val) {
                    gameGrid[x][writePos].val = prevVal * 2;
                    prevVal = -1;
                    writePos--;
                    somethingMoved = true;
                } else {
                    gameGrid[x][writePos].val = prevVal;
                    prevVal = gameGrid[x][y].val;
                    writePos--;
                    somethingMoved = true;
                }
            }
        }
        if (prevVal != -1) {
            gameGrid[x][writePos].val = prevVal;
            writePos--;
            somethingMoved = true;
        }
        // Fill remaining cells with 0
        while (writePos >= 0) {
            gameGrid[x][writePos].val = 0;
            writePos--;
        }
    }
}

void getGesture() {
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

bool leftInput() {
    return IsKeyPressed(KEY_H) || IsKeyPressed(KEY_LEFT) ||
           IsKeyPressed(KEY_A) || currentGesture == GESTURE_SWIPE_LEFT;
}
bool rightInput() {
    return IsKeyPressed(KEY_L) || IsKeyPressed(KEY_RIGHT) ||
           IsKeyPressed(KEY_D) || currentGesture == GESTURE_SWIPE_RIGHT;
}
bool upInput() {
    return IsKeyPressed(KEY_K) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) ||
           currentGesture == GESTURE_SWIPE_UP;
}
bool downInput() {
    return IsKeyPressed(KEY_J) || IsKeyPressed(KEY_DOWN) ||
           IsKeyPressed(KEY_S) || currentGesture == GESTURE_SWIPE_DOWN;
}

void ProcessInput() {
    getGesture();

    somethingMoved = false;
    if (rightInput()) {
        MoveRight();
    } else if (leftInput()) {
        MoveLeft();
    } else if (upInput()) {
        MoveUp();
    } else if (downInput()) {
        MoveDown();
    } else if (IsKeyPressed(KEY_F12)) {
        setScreenSizes();
    }
    if (somethingMoved) {
        SpawnRandomTile();
    }
}

void SpawnRandomTile() {
    // printf("Empty: %i\n", emptyCount);
    if (emptyCount == 0) {
        return;
    }
    int cellIndex = GetRandomValue(0, emptyCount - 1);

    int cellValue = 2;
    if (GetRandomValue(1, 10) == 10) {
        cellValue = 4;
    }

    gameGrid[emptyTiles[cellIndex].x][emptyTiles[cellIndex].y].val = cellValue;
}

void setScreenSizes() {
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

    touchArea.x = 220;
    touchArea.y = 10;
    touchArea.width = Screen_Width - 230.0f;
    touchArea.height = Screen_Height - 20.0f;
}

void drawAllTiles() {
    for (int x = 0; x < GRID_COLS; x++) {
        for (int y = 0; y < GRID_ROWS; y++) {
            gameGrid[x][y].val = powerOfTwo(x + y * GRID_COLS);
        }
    }
}

void processGameOver() {
    if (emptyCount == 0) {
        Game_Over = true;
    }
}

int main() {
    // randomSeed = time(NULL);  // not need for some reason???
    // printf("seed: %i\n", randomSeed);
    SetRandomSeed(randomSeed);

    setScreenSizes(); // init to something
    InitWindow(Screen_Width, Screen_Height, "2048");
    setScreenSizes(); // set actual values

    SetTargetFPS(60);

    // drawAllTiles();

    SpawnRandomTile();
    SpawnRandomTile();

    while (!WindowShouldClose()) {
        float delta = GetFrameTime();

        getEmptyTiles();
        ProcessInput();
        processGameOver();
        BeginDrawing();

        ClearBackground(DARKGRAY);

        DrawText(TextFormat("FPS: %i", (int)(1.0f / delta)), 10, 10, 20, WHITE);
        DrawGameGrid();

        if (Game_Over) {

            char *gameOverText = "You Lose!";
            int fontSize = Gamebox_Height / 5;
            int textWidth = MeasureText(gameOverText, fontSize);
            int posX = (Screen_Width - textWidth) / 2;
            int posY = (Screen_Height - fontSize) / 2;
            DrawText(gameOverText, posX, posY, fontSize, WHITE);
        }

        EndDrawing();
    }

    CloseWindow();
}
