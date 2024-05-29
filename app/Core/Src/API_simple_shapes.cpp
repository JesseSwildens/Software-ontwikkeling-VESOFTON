#include "API_simple_shapes.h"
#include "stm32_ub_vga_screen.h"

#include <math.h>
#include <string>

#define BETWEEN(x, y, z) ((x < z) && (x > y))
#define OUTSIDE(x, y, z) ((x > z) || (x < y))

/**
 * @note Maximum color value supported
 */
#define MAX_COLOR_DEPTH 0xff // 8 bit for all colors

/**
 * @note math PI
 */
#define PI 3.1415926

/**
 * @note Max radius the circle can be without getting gaps in circle
 */
#define MAX_RADIUS_SIZE 25
/**
 * @note Amount of points draw while drawing a circle
 */
#define MAX_CIRCLE_POINTS 100

static int (*loggerCallback)(const char* message, int length) = nullptr;
static void log_message(std::string message);
static void API_set_pixel(int, int, uint8_t);

/**
 * @brief Drawing line
 *
 * @param x_12 X coordinates of the points
 * @param y_12 Y coordinates of the points
 * @param color Color of the line
 * @param weight Thickness of the corresponding line
 * @param reserved Currently unused. Reserved for future use.
 *
 * @return -1 if error occured, otherwise 0 (Also a call to Logger callback is sent if ptr isn't NULL)
 */
int API_draw_line(int x0, int y0, int x1, int y1, int color, int weight, int reserved)
{
    (void)reserved;

    if (OUTSIDE(x0, 0, VGA_DISPLAY_X) || OUTSIDE(x1, 0, VGA_DISPLAY_X)
        || OUTSIDE(y0, 0, VGA_DISPLAY_Y) || OUTSIDE(y1, 0, VGA_DISPLAY_Y))
    {
        log_message("Line outside range of display");
        return -1;
    }

    for (int i = 1; i < weight; i++)
    {
        API_draw_circle(x0, y0, i, color, 0);
    }

    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep)
    {
        int temp = x0;
        x0 = y0;
        y0 = x0;
        temp = x1;
        x1 = y1;
        y1 = temp;
    }

    if (x0 > x1)
    {
        int temp = x0;
        x0 = x1;
        x1 = temp;
        temp = y0;
        y0 = y1;
        y1 = temp;
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1)
    {
        ystep = 1;
    }
    else
    {
        ystep = -1;
    }

    for (; x0 <= x1; x0++)
    {
        if (steep)
        {
            API_draw_circle(y0, x0, weight, color, 1);
        }
        else
        {
            API_draw_circle(x0, y0, weight, color, 1);
        }
        err -= dy;
        if (err < 0)
        {
            y0 += ystep;
            err += dx;
        }
    }

    return 0;
}

/**
 * @brief Drawing circle
 *
 * @param x0_y0 position on screen
 * @param radius Radius of the circle
 * @param color Color of the circle
 * @param filled Fill in the circle
 *
 * @return -1 if error occured, otherwise 0
 */
int API_draw_circle(int x, int y, int radius, int color, int filled)
{
    if (OUTSIDE(x, 0, VGA_DISPLAY_X) || OUTSIDE(y, 0, VGA_DISPLAY_Y))
    {
        log_message("X or Y is outside the allowed range");
        return -1;
    }

    if (OUTSIDE(color, 0, MAX_COLOR_DEPTH))
    {
        log_message("Color outside of allowed range. Color scaling isn't supported.");
        return -1;
    }

    if (OUTSIDE(radius, 0, MAX_RADIUS_SIZE))
    {
        log_message("Radius is outside of allowed range");
        return -1;
    }

    if (filled)
    {
        for (int y_temp = -radius; y_temp <= radius; y_temp++)
            for (int x_temp = -radius; x_temp <= radius; x_temp++)
                if (x_temp * x_temp + y_temp * y_temp <= radius * radius)
                    API_set_pixel(x + x_temp, y + y_temp, color);
        return 0;
    }

    int x_temp = radius;
    int y_temp = 0;
    int decision_over_2 = 1 - x_temp; // Decision criterion divided by 2 evaluated at x=r, y=0

    while (y_temp <= x_temp)
    {
        // Draw the 8 octants
        API_set_pixel(x + x_temp, y + y_temp, color);
        API_set_pixel(x + y_temp, y + x_temp, color);
        API_set_pixel(x - y_temp, y + x_temp, color);
        API_set_pixel(x - x_temp, y + y_temp, color);
        API_set_pixel(x - x_temp, y - y_temp, color);
        API_set_pixel(x - y_temp, y - x_temp, color);
        API_set_pixel(x + y_temp, y - x_temp, color);
        API_set_pixel(x + x_temp, y - y_temp, color);

        y_temp++;
        if (decision_over_2 <= 0)
        {
            decision_over_2 += 2 * y_temp + 1; // Change in decision criterion for y -> y+1
        }
        else
        {
            x_temp--;
            decision_over_2 += 2 * (y_temp - x_temp) + 1; // Change for y -> y+1, x -> x-1
        }
    }

    return 0; // Success
}

/**
 * @brief Helper function for calling the logger callback for in the API layer
 *
 * @param message std::string type with the corresponding message
 *
 * @return None
 */
static void log_message(std::string message)
{
    if (loggerCallback == nullptr)
        return;

    (*loggerCallback)(message.c_str(), message.length());
}

/**
 * @brief Register a callback that can be used to transfer messages from this layer to other layers. Otherwise logger functionality it is disregarded and not used.
 *
 * @param pFunction Function pointer with a const char* for the string and int length for the length of the string to prevent buffer overflows.
 *
 * @return Integer 0 if succesfull otherwise -1
 */
int API_register_logger_callback(int (*pFunction)(const char* string, int len))
{
    if (pFunction != nullptr)
    {
        loggerCallback = pFunction;
        return 0;
    }

    return -1;
}

/**
 * @brief Draw pixel to screen.
 *
 * @param x X position on screen
 * @param y y position on screen
 * @param color color to set the pixel to
 *
 * @return None
 */
static void API_set_pixel(int x, int y, uint8_t color)
{
    if (OUTSIDE(x, 0, VGA_DISPLAY_X) || OUTSIDE(y, 0, VGA_DISPLAY_Y))
        return;
    VGA_RAM1[(y * (VGA_DISPLAY_X + 1)) + x] = color;
}

/**
 * @brief Drawing rectangle on the screen
 *
 * @param xy upper left corner of the rectangle
 * @param width width of the rectangle
 * @param height height of the rectangle
 * @param color Color of the rectangle
 * @param filled Fill in the rectangle
 * @param Weight Weight of the lines in the unfilled rectangle. Unused in filled
 * @param reserved Currently unused. Reserved for future use.
 *
 * @return 0 if succesfull, otherwise >= -1 if error occured
 */
int API_draw_rectangle(int x, int y, int width, int height, int color, int filled, int weight, int reserved)
{
    (void)reserved;
    if (OUTSIDE(x + width, 0, VGA_DISPLAY_X) || OUTSIDE(y + height, 0, VGA_DISPLAY_Y))
        return -1;

    int ret = 0;
    if (filled)
    {
        for (int i = x; i < (x + width); i++)
        {
            for (int j = y; j < (y + height); j++)
            {
                API_set_pixel(i, j, color);
            }
        }
    }
    else
    {
        ret += API_draw_line(x, y, (x + width), y, color, weight, 0);
        ret += API_draw_line((x + width), (y + height), (x + width), y, color, weight, 0);
        ret += API_draw_line(x, (y + height), (x + width), (y + height), color, weight, 0);
        ret += API_draw_line(x, y, x, (y + height), color, weight, 0);
    }

    return ret;
}

/**
 * @brief clear screen to certain color
 *
 * @param color color that is used to set the background and overwrite all previous images on the screen
 */
int API_clearscreen(int color)
{
    if (color > MAX_COLOR_DEPTH)
    {
        log_message("Color outside the range of the display");
    }
    UB_VGA_FillScreen((uint8_t)color);
    return 0;
}