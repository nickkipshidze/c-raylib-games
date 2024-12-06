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

void updateCamera(Camera2D *camera, float *camPosX, float *camPosY) {
    camera -> target = moveVecTo(
        camera -> target,
        (Vector2){*camPosX, *camPosY},
        8
    );

    *camPosX += -GetMouseWheelMoveV().x * (2500 + (1000 / camera -> zoom)) * GetFrameTime();
    *camPosY += -GetMouseWheelMoveV().y * (2500 + (1000 / camera -> zoom)) * GetFrameTime();

    if (IsKeyDown(KEY_LEFT_CONTROL) == true) {
        if (IsKeyDown(KEY_EQUAL))
            camera -> zoom += camera -> zoom * 0.9 * GetFrameTime();
        if (IsKeyDown(KEY_MINUS))
            camera -> zoom -= camera -> zoom * 0.9 * GetFrameTime();
        
        if (camera -> zoom > 4) camera -> zoom = 4;
    }
}

int main(int argc, char *argv[]) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Pistonsim - By Nick");
    SetTargetFPS(60);
    
    float timeSinceLastTick = 0.0f;
    float gameTickInterval = 1.0f / 20.0f;

    float camPosX = WINDOW_WIDTH/2.0+((worldWidth/2.0-7)*tileSize);
    float camPosY = WINDOW_HEIGHT/2.0+((worldHeight/2.0-3)*tileSize);

    Camera2D camera = {0};
    camera.offset = (Vector2){WINDOW_WIDTH/2.0, WINDOW_HEIGHT/2.0};
    camera.target = (Vector2){camPosX, camPosY};
    camera.rotation = 0.0;
    camera.zoom = 1.0;

    struct Block *world = NULL;
    initWorld(&world);

    for (int x = worldWidth/2-10; x < worldWidth/2+11; x++)
        setBlock(world, x, worldWidth/2, (struct Block){BLOCK_ID_WIRE, .state = 0b0101});
    for (int y = worldHeight/2-10; y < worldHeight/2+11; y++)
        setBlock(world, worldWidth/2, y, (struct Block){BLOCK_ID_WIRE, .state = 0b1010});
    setBlock(world, worldWidth/2, worldHeight/2, (struct Block){.blockId = BLOCK_ID_POWER, .active = true, .data = 9});

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        timeSinceLastTick += deltaTime;

        int hoveridx = getWorldHoverIdx(camera);
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            if (hoveridx != -1)
                world[hoveridx] = (struct Block){BLOCK_ID_WIRE, 0, 0, false};
        } else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (hoveridx != -1)
                world[hoveridx] = (struct Block){BLOCK_ID_AIR};
        }

        while (timeSinceLastTick >= gameTickInterval) {
            updateWorld(world);
            timeSinceLastTick -= gameTickInterval;
        }

        updateCamera(&camera, &camPosX, &camPosY);

        BeginDrawing();
            ClearBackground(BLACK);

            BeginMode2D(camera);
                drawBgGrid(
                    0, 0,
                    worldWidth*tileSize, worldHeight*tileSize,
                    (Color){11, 11, 11, 255}
                );
                drawWorld(world);
                if (hoveridx != -1)
                    DrawRectangle(
                        (hoveridx % worldWidth) * tileSize,
                        (hoveridx / worldWidth) * tileSize,
                        tileSize, tileSize, (Color){255, 255, 255, 32}
                    );
            EndMode2D();
        EndDrawing();
    }

    CloseWindow();
    free(world);

    return 0;
}
