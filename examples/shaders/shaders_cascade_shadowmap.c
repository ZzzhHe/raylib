/*******************************************************************************************
*
*   raylib [shaders] example - Cascade Shadow Mapping
*
*   Example originally created with raylib 4.5, last time updated with raylib 4.5
*
*   Example contributed by <user_name> (@<user_github>) and reviewed by Ramon Santamaria (@raysan5)
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2023 <user_name> (@<user_github>)
*
********************************************************************************************/

#include "raylib.h"

#include "raymath.h"
#include "rlgl.h"

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"


// #if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
// #else   // PLATFORM_ANDROID, PLATFORM_WEB
    // #define GLSL_VERSION            100
// #endif

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [shaders] example - Cascade Shadow Mapping");

    Camera3D camera = (Camera3D){ 0 };
    camera.position = (Vector3){ 0.0f, 10.0f, 10.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Shader shader = LoadShader(TextFormat("resources/shaders/glsl%i/csm.vs", GLSL_VERSION)
                            , TextFormat("resources/shaders/glsl%i/csm.fs", GLSL_VERSION));
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

    // directional light
    Light light = CreateLight(LIGHT_DIRECTIONAL, (Vector3){ 10.0f, 10.0f, 0.0f }, Vector3Zero(), WHITE, shader);

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        Vector3 viewPos = camera.position;
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], &viewPos, SHADER_UNIFORM_VEC3);
        UpdateCamera(&camera, CAMERA_ORBITAL);

        UpdateLightValues(shader, light);

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);

                BeginShaderMode(shader);

                    DrawPlane((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector2){ 20.0f, 20.0f }, WHITE);
                    DrawCube((Vector3){ 5.0f, 0.0f, 5.0f }, 2.0f, 2.0f, 2.0f, BLUE);
                    DrawCube((Vector3){ 5.0f, 0.0f, -5.0f }, 2.0f, 2.0f, 2.0f, YELLOW);
                    DrawCube((Vector3){ -5.0f, 0.0f, 5.0f }, 2.0f, 2.0f, 2.0f, RED);
                    DrawCube((Vector3){ -5.0f, 0.0f, -5.0f }, 2.0f, 2.0f, 2.0f, GREEN);


                EndShaderMode();

                DrawGrid(40, 0.5f);

            EndMode3D();

            DrawFPS(10, 10);

            // DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);  // Example

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    UnloadShader(shader);       // Unload shader

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
