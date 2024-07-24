/*******************************************************************************************
*
*   raylib [core] example - Basic window
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

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif

Mesh CreateGrassBladeMesh();

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "raylib [shaders] example - vertex displacement");

    Camera camera = {0};
    camera.position = (Vector3){10.0f, 5.0f, 10.0f};
    camera.target = (Vector3){0.0f, 1.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Create Grass Blade Mesh
    Mesh grassBladeMesh = CreateGrassBladeMesh();
    
    // Define transforms to be uploaded to GPU for instances

    int MAX_GRASS_BLADES = 10000;
    int MAX_GRID_SIZE = 30;
    Matrix bladeMatrixs[10000];
    for (int i = 0; i < MAX_GRASS_BLADES; i++) {

        float randX = GetRandomValue(-MAX_GRID_SIZE, MAX_GRID_SIZE);
        float randZ = GetRandomValue(-MAX_GRID_SIZE, MAX_GRID_SIZE);
        Matrix translation = MatrixTranslate(randX, 0.0f, randZ);
        
        Matrix scale = MatrixScale(1.0f, 1.5f, 1.0f);

        float rotationAngle = (float) GetRandomValue(0, 100) / 100.0f * 360.0f;
        Vector3 rotationAxis = (Vector3){0.0f, 1.0f, 0.0f};
        Matrix rotation = MatrixRotate(rotationAxis, rotationAngle);
        
        bladeMatrixs[i] = MatrixMultiply(MatrixMultiply(translation, rotation), scale);
    }

    // Load Shader
    Shader shader = LoadShader(TextFormat("resources/shaders/glsl%i/vertex_displacement.vs", GLSL_VERSION),
                                TextFormat("resources/shaders/glsl%i/lighting.fs", GLSL_VERSION));
    // Get shader locations
    shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(shader, "mvp");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocationAttrib(shader, "instanceTransform");
    
    // Set shader value: ambient light level
    int ambientLoc = GetShaderLocation(shader, "ambient");
    SetShaderValue(shader, ambientLoc, (float[4]){ 0.2f, 0.2f, 0.2f, 1.0f }, SHADER_UNIFORM_VEC4);

    // Create One Directional Light
    Light light = CreateLight(LIGHT_DIRECTIONAL, (Vector3){ 20.0f, 20.0f, 0.0f }, Vector3Zero(), WHITE, shader);

    // Create a basic material for the instances
    Material matInstances = LoadMaterialDefault();
    matInstances.shader = shader;
    matInstances.maps[MATERIAL_MAP_DIFFUSE].color = GREEN;

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update
        UpdateCamera(&camera, CAMERA_FREE);

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
        DrawMeshInstanced(grassBladeMesh, matInstances, bladeMatrixs, MAX_GRASS_BLADES);
        EndMode3D();

        DrawText("Grass Blade Mesh", 10, 10, 20, DARKGRAY);
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    UnloadMesh(grassBladeMesh);
    UnloadShader(shader);
    CloseWindow();

    return 0;
}

Mesh CreateGrassBladeMesh() {
    Mesh mesh = {0};

    // Define vertices
    Vector3 vertices[] = {
        {-0.4f, 0, 0},    // Vertex 0: Left base
        {0.4f, 0, 0},     // Vertex 1: Right base
        {-0.25f, 1.0f, 0},   // Vertex 2: Second left
        {0.25f, 1.0f, 0},    // Vertex 3: Second right
        {-0.125f, 2.0f, 0},    // Vertex 4: Third left
        {0.125f, 2.0f, 0},      // Vertex 5: Third right
        {0, 3, 0}           // Vertex 6: Apex
    };

    // Define each triangle's indices
    unsigned short indices[] = {
        0, 1, 2,  // Triangle 1: Base to second level left
        1, 3, 2,  // Triangle 2: Base right to second level
        2, 3, 4,  // Triangle 3: Second level to third level left
        3, 5, 4,  // Triangle 4: Second right to third level
        4, 5, 6   // Triangle 5: Top triangle
    };

    // UVs for each vertex
    Vector2 uvs[] = {
        {0, 0},
        {1, 0},
        {0, 0.33f},
        {1, 0.33f},
        {0, 0.66f},
        {1, 0.66f},
        {0.5f, 1}
    };

    // Create the mesh
    mesh.vertexCount = sizeof(vertices) / sizeof(vertices[0]);
    mesh.triangleCount = sizeof(indices) / (3 * sizeof(indices[0]));

    // Allocate memory for the mesh data
    mesh.vertices = (float *)MemAlloc(mesh.vertexCount * 3 * sizeof(float)); // 3 components per vertex (x, y, z)
    mesh.texcoords = (float *)MemAlloc(mesh.vertexCount * 2 * sizeof(float)); // 2 components per UV (u, v)
    mesh.indices = (unsigned short *)MemAlloc(mesh.triangleCount * 3 * sizeof(unsigned short)); // 3 indices per triangle

    // Copy data to the mesh
    for (int i = 0; i < mesh.vertexCount; i++) {
        mesh.vertices[i * 3 + 0] = vertices[i].x;
        mesh.vertices[i * 3 + 1] = vertices[i].y;
        mesh.vertices[i * 3 + 2] = vertices[i].z;
        mesh.texcoords[i * 2 + 0] = uvs[i].x;
        mesh.texcoords[i * 2 + 1] = uvs[i].y;
    }

    for (int i = 0; i < mesh.triangleCount * 3; i++) {
        mesh.indices[i] = indices[i];
    }

    // Generate the normals
    UploadMesh(&mesh, false);

    return mesh;
}