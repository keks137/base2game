#include "constants.h"
#include <memory.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>

enum {
    c_lightgray = 0,
    c_orange = 2,
    c_pink = 4,
    c_green = 8,
    c_lime = 16,
    c_babyblue = 32,
    c_blue = 64,
} Colors;

#define MAX_GESTURE_STRINGS 20

Vector2 touchPosition = {0, 0};
Rectangle touchArea = {220, 10, SCREEN_WIDTH - 230.0f, SCREEN_HEIGHT - 20.0f};

int gesturesCount = 0;
char gestureStrings[MAX_GESTURE_STRINGS][32];

int currentGesture = GESTURE_NONE;
int lastGesture = GESTURE_NONE;

enum { d_left, d_down, d_up, d_right } Directions;

Color map_color(int power) {
    switch (power) {
    case c_lightgray:
        return LIGHTGRAY;
    case c_orange:
        return ORANGE;
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
} Tile;

Tile gameGrid[GRID_COLS][GRID_ROWS] = {0};
static Pos emptyTiles[GRID_ROWS + GRID_COLS] = {0};

int moveList[19];

float moveSpeed = 1.0;

void SpawnRandomTile();

static inline float lerp(float v0, float v1, int elapsed) {
    return (v0 - v1) * elapsed * moveSpeed;
}

int getEmptyTiles() {
    int emptyCount = 0;
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
    return emptyCount;
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
            DrawRectangle(GAMEBOX_X + x * CELL_WIDTH,
                          GAMEBOX_Y + y * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT,
                          map_color(gameGrid[x][y].val));
            char number[30];

            sprintf(number, "%d", gameGrid[x][y].val);
            if (*number == '0') {

            } else {
                DrawText(number, GAMEBOX_X + x * CELL_WIDTH + CELL_WIDTH / 2,
                         GAMEBOX_Y + y * CELL_HEIGHT + CELL_HEIGHT / 2, 20,
                         BLACK);
            }
        }
    }
}

bool somethingMoved = false;
void MoveRight() {
    for (int y = 0; y < GRID_ROWS; y++) {
        for (int x = GRID_COLS; x >= 0; x--) {
            if (gameGrid[x][y].val != 0) {
                int val = gameGrid[x][y].val;
                if (gameGrid[x + 1][y].val == val && x + 1 < GRID_COLS) {
                    val *= 2;
                    gameGrid[x][y].val = 0;
                    gameGrid[x + 1][y].val = val;
                    somethingMoved = true;
                } else if (gameGrid[x + 1][y].val == 0 && x + 1 < GRID_COLS) {
                    gameGrid[x][y].val = 0;
                    gameGrid[x + 1][y].val = val;
                    somethingMoved = true;
                    y--;
                }
            }
        }
    }
}
void MoveLeft() {
    for (int y = 0; y < GRID_ROWS; y++) {
        for (int x = 0; x < GRID_COLS; x++) {
            if (gameGrid[x][y].val != 0) {
                int val = gameGrid[x][y].val;
                if (gameGrid[x - 1][y].val == val && x - 1 >= 0) {
                    val *= 2;
                    gameGrid[x][y].val = 0;
                    gameGrid[x - 1][y].val = val;
                    somethingMoved = true;
                } else if (gameGrid[x - 1][y].val == 0 && x - 1 >= 0) {
                    gameGrid[x][y].val = 0;
                    gameGrid[x - 1][y].val = val;
                    somethingMoved = true;
                    y--;
                }
            }
        }
    }
}
void MoveUp() {
    for (int x = 0; x < GRID_COLS; x++) {
        for (int y = 0; y < GRID_ROWS; y++) {
            if (gameGrid[x][y].val != 0) {
                int val = gameGrid[x][y].val;
                if (gameGrid[x][y - 1].val == val && y - 1 >= 0) {
                    val *= 2;
                    gameGrid[x][y].val = 0;
                    gameGrid[x][y - 1].val = val;
                    somethingMoved = true;
                } else if (gameGrid[x][y - 1].val == 0 && y - 1 >= 0) {
                    gameGrid[x][y].val = 0;
                    gameGrid[x][y - 1].val = val;
                    somethingMoved = true;
                    x--;
                }
            }
        }
    }
}
void MoveDown() {
    for (int x = 0; x < GRID_COLS; x++) {
        for (int y = GRID_ROWS; y >= 0; y--) {
            if (gameGrid[x][y].val != 0) {
                int val = gameGrid[x][y].val;
                if (gameGrid[x][y + 1].val == val && y + 1 < GRID_ROWS) {
                    val *= 2;
                    gameGrid[x][y].val = 0;
                    gameGrid[x][y + 1].val = val;
                    somethingMoved = true;
                } else if (gameGrid[x][y + 1].val == 0 && y + 1 < GRID_ROWS) {
                    gameGrid[x][y].val = 0;
                    gameGrid[x][y + 1].val = val;
                    somethingMoved = true;
                    x--;
                }
            }
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
    }
    if (leftInput()) {
        MoveLeft();
    }
    if (upInput()) {
        MoveUp();
    }
    if (downInput()) {
        MoveDown();
    }
    if (somethingMoved) {
        SpawnRandomTile();
    }
}

void SpawnRandomTile() {
    int emptyCount = getEmptyTiles();
    if (emptyCount == 0) {
        return;
    }
    int cellIndex = GetRandomValue(0, emptyCount);

    int cellValue = 1;
    if (GetRandomValue(1, 10) == 10) {
        cellValue = 2;
    }

    gameGrid[emptyTiles[cellIndex].x][emptyTiles[cellIndex].y].val = cellValue;
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "2048");

    SetTargetFPS(60);

    SetRandomSeed(47060);

    SpawnRandomTile();
    SpawnRandomTile();
    SpawnRandomTile();
    SpawnRandomTile();
    SpawnRandomTile();
    SpawnRandomTile();
    SpawnRandomTile();
    SpawnRandomTile();

    while (!WindowShouldClose()) {
        float delta = GetFrameTime();

        ProcessInput();
        BeginDrawing();

        ClearBackground(DARKGRAY);

        DrawText(TextFormat("FPS: %i", (int)(1.0f / delta)), 10, 10, 20, BLACK);
        DrawGameGrid();

        EndDrawing();
    }

    CloseWindow();
}
