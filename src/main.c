#include "constants.h"
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

int gameGrid[GRID_COLS][GRID_ROWS] = {0};

void SpawnRandomTile();

void DrawGameGrid() {
  for (int y = 0; y < GRID_ROWS; y++) {
    for (int x = 0; x < GRID_COLS; x++) {
      DrawRectangle(GAMEBOX_X + x * CELL_WIDTH, GAMEBOX_Y + y * CELL_HEIGHT,
                    CELL_WIDTH, CELL_HEIGHT, map_color(gameGrid[x][y]));
      char number[30];

      sprintf(number, "%d", gameGrid[x][y]);
      DrawText(number, GAMEBOX_X + x * CELL_WIDTH + CELL_WIDTH/2, GAMEBOX_Y + y * CELL_HEIGHT + CELL_HEIGHT/2,
               20, BLACK);
    }
  }
}

bool somethingMoved = false;
void MoveRight() {
  for (int y = 0; y < GRID_ROWS; y++) {
    for (int x = GRID_COLS; x >= 0; x--) {
      if (gameGrid[x][y] != 0) {
        int val = gameGrid[x][y];
        if (gameGrid[x + 1][y] == val && x + 1 < GRID_COLS) {
          val *= 2;
          gameGrid[x][y] = 0;
          gameGrid[x + 1][y] = val;
          somethingMoved = true;
        } else if (gameGrid[x + 1][y] == 0 && x + 1 < GRID_COLS) {
          gameGrid[x][y] = 0;
          gameGrid[x + 1][y] = val;
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
      if (gameGrid[x][y] != 0) {
        int val = gameGrid[x][y];
        if (gameGrid[x - 1][y] == val && x - 1 >= 0) {
          val *= 2;
          gameGrid[x][y] = 0;
          gameGrid[x - 1][y] = val;
          somethingMoved = true;
        } else if (gameGrid[x - 1][y] == 0 && x - 1 >= 0) {
          gameGrid[x][y] = 0;
          gameGrid[x - 1][y] = val;
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
      if (gameGrid[x][y] != 0) {
        int val = gameGrid[x][y];
        if (gameGrid[x][y - 1] == val && y - 1 >= 0) {
          val *= 2;
          gameGrid[x][y] = 0;
          gameGrid[x][y - 1] = val;
          somethingMoved = true;
        } else if (gameGrid[x][y - 1] == 0 && y - 1 >= 0) {
          gameGrid[x][y] = 0;
          gameGrid[x][y - 1] = val;
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
      if (gameGrid[x][y] != 0) {
        int val = gameGrid[x][y];
        if (gameGrid[x][y + 1] == val && y + 1 < GRID_ROWS) {
          val *= 2;
          gameGrid[x][y] = 0;
          gameGrid[x][y + 1] = val;
          somethingMoved = true;
        } else if (gameGrid[x][y + 1] == 0 && y + 1 < GRID_ROWS) {
          gameGrid[x][y] = 0;
          gameGrid[x][y + 1] = val;
          somethingMoved = true;
          x--;
        }
      }
    }
  }
}

void ProcessInput() {
  somethingMoved = false;
  if (IsKeyPressed(KEY_L) || IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
    MoveRight();
  }
  if (IsKeyPressed(KEY_H) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
    MoveLeft();
  }
  if (IsKeyPressed(KEY_K) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
    MoveUp();
  }
  if (IsKeyPressed(KEY_J) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
    MoveDown();
  }
  if (somethingMoved) {
    SpawnRandomTile();
  }
}

void SpawnRandomTile() {
  int x = GetRandomValue(0, GRID_COLS);
  int y = GetRandomValue(0, GRID_ROWS);

  if (gameGrid[x][y] == 0) {
    // TODO: actual random

    gameGrid[x][y] = GetRandomValue(2, 2);
  }
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
