#include <stdio.h>
#include <stdlib.h>

#include "world.h"
#include "block.h"

const int tileSize = 64;

void initWorld(struct Block **world) {
    *world = malloc(sizeof(struct Block) * worldWidth * worldHeight);

    for (int i = 0; i < worldWidth * worldHeight; i++) {
        (*world)[i] = (struct Block){
            BLOCK_ID_AIR, -1, -1, false
        };
    }
}

void printWorld(struct Block *world) {
    for (int y = 0; y < worldHeight; y++) {
        for (int x = 0; x < worldWidth; x++) {
            printf("%d ", world[y*worldWidth+x].blockId);
        }
        printf("\n");
    }
}

void updateWorld(struct Block *world) {
    int dirOfs[4][2] = {
        {0, -1}, {1, 0}, {0, 1}, {-1, 0}
    };

    for (int y = 0; y < worldHeight; y++) {
        for (int x = 0; x < worldWidth; x++) {
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

int getWorldHoverIdx(Camera2D camera) {
    float relMouseX = GetMousePosition().x;
    float relMouseY = GetMousePosition().y;
    
    Vector2 mouseWorldPos = GetScreenToWorld2D((Vector2){relMouseX, relMouseY}, camera);
    
    int mouseX = (int)(mouseWorldPos.x / tileSize);
    int mouseY = (int)(mouseWorldPos.y / tileSize);
    
    if (mouseX >= 0 && mouseX < worldWidth && mouseY >= 0 && mouseY < worldHeight)
        return (mouseY * worldWidth) + mouseX;
    else
        return -1;
}
