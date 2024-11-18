#include <raylib.h>
#include <stdbool.h>
#include <stdlib.h>

#include "alpha_beta.h"
#include "puzzle_1.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 512

struct DragImg {
    int posx, posy;
    int width, height;
    Texture2D texture;
    bool dragging;
};

void genCropRects(Rectangle *cropRects, Image image, int width, int height, int tileSize) {
    int gridWidth = width / tileSize;
    int gridHeight = height / tileSize;

    int idx = 0;
    for (int y = 0; y < gridHeight; y++) {
        for (int x = 0; x < gridWidth; x++) {
            cropRects[idx] = (Rectangle){x*tileSize, y*tileSize, tileSize, tileSize};
            idx++;
        }
    }
}

void genCropImages(Image *cropImages, Rectangle *cropRects, Image image, int cropCount) {
    for (int i = 0; i < cropCount; i++) {
        cropImages[i] = ImageCopy(image);
        ImageCrop(&cropImages[i], cropRects[i]);
    }
}

void genDragImgs(struct DragImg *dragImgs, Rectangle *cropRects, Texture2D *textures, int cropCount, int randOfsVar, int ofsx, int ofsy) {
    for (int i = 0; i < cropCount; i++) {
        dragImgs[i] = (struct DragImg){
            cropRects[i].x + GetRandomValue(-randOfsVar, randOfsVar) + ofsx,
            cropRects[i].y + GetRandomValue(-randOfsVar, randOfsVar) + ofsy,
            cropRects[i].width,
            cropRects[i].height,
            textures[i],
            false
        };
    }
}

void imgsToTxts(Texture2D *textures, Image *images, int count) {
    for (int i = 0; i < count; i++) {
        textures[i] = LoadTextureFromImage(images[i]);
    }
}

void unloadImages(Image *images, int count) {
    for (int i = 0; i < count; i++) {
        UnloadImage(images[i]);
    }
}

void unloadTextures(Texture2D *textures, int count) {
    for (int i = 0; i < count; i++) {
        UnloadTexture(textures[i]);
    }
}

void drawGrid(Color primary, Color secondary, int tileSize) {
    Color tmp;

    for (int y = 0; y < SCREEN_HEIGHT / tileSize + 1; y++) {
        for (int x = 0; x < SCREEN_WIDTH / tileSize + 1; x++) {
            if (y % 2 == 0 && x % 2 == 0) tmp = primary;
            else if (y % 2 == 0 && x % 2 != 0) tmp = secondary;
            else if (y % 2 != 0 && x % 2 == 0) tmp = secondary;
            else if (y % 2 != 0 && x % 2 != 0) tmp = primary;

            DrawRectangle(
                x*tileSize, y*tileSize,
                tileSize, tileSize,
                tmp
            );
        }
    }
}

void drawDragImgs(struct DragImg *dragImgs, int count) {
    for (int i = 0; i < count; i++) {
        DrawTexture(dragImgs[i].texture, dragImgs[i].posx, dragImgs[i].posy, WHITE);
    }
}

int getDragIdxByXY(struct DragImg *dragImgs, int x, int y, int count) {
    for (int i = 0; i < count; i++) {
        if (
            dragImgs[i].posx < x && dragImgs[i].posx + dragImgs[i].width > x
            && dragImgs[i].posy < y && dragImgs[i].posy + dragImgs[i].height > y
        ) {
            return i;
        }
    }
    return -1;
}

bool anyDragging(struct DragImg *dragImgs, int count) {
    for (int i = 0; i < count; i++) {
        if (dragImgs[i].dragging == true)
            return true;
    }
    return false;
}

void updateDragImgs(struct DragImg *dragImgs, int count) {
    int draggingIndex = -1;
    for (int i = 0; i < count; i++) {
        if (dragImgs[i].dragging == true) {
            dragImgs[i].posx = GetMouseX() - 40;
            dragImgs[i].posy = GetMouseY() - 40;
            draggingIndex = i;
        }
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        int pressedIndex = getDragIdxByXY(dragImgs, GetMouseX(), GetMouseY(), count);
        if (draggingIndex == -1 && pressedIndex != -1) {
            dragImgs[pressedIndex].dragging = true;
        }
    } else {
        if (draggingIndex != -1)
            dragImgs[draggingIndex].dragging = false;
    }
}

void snapGridDragImgs(struct DragImg *dragImgs, int tileSize, int count) {
    for (int i = 0; i < count; i++) {
        int snappedX = (int)((dragImgs[i].posx + tileSize / 2) / tileSize) * tileSize;
        int snappedY = (int)((dragImgs[i].posy + tileSize / 2) / tileSize) * tileSize;

        bool overlap = false;
        for (int j = 0; j < count; j++) {
            if (i != j && dragImgs[j].posx == snappedX && dragImgs[j].posy == snappedY) {
                overlap = true;
                break;
            }
        }

        if (!overlap) {
            dragImgs[i].posx = snappedX;
            dragImgs[i].posy = snappedY;
        }
    }
}

bool checkCorrectArr(struct DragImg *dragImgs, int columns, int rows, int tileSize, int count) {
    int last = -1;

    for (int i = 0; i < count; i++) {
        int expectedX = (i % columns) + (dragImgs[0].posx / tileSize);
        int expectedY = (i / columns) + (dragImgs[0].posy / tileSize);

        int actualX = dragImgs[i].posx / tileSize;
        int actualY = dragImgs[i].posy / tileSize;

        if (actualX != expectedX || actualY != expectedY) {
            return false;
        }
    }

    if (anyDragging(dragImgs, count) == true)
        return false;
    return true;
}

void shuffleDragImgs(struct DragImg *dragImgs, int iterations, int count) {
    int idx1, idx2, tmp;
    for (int i = 0; i < iterations; i++) {
        idx1 = GetRandomValue(0, count-1);
        idx2 = GetRandomValue(0, count-1);

        tmp = dragImgs[idx1].posx;
        dragImgs[idx1].posx = dragImgs[idx2].posx;
        dragImgs[idx2].posx = tmp;

        tmp = dragImgs[idx1].posy;
        dragImgs[idx1].posy = dragImgs[idx2].posy;
        dragImgs[idx2].posy = tmp;
    }
}

int main(int argc, char *argv[]) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Puzimg");
    SetTargetFPS(30);

    Image puzzle_1 = ImageCopy((Image){
        .data = PUZZLE_1_DATA,
        .width = PUZZLE_1_WIDTH,
        .height = PUZZLE_1_HEIGHT,
        .format = PUZZLE_1_FORMAT,
        .mipmaps = 1
    });
    ImageResize(&puzzle_1, 400, 400);

    Image img_font = (Image){
        .data = ALPHA_BETA_DATA,
        .width = ALPHA_BETA_WIDTH,
        .height = ALPHA_BETA_HEIGHT,
        .format = ALPHA_BETA_FORMAT,
        .mipmaps = 1
    };
    Font font_main = LoadFontFromImage(img_font, MAGENTA, ' ');

    int tileSize = 80;
    int rows = (400 / tileSize), columns = (400 / tileSize);
    int tileCount = rows * columns;
    Rectangle *cropRects = malloc(sizeof(Rectangle) * tileCount);
    Image *cropImages = malloc(sizeof(Image) * tileCount);
    Texture2D *textures = malloc(sizeof(Texture2D) * tileCount);

    genCropRects(cropRects, puzzle_1, 400, 400, tileSize);
    genCropImages(cropImages, cropRects, puzzle_1, tileCount);
    imgsToTxts(textures, cropImages, tileCount);
    unloadImages(cropImages, tileCount);
    UnloadImage(puzzle_1);
    free(cropImages);

    struct DragImg *dragImgs = malloc(sizeof(struct DragImg) * tileCount);
    genDragImgs(dragImgs, cropRects, textures, tileCount, 0, 340, 80);
    shuffleDragImgs(dragImgs, 20, tileCount);

    int winFlicker = 0;

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            drawGrid(RAYWHITE, WHITE, tileSize);
            drawDragImgs(dragImgs, tileCount);
            snapGridDragImgs(dragImgs, tileSize, tileCount);

            if (checkCorrectArr(dragImgs, columns, rows, tileSize, tileCount) == true) {
                winFlicker++;
                if (winFlicker/10 % 2 == 0)
                    DrawTextEx(font_main, "PUZZLE COMPLETE!", (Vector2){180, 200}, 70, 4, GOLD);
                else
                    DrawTextEx(font_main, "PUZZLE COMPLETE!", (Vector2){180, 200}, 70, 4, YELLOW);
            } else {
                updateDragImgs(dragImgs, tileCount);
            }
        EndDrawing();
    }

    unloadTextures(textures, tileCount);
    UnloadFont(font_main);

    free(cropRects);
    free(textures);
    free(dragImgs);

    CloseWindow();
    return 0;
}
