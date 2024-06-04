#include <catch2/catch_test_macros.hpp>

#include "API_graphics.h"
#include "stm32_ub_vga_screen.h"

TEST_CASE("Write Square to corner of 10,10 to 20,20 (pass)", "[require]")
{
    REQUIRE(API_clearscreen(VGA_COL_BLACK) == 0);
    REQUIRE(API_draw_rectangle(10, 10, 10, 10, VGA_COL_GREEN, 1, 0, 0) == 0);

    for (size_t x = 0; x < VGA_DISPLAY_X; x++)
    {
        for (size_t y = 0; y < VGA_DISPLAY_Y; y++)
        {
            CAPTURE(x, y);
            REQUIRE(VGA_RAM1[x + (y * (VGA_DISPLAY_X + 1))] == ((((x >= 10 && x < 20) && (y >= 10 && y < 20)) ? VGA_COL_GREEN : VGA_COL_BLACK)));
        }
    }
}