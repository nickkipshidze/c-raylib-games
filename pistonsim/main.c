#include <raylib.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 512

#define BLOCK_ID_AIR 0x00
#define BLOCK_ID_POWER 0x01
#define BLOCK_ID_WIRE 0x02

struct Block {
    int blockId;
    int data;
    float state;
    bool active;
    bool ghost;
};

const int worldWidth = 16;
const int worldHeight = 8;
const int tileSize = 64;

void initWorld(struct Block **world) {
    *world = malloc(sizeof(struct Block) * worldWidth * worldHeight);

    for (int i = 0; i < worldWidth * worldHeight; i++) {
        (*world)[i] = (struct Block){
            BLOCK_ID_AIR, -1, -1, false
        };
    }
}

void setBlock(struct Block *world, int x, int y, struct Block block) {
    world[y*worldWidth+x] = block;
}

struct Block *getBlock(struct Block *world, int x, int y) {
    if (x >= 0 && x < worldWidth && y >= 0 && y < worldHeight)
        return &world[y*worldWidth+x];
    else
        return (struct Block *){BLOCK_ID_AIR};
}

int getBlockId(struct Block *world, int x, int y) {
    if (x >= 0 && x < worldWidth && y >= 0 && y < worldHeight)
        return world[y*worldWidth+x].blockId;
    else
        return BLOCK_ID_AIR;
}

void printWorld(struct Block *world) {
    for (int y = 0; y < worldHeight; y++) {
        for (int x = 0; x < worldWidth; x++) {
            printf("%d ", world[y*worldWidth+x].blockId);
        }
        printf("\n");
    }
}

void drawBlock(struct Block block, int x, int y) {
    if (block.blockId == BLOCK_ID_POWER) {
        DrawRectangle(x, y, tileSize, tileSize, (Color){245, 28, 28, 255});
    } else if (block.blockId == BLOCK_ID_WIRE) {
        if (block.active == true)
            DrawRectangle(x+15, y+15, tileSize-30, tileSize-30, (Color){52 + (12 * block.data), 26, 26, 255});
        else
            DrawRectangle(x+20, y+20, tileSize-40, tileSize-40, (Color){52, 19, 19, 255});
    }
}

void drawWorld(struct Block *world) {
    for (int y = 0; y < worldHeight; y++) {
        for (int x = 0; x < worldWidth; x++) {
            drawBlock(world[y*worldWidth+x], x*64, y*64);
        }
    }
}

void drawBgGrid(int x1, int y1, int x2, int y2, Color color) {
    for (int y = y1; y < y2; y += tileSize) {
        for (int x = x1; x < x2; x += tileSize) {
            if ((x / tileSize + y / tileSize) % 2 == 0)
                DrawRectangle(x, y, tileSize, tileSize, color);
        }
    }
}

void updateWorld(struct Block *world) {
    int dirOfs[4][2] = {
        {-1, 0}, {1, 0}, {0, -1}, {0, 1}
    };

    for (int y = 0; y < worldHeight; y++) {
        for (int x = 0; x < worldWidth; x++) {
            struct Block *target = getBlock(world, x, y);
            target -> ghost = false;

            if (target -> blockId == BLOCK_ID_WIRE) {
                int maxSupply = -1;
                for (int i = 0; i < 4; i++) {
                    if (
                        (getBlockId(world, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_POWER ||
                        getBlockId(world, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_WIRE) &&
                        getBlock(world, x+dirOfs[i][0], y+dirOfs[i][1]) -> active == true &&
                        getBlock(world, x+dirOfs[i][0], y+dirOfs[i][1]) -> ghost == false &&
                        getBlock(world, x+dirOfs[i][0], y+dirOfs[i][1]) -> data > 1
                    ) {
                        int supply = getBlock(world, x+dirOfs[i][0], y+dirOfs[i][1]) -> data;
                        if (supply >= maxSupply) maxSupply = supply;
                    }
                }

                if (target -> active == false && maxSupply != -1) {
                    target -> active = true;
                    target -> data = maxSupply - 1;
                    target -> ghost = true;
                }

                if (target -> active == true) {
                    if (maxSupply == -1) {
                        target -> active = false;
                        target -> data = 0;
                        target -> ghost = true;
                    } else if (target -> data != maxSupply - 1) {
                        target -> data = maxSupply - 1;
                        target -> ghost = true;
                    }
                }
            }
        }
    }
}

Vector2 moveVecTo(Vector2 source, Vector2 target, float drift) {
    float distx = source.x - target.x;
    float disty = source.y - target.y;
    return (Vector2){source.x - distx/drift, source.y - disty/drift};
}

int getWorldHoverIdx(Camera2D camera) {
    float relMouseX = GetMousePosition().x + camera.target.x - camera.offset.x;
    float relMouseY = GetMousePosition().y + camera.target.y - camera.offset.y;
    int mouseX = relMouseX / tileSize;
    int mouseY = relMouseY / tileSize;
    
    if (mouseX >= 0 && mouseX < worldWidth && mouseY >= 0 && mouseY < worldHeight)
        return (mouseY * worldWidth) + mouseX;
    else
        return -1;
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
