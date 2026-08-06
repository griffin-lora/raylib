#include <stddef.h>
#include <raylib.h>
#include <rlvk.h>

int main() {
    float width = 1500.0f;
    float height = 1500.0f;

    InitWindow(width + 100.0f, height + 100.0f, "raylib [core] example - basic window");

    SetTargetFPS(60);

    float t = 0.0f;

    while (!WindowShouldClose()) {
        t += 0.01f;

        BeginDrawing();
        ClearBackground(RED);

        DrawRectangle(0, 0, 100, 100, BLUE);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}