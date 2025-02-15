#include "constants.h"
#include <raylib.h>

Color map_color(int power) {
  switch (power) {
  case 1:
    return ORANGE;
  default:
    return LIGHTGRAY;
  };
}

int gameGrid[GRID_COLS][GRID_ROWS] = {0};

void DrawGameGrid() {
  for (int y = 0; y < GRID_ROWS; y++) {
    for (int x = 0; x < GRID_COLS; x++) {
      DrawRectangle(GAMEBOX_X + x * CELL_WIDTH, GAMEBOX_Y + y * CELL_HEIGHT,
                    CELL_WIDTH, CELL_HEIGHT, map_color(gameGrid[x][y]));
    }
  }
}

int main() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "2048");

  SetTargetFPS(60);

  gameGrid[0][2] = 1;

  while (!WindowShouldClose()) {
    float delta = GetFrameTime();
    BeginDrawing();

    ClearBackground(DARKGRAY);

    DrawText(TextFormat("FPS: %i", (int)(1.0f / delta)), 10, 10, 20, BLACK);
    DrawGameGrid();

    EndDrawing();
  }
  CloseWindow();
}
