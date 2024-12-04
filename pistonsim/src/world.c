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
