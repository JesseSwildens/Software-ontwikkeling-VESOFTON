#include "API_graphics.h"

int main(void)
{
    API_Init();

    API_clearscreen(VGA_COL_BLACK);

    API_clearscreen(VGA_COL_BLUE);

    API_draw_circle(100, 200, 20, VGA_COL_YELLOW, 0);
    API_draw_circle(150, 200, 20, VGA_COL_CYAN, 1);

    API_draw_text(0, 0, VGA_COL_GREEN, (char*)"Hello C++ enjoyers!\nThis is Working!!!!", (char*)"FreeSerif", 1, 0, 0);
    API_draw_text(50, 100, VGA_COL_RED, (char*)"Wrapping also works!", (char*)"FreeMono", 2, 2, 0);

    while (1)
    {
        // Endless loop
    }
}