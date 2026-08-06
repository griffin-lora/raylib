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
        
        EndShaderMode();

        rlBegin(RL_TRIANGLES);
        
        rlColor3f(1.0f, 0.0f, 0.0f);
        rlVertex2f(50.0f, 50.0f + height);
        
        rlColor3f(0.0f, 1.0f, 0.0f);
        rlVertex2f(50.0f + width, 50.0f + height);
        
        rlColor3f(0.0f, 0.0f, 1.0f);
        rlVertex2f(50.0f + 0.5f * width, 50.0f + (1.0f - 0.866025403784f) * height);

        rlEnd();

        DrawRectangle(0, 0, 100, 100, CLITERAL(Color){ 255, 0, 0, 255 });
        EndDrawing();
    }

    CloseWindow();

    return 0;
}