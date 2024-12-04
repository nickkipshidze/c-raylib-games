#include <raylib.h>
#include <stdlib.h>
#include <stdbool.h>

#include "block.h"
#include "world.h"
#include "draw.h"

Vector2 moveVecTo(Vector2 source, Vector2 target, float drift) {
    float distx = source.x - target.x;
    float disty = source.y - target.y;
    return (Vector2){source.x - distx/drift, source.y - disty/drift};
}

int main(int argc, char *argv[]) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Pistonsim - By Nick");
    SetTargetFPS(60);
    
    float timeSinceLastTick = 0.0f;
    float gameTickInterval = 1.0f / 20.0f;

    float camPosX = WINDOW_WIDTH/2.0, camPosY = WINDOW_HEIGHT/2.0;
    Camera2D camera = {0};
    camera.offset = (Vector2){camPosX, camPosY};
    camera.rotation = 0.0;
    camera.zoom = 1.0;

    struct Block *world = NULL;
    initWorld(&world);

    setBlock(world, 2, 2, (struct Block){.blockId = BLOCK_ID_POWER, .active = true, .data = 9});
    setBlock(world, 3, 2, (struct Block){BLOCK_ID_WIRE});
    setBlock(world, 4, 2, (struct Block){BLOCK_ID_WIRE});
    setBlock(world, 5, 2, (struct Block){BLOCK_ID_WIRE});
    setBlock(world, 6, 2, (struct Block){BLOCK_ID_WIRE});
    setBlock(world, 7, 2, (struct Block){BLOCK_ID_WIRE});
    setBlock(world, 7, 3, (struct Block){BLOCK_ID_WIRE});
    setBlock(world, 7, 4, (struct Block){BLOCK_ID_WIRE});
    setBlock(world, 8, 4, (struct Block){BLOCK_ID_WIRE});
    setBlock(world, 9, 4, (struct Block){BLOCK_ID_WIRE});
    setBlock(world, 9, 3, (struct Block){BLOCK_ID_WIRE});
    setBlock(world, 9, 1, (struct Block){BLOCK_ID_WIRE});

    printWorld(world);

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        timeSinceLastTick += deltaTime;

        while (timeSinceLastTick >= gameTickInterval) {
            updateWorld(world);
            timeSinceLastTick -= gameTickInterval;
        }

        camera.target = moveVecTo(
            camera.target,
            (Vector2){camPosX, camPosY},
            8
        );

        camPosX += -GetMouseWheelMoveV().x * 2500 * GetFrameTime();
        camPosY += -GetMouseWheelMoveV().y * 2500 * GetFrameTime();

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            if (getWorldHoverIdx(camera) != -1)
                world[getWorldHoverIdx(camera)].blockId = BLOCK_ID_WIRE;
        } else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (getWorldHoverIdx(camera) != -1)
                world[getWorldHoverIdx(camera)] = (struct Block){BLOCK_ID_AIR};
        }

        if (IsKeyDown(KEY_LEFT_CONTROL) == true) {
            if (IsKeyDown(KEY_EQUAL))
                camera.zoom += camera.zoom * 0.9 * GetFrameTime();
            if (IsKeyDown(KEY_MINUS))
                camera.zoom -= camera.zoom * 0.9 * GetFrameTime();
            
            if (camera.zoom < 0.2) camera.zoom = 0.2;
            if (camera.zoom > 5) camera.zoom = 5;
        }

        BeginDrawing();
            ClearBackground(BLACK);

            BeginMode2D(camera);
                drawBgGrid(0, 0, 1024, 512, (Color){11, 11, 11, 255});
                drawWorld(world);
            EndMode2D();
        EndDrawing();
    }

    CloseWindow();
    free(world);

    return 0;
}
