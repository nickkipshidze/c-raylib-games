#include <stdbool.h>
#include "block.h"

const int BLOCK_ID_AIR = 0x00;
const int BLOCK_ID_POWER = 0x01;
const int BLOCK_ID_WIRE = 0x02;

int worldWidth = 128;
int worldHeight = 128;

struct Block;

void setBlock(struct Block *world, int x, int y, struct Block block) {
    world[y*worldWidth+x] = block;
}

struct Block *getBlock(struct Block *world, int x, int y) {
    if (x >= 0 && x < worldWidth && y >= 0 && y < worldHeight)
        return &world[y*worldWidth+x];
    else {
        static struct Block air = {.blockId = BLOCK_ID_AIR};
        return &air;
    }
}

int getBlockId(struct Block *world, int x, int y) {
    if (x >= 0 && x < worldWidth && y >= 0 && y < worldHeight)
        return world[y*worldWidth+x].blockId;
    else
        return BLOCK_ID_AIR;
}
