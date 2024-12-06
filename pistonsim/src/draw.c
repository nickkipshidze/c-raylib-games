#include <raylib.h>
#include <stdio.h>

#include "block.h"
#include "world.h"

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 512;

void drawBlock(struct Block block, int x, int y) {
    if (block.blockId == BLOCK_ID_POWER) {
        DrawRectangleRounded(
            (Rectangle){x, y, tileSize, tileSize},
            0.2, 9, (Color){245, 28, 28, 255}
        );
    } else if (block.blockId == BLOCK_ID_WIRE) {
        Color color = (Color){52, 19, 19, 255};
        if (block.active == true)
            color = (Color){52 + (12 * block.data), 26, 26, 255};
        
        unsigned int state = block.state;
        if (((state >> 3) & 1) == 1)
            DrawRectangle(x+22, y, 19, 41, color);
        if (((state >> 2) & 1) == 1)
            DrawRectangle(x+22, y+22, 42, 19, color);
        if (((state >> 1) & 1) == 1)
            DrawRectangle(x+22, y+22, 19, 42, color);
        if (((state >> 0) & 1) == 1)
            DrawRectangle(x, y+22, 41, 19, color);
        if (state == 0)
            DrawRectangle(x+22, y+22, 19, 19, color);
    }
}

void drawWorld(struct Block *world) {
    for (int y = 0; y < worldHeight; y++)
        for (int x = 0; x < worldWidth; x++)
            drawBlock(world[y*worldWidth+x], x*64, y*64);
}

void drawBgGrid(int x1, int y1, int x2, int y2, Color color) {
    for (int y = y1; y < y2; y += tileSize)
        for (int x = x1; x < x2; x += tileSize)
            if ((x / tileSize + y / tileSize) % 2 == 0)
                DrawRectangle(x, y, tileSize, tileSize, color);
}
