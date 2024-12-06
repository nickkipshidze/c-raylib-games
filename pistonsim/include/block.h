#ifndef BLOCK_H
#define BLOCK_H

extern const int BLOCK_ID_AIR;
extern const int BLOCK_ID_POWER;
extern const int BLOCK_ID_WIRE;

extern int worldWidth;
extern int worldHeight;

struct Block {
    int blockId;
    float data;
    float state;
    bool active;
    bool ghost;
};

void setBlock(struct Block *world, int x, int y, struct Block block);
struct Block *getBlock(struct Block *world, int x, int y);
int getBlockId(struct Block *world, int x, int y);

#endif