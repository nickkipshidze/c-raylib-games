#include <raylib.h>

#include "block.h"
#include "world.h"

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 512;

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
