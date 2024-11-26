#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 512

// Made by Nick Kipshidze C:

struct cardTile {
    int width, height;
    int posx, posy;
    int ofsx, ofsy;
    bool flipped, flipping, alive;
    Color value;
};

void mkCards(struct cardTile *cardGrid, int tileWidth, int tileHeight, int gridRows, int gridCols, float padding, int offsetx, int offsety) {
    // Populate the cardGrid array with this function

    const int cardCount = gridRows * gridCols;

    // Generate pairs of random colors
    Color randColors[cardCount];
    Color tmpColor;
    for (int i = 0; i < cardCount; i+=2) {
        tmpColor = (Color){
            GetRandomValue(0, 85)*3,
            GetRandomValue(0, 85)*3,
            GetRandomValue(0, 85)*3,
            255
        };
        randColors[i] = tmpColor;
        randColors[i+1] = tmpColor;
    }

    // Shuffle the color pairs
    Color tmpNum;
    int randInt1, randInt2;
    for (int i = 0; i < cardCount*2; i++) {
        randInt1 = GetRandomValue(0, cardCount-1);
        randInt2 = GetRandomValue(0, cardCount-1);
        tmpNum = randColors[randInt1];
        randColors[randInt1] = randColors[randInt2];
        randColors[randInt2] = tmpNum;
    }

    int cardIdx = 0;
    for (int y = 0; y < gridRows; y++) {
        for (int x = 0; x < gridCols; x++) {
            cardGrid[cardIdx] = (struct cardTile){
                tileWidth, tileHeight,
                x*tileWidth*padding+offsetx, y*tileHeight*padding+offsety,
                0, 0, false, false, true, randColors[cardIdx]
            };
            cardIdx++;
        }
    }
}

void printCards(struct cardTile *cardGrid, int len) {
    // Debug function for printing cardGrid values
    for (int i = 0; i < len; i++) {
        printf("{%d %d %d %d %d %d} ", cardGrid[i].width, cardGrid[i].height, cardGrid[i].posy, cardGrid[i].posx, cardGrid[i].flipped, cardGrid[i].flipping);
    }
    printf("\n");
}

void drawCards(struct cardTile *cardGrid, int len) {
    Color tileColor;

    for (int i = 0; i < len; i++) {
        if (cardGrid[i].flipped == true) tileColor = cardGrid[i].value;
        else tileColor = (Color){128, 128, 128, 255};

        if (cardGrid[i].alive == true) DrawRectangleRounded(
            (Rectangle){cardGrid[i].posx+cardGrid[i].ofsx, cardGrid[i].posy+cardGrid[i].ofsy, cardGrid[i].width, cardGrid[i].height},
            0.2, 9, tileColor
        );
    }
}

int getGridHoverI(struct cardTile *cardGrid, int len) {
    Vector2 mousePosition = GetMousePosition();

    // Temporary variables to make the "if" condition shorter
    int x1, y1, x2, y2;
    for (int i = 0; i < len; i++) {
        x1 = cardGrid[i].posx; y1 = cardGrid[i].posy;
        x2 = x1 + cardGrid[i].width; y2 = y1 + cardGrid[i].height;

        // Basic collision checking
        if (mousePosition.x > x1 && mousePosition.x < x2 && mousePosition.y > y1 && mousePosition.y < y2)
            if (cardGrid[i].alive == true) return i;
    }

    return -1;
}

bool gridClickEvent(struct cardTile *cardGrid, int clickedIndex) {
    // If clickedIndex is -1, function was called with no meaning
    if (clickedIndex == -1) return false;

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        // If tile is not flipped yet, and it is not being flipped already (animating) -> start flipping
        if (cardGrid[clickedIndex].flipped == false && cardGrid[clickedIndex].flipping == false) {
            cardGrid[clickedIndex].flipping = true;
        }

        // Unflip card for debug purposes
        // if (cardGrid[clickedIndex].flipped == true && cardGrid[clickedIndex].flipping == false) {
        //     cardGrid[clickedIndex].flipping = true;
        // }

        return true;
    }

    return false;
}

void animateTiles(struct cardTile *cardGrid, int cardWidth, int len) {
    for (int i = 0; i < len; i++) {
        // When it's time to flip -> start squeezing
        if (cardGrid[i].flipping == true) {
            cardGrid[i].width -= 1000 * GetFrameTime();
        }

        // When it has been squeezed all the way -> stop flipping
        if (cardGrid[i].width <= 0 && cardGrid[i].flipping == true) {
            cardGrid[i].flipping = false;
            cardGrid[i].flipped = !cardGrid[i].flipped;
        }

        // When it has been squeezed and stopped flipping -> start expanding again
        if (cardGrid[i].width < 128 && cardGrid[i].flipping == false) {
            cardGrid[i].width += 1000 * GetFrameTime();
            if (cardGrid[i].width > cardWidth) cardGrid[i].width = cardWidth;
        }
        cardGrid[i].ofsx = (cardWidth - cardGrid[i].width) / 2;
    }
}

int getFlippedCount(struct cardTile *cardGrid, int len) {
    int flipped = 0;
    for (int i = 0; i < len; i++) {
        // If card is flipped or being flipped -> Increase the flipped count
        if (cardGrid[i].flipped == true || (cardGrid[i].flipped != true && cardGrid[i].flipping == true))
            flipped++;
    }
    return flipped;
}

void hideAllCards(struct cardTile *cardGrid, int len) {
    for (int i = 0; i < len; i++) {
        if (cardGrid[i].flipped == true)
            cardGrid[i].flipping = true;
    }
}

bool destroySameCards(struct cardTile *cardGrid, int cardWidth, int len) {
    // Bool value return
    // Returns true when it's done animating the cards
    // Returns false when it need more calls/time

    Color flippedColor1, flippedColor2;
    int flpIdx1 = -1, flpIdx2 = -1;

    for (int i = 0; i < len; i++) {
        if (cardGrid[i].flipped == true) {
            if (flpIdx1 == -1) {
                flippedColor1 = cardGrid[i].value;
                flpIdx1 = i;
            } else {
                flippedColor2 = cardGrid[i].value;
                flpIdx2 = i;
                break;
            }
        }
    }

    if (flpIdx1 == -1 || flpIdx2 == -1)
        return true;

    // If the colors match then we squeeze the card and make it a meaningless value (destroy it)
    if (flippedColor1.r == flippedColor2.r && flippedColor1.g == flippedColor2.g && flippedColor1.b == flippedColor2.b) {
        if (cardGrid[flpIdx1].width <= 0 && cardGrid[flpIdx2].width <= 0) {
            cardGrid[flpIdx1] = (struct cardTile){-1, -1, -1, -1, -1, -1, false, false, false, WHITE};
            cardGrid[flpIdx2] = (struct cardTile){-1, -1, -1, -1, -1, -1, false, false, false, WHITE};
        } else {
            cardGrid[flpIdx1].width -= 1000 * GetFrameTime();
            cardGrid[flpIdx1].ofsx = (cardWidth - cardGrid[flpIdx1].width) / 2;

            cardGrid[flpIdx2].width -= 1000 * GetFrameTime();
            cardGrid[flpIdx2].ofsx = (cardWidth - cardGrid[flpIdx2].width) / 2;

            return false;
        }
    }

    return true;
}

bool checkWin(struct cardTile *cardGrid, int len) {
    for (int i = 0; i < len; i++) {
        if (!(cardGrid[i].value.r == WHITE.r && cardGrid[i].value.g == WHITE.g && cardGrid[i].value.b == WHITE.b)) {
            return false;
        }
    }
    return true;
}

void drawStats(float timePassed, int totalClicks, int movesMade) {
    char strTimePassed[256], strTotalClicks[256], strMovesMade[256];

    sprintf(strTimePassed, "Time passed: %d Sec", (int)timePassed);
    sprintf(strTotalClicks, "Flip count: %d", totalClicks);
    sprintf(strMovesMade, "Moves made: %d", movesMade);

    DrawText(strTimePassed, 10, 10, 20, BLACK);
    DrawText(strTotalClicks, 10, 40, 20, BLACK);
    DrawText(strMovesMade, 10, 70, 20, BLACK);
}

int main(int argc, char *argv[]) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Memory Game");
    SetTargetFPS(30);

    int cardCount = 6 * 4;
    int cardSize = 100;
    struct cardTile *cardGrid = malloc(sizeof(struct cardTile)*cardCount);

    float flipBackTimer = 0, flipBackTime = 0.5;

    mkCards(cardGrid, cardSize, cardSize, 4, 6, 1.1, 300, 45);
    printCards(cardGrid, cardCount);

    float timePassed = 0, totalClicks = 0, movesMade = 0;

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            drawCards(cardGrid, cardCount);

            if (getFlippedCount(cardGrid, cardCount) < 2) {
                if (gridClickEvent(
                    cardGrid, getGridHoverI(cardGrid, cardCount)
                ))
                    totalClicks++;
            } else {
                flipBackTimer += GetFrameTime();
                if (flipBackTimer >= flipBackTime) {
                    if (destroySameCards(cardGrid, cardSize, cardCount) == true) {
                        hideAllCards(cardGrid, cardCount);
                        movesMade++;
                        flipBackTimer = 0;
                    }
                }
            }

            if (flipBackTimer < flipBackTime)
                animateTiles(cardGrid, cardSize, cardCount);

            if (checkWin(cardGrid, cardCount) == true) {
                DrawText("Grid cleared!", 200, 200, 95, BLUE);
            } else {
                timePassed += GetFrameTime();
            }

            drawStats(timePassed, totalClicks, movesMade);
        EndDrawing();
    }

    CloseWindow();
    free(cardGrid);

    return 0;
}
