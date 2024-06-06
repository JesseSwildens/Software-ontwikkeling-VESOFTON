#ifndef API_GRAPHICS_H
#define API_GRAPHICS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @note struct containing size and position of a bitmap
     */
    typedef struct
    {
        int x; // x position top left
        int y; // y position top left
    } bitmap_position;

    /**
     * @brief Drawing text to the screen
     *
     * @param xy_lup upper left corner of the text
     * @param color color to draw the text in
     * @param text The text that needs to be drawn
     * @param fontname The selected font in which the text needs to be drawn
     * @param fontsize Size of the font
     * @param fontstyle Style (Italic, normal, etc..)
     * @param reserved Currently unused. Reserved for future use.
     *
     * @return 0 if succesfull, otherwise -1 if error occured
     */
    int API_draw_text(int x_lup, int y_lup, int color, char* text, char* fontname, int fontsize, int fontstyle, int reserved);

    /**
     * @brief Drawing line
     *
     * @param x01 X coordinates of the points
     * @param y01 Y coordinates of the points
     * @param color Color of the line
     * @param weight Thickness of the corresponding line
     * @param reserved Currently unused. Reserved for future use.
     *
     * @return -1 if error occured, otherwise 0 (Also a call to Logger callback is sent if ptr isn't NULL)
     */
    int API_draw_line(int x0, int y0, int x1, int y1, int color, int weight, int reserved);

    /**
     * @brief Drawing rectangle on the screen
     *
     * @param xy upper left corner of the rectangle
     * @param width width of the rectangle
     * @param height height of the rectangle
     * @param color Color of the rectangle
     * @param filled Fill in the rectangle
     * @param Weight Weight of the lines in the unfilled rectangle. Unused in filled
     * @param reserved1 Currently unused. Reserved for future use.
     *
     * @return 0 if succesfull, otherwise -1 if error occured
     */
    int API_draw_rectangle(int x, int y, int width, int height, int color, int filled, int weight, int reserved);

    /**
     * @brief draws a bitmap
     *
     * @param bitmap pointer to the bitmap array
     * @param x_lup x position of the top left corner of the bitmap
     * @param y_lup y position of the top left corner of the bitmap
     *
     * @return bitmap position struct
     */
    int API_draw_bitmap(int bm_nr, int x_lup, int y_lup);

    /**
     * @brief clear screen to certain color
     *
     * @param color color that is used to set the background and overwrite all previous images on the screen
     */
    int API_clearscreen(int color);

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
    int API_draw_circle(int x, int y, int radius, int color, int filled);

    /**
     * @brief Waiting to next command
     *
     * @param msecs milliseconds to wait
     *
     * @return 0 if wait was succesful, -1 if the timer had failed to count
     */
    int API_wait(int msecs);

    /**
     * @brief Register a callback that can be used to transfer messages from this layer to other layers. Otherwise logger functionality it is disregarded and not used.
     *
     * @param pFunction Function pointer with a const char* for the string and int length for the length of the string to prevent buffer overflows.
     *
     * @return Integer 0 if succesfull otherwise -1
     */
    int API_register_logger_callback(void (*pFunction)(const char* string, int len));

    /**
     * @brief Get the current Tick count
     *
     * @param None
     *
     * @return uint64_t current Tick
     */
    uint64_t API_get_tick(void);

    /**
     * @brief Enabling systick for the timekeeping
     *
     * @param None
     *
     * @return None
     */
    void API_InitTick(void);

    /**
     * @brief Initialize the system with Clocks and DMA for the Screen signal generation
     *
     * @param None
     *
     * @return None
     */
    void API_Init(void);

    void API_set_pixel(int, int, uint8_t);

    /**
     * @brief Supported colors for the VGA screen
     */
#define VGA_COL_BLACK 0x00
#define VGA_COL_BLUE 0x03
#define VGA_COL_GREEN 0x1C
#define VGA_COL_RED 0xE0
#define VGA_COL_WHITE 0xFF
#define VGA_COL_CYAN 0x1F
#define VGA_COL_MAGENTA 0xE3
#define VGA_COL_YELLOW 0xFC

    extern uint64_t Tick;
#ifdef __cplusplus
}
#endif

#endif // API_GRAPHICS_H