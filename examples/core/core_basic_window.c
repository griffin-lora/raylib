#include <stddef.h>
#include <raylib.h>
#include <rlvk.h>

int main() {
    float width = 1500.0f;
    float height = 1500.0f;

    InitWindow(width + 100.0f, height + 100.0f, "raylib [core] example - basic window");

    Shader shader = LoadShader("examples/core/resources/vertex.glsl", "examples/core/resources/fragment.glsl");

    SetTargetFPS(60);

    float t = 0.0f;

    while (!WindowShouldClose()) {
        t += 0.01f;

        BeginDrawing();

        BeginShaderMode(shader);

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

        DrawTriangle((Vector2){430, 80},
                        (Vector2){430 - 60, 150},
                        (Vector2){430 + 60, 150}, VIOLET);

        EndDrawing();
    }

    UnloadShader(shader);

    CloseWindow();

    return 0;
}