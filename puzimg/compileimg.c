#include <raylib.h>

int main(void) {
    Image image;

    image = LoadImage("puzzle_1.png");
    ExportImageAsCode(image, "puzzle_1.h");
    UnloadImage(image);

    image = LoadImage("alpha_beta.png");
    ExportImageAsCode(image, "alpha_beta.h");
    UnloadImage(image);

    return 0;
}
