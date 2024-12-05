#ifndef DRAW_H
#define DRAW_H

#include <raylib.h>
#include "block.h"

extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;

void drawBlock(struct Block block, int x, int y);
void drawWorld(struct Block *world);
void drawBgGrid(int x1, int y1, int x2, int y2, Color color);

#endif