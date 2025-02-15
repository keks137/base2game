#include <raylib.h>

int main() {
  InitWindow(400, 200, "hi");
  while (!WindowShouldClose()) {
    BeginDrawing();

    ClearBackground(RAYWHITE);

    DrawText("Congrats! You created your first window!", 190, 200, 20,
             LIGHTGRAY);

    EndDrawing();
  }
  CloseWindow();
}
