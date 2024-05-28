#ifndef LL_PARSER_H
#define LL_PARSER_H

#ifdef __cplusplus
extern "C"
{
#endif
    void API_draw_text(int x_lup, int y_lup, int color, char* text, char* fontname,
        int fontsize, int fontstyle, int reserved);
    int API_draw_circle(int x0, int y0, int radius, int color, int reserved);
    void function();

#ifdef __cplusplus
}
#endif
#endif
