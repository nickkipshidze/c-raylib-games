#include <stdio.h>
#include <stdlib.h>

#include "world.h"
#include "block.h"
#include "draw.h"

int WRLDWidth = 128;
int WRLDHeight = 128;
const int WRLDTileSize = 64;

float WRLDLastTick = 0.0;
float WRLDTickRate = 0.05;
bool WRLDTickPaused = false;

void initWorld(struct Block **world) {
    *world = malloc(sizeof(struct Block) * WRLDWidth * WRLDHeight);

    for (int i = 0; i < WRLDWidth * WRLDHeight; i++) {
        (*world)[i] = (struct Block){
            BLOCK_ID_AIR, -1, -1, false
        };
    }
}

void printWorld(struct Block *world) {
    for (int y = 0; y < WRLDHeight; y++) {
        for (int x = 0; x < WRLDWidth; x++) {
            printf("%d ", world[y*WRLDWidth+x].blockId);
        }
        printf("\n");
    }
}

void tickWorld(struct Block *world) {
    int dirOfs[4][2] = {
        {0, -1}, {1, 0}, {0, 1}, {-1, 0}
    };

    for (int y = 0; y < WRLDHeight; y++) {
        for (int x = 0; x < WRLDWidth; x++) {
            struct Block *target = getBlock(world, x, y);
            target -> ghost = false;

            if (target -> blockId == BLOCK_ID_WIRE) {
                int maxSupply = -1;
                bool ghostSupply = false;
                unsigned int connDirs = 0b0000;

                for (int i = 0; i < 4; i++) {
                    if (
                        (getBlockId(world, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_POWER ||
                        getBlockId(world, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_WIRE) &&
                        getBlock(world, x+dirOfs[i][0], y+dirOfs[i][1]) -> active == true &&
                        getBlock(world, x+dirOfs[i][0], y+dirOfs[i][1]) -> data > 1
                    ) {
                        int supply = getBlock(world, x+dirOfs[i][0], y+dirOfs[i][1]) -> data;
                        if (supply >= maxSupply) {
                            maxSupply = supply;
                            ghostSupply = getBlock(world, x+dirOfs[i][0], y+dirOfs[i][1]) -> ghost;
                        }
                    }
                    
                    if (
                        getBlockId(world, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_POWER ||
                        getBlockId(world, x+dirOfs[i][0], y+dirOfs[i][1]) == BLOCK_ID_WIRE
                    ) {
                        connDirs |= (1 << (3 - i));
                    }
                }

                target -> state = connDirs;

                if (target -> active == false && maxSupply != -1 && ghostSupply == false) {
                    target -> active = true;
                    target -> data = maxSupply - 1;
                    target -> ghost = true;
                }

                if (target -> active == true) {
                    if (maxSupply == -1) {
                        target -> active = false;
                        target -> data = 0;
                        target -> ghost = true;
                    } else if (target -> data != maxSupply - 1 && ghostSupply == false) {
                        target -> data = maxSupply - 1;
                        target -> ghost = true;
                    }
                }
            }
        }
    }
}

void updateWorld(struct Block *world, Camera2D camera) {
    int hoveridx = getWorldHoverIdx(camera);
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        if (hoveridx != -1)
            world[hoveridx] = (struct Block){BLOCK_ID_WIRE, 0, 0, false};
    } else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (hoveridx != -1)
            world[hoveridx] = (struct Block){BLOCK_ID_AIR};
    }

    if (IsKeyPressed(KEY_F3)) DEBUG_MODE = !DEBUG_MODE;

    if (DEBUG_MODE) {
        if (IsKeyPressed(KEY_SPACE)) WRLDTickPaused = !WRLDTickPaused;
        if (WRLDTickPaused && IsKeyPressed(KEY_ENTER)) {
            tickWorld(world);
        }
    }

    if (!WRLDTickPaused) {
        WRLDLastTick += GetFrameTime();
        while (WRLDLastTick >= WRLDTickRate) {
            tickWorld(world);
            WRLDLastTick -= WRLDTickRate;
        }
    }

    drawWorld(world, camera);

    if (hoveridx != -1)
        DrawRectangle(
            (hoveridx % WRLDWidth) * WRLDTileSize,
            (hoveridx / WRLDWidth) * WRLDTileSize,
            WRLDTileSize, WRLDTileSize, (Color){255, 255, 255, 32}
        );
}

int getWorldHoverIdx(Camera2D camera) {
    float relMouseX = GetMousePosition().x;
    float relMouseY = GetMousePosition().y;
    
    Vector2 mouseWorldPos = GetScreenToWorld2D((Vector2){relMouseX, relMouseY}, camera);
    
    int mouseX = (int)(mouseWorldPos.x / WRLDTileSize);
    int mouseY = (int)(mouseWorldPos.y / WRLDTileSize);
    
    if (mouseX >= 0 && mouseX < WRLDWidth && mouseY >= 0 && mouseY < WRLDHeight)
        return (mouseY * WRLDWidth) + mouseX;
    else
        return -1;
}
