#ifndef WORLD_H
#define WORLD_H

#include <raylib.h>
#include "block.h"

extern const int tileSize;

void initWorld(struct Block **world);
void printWorld(struct Block *world);
void updateWorld(struct Block *world);
int getWorldHoverIdx(Camera2D camera);

#endif