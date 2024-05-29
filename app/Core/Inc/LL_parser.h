#ifndef LL_PARSER_H
#define LL_PARSER_H

enum commands
{
    nocommand = 0,
    clearscherm = 1,
    lijn = 2,
    rechthoek = 3,
    tekst = 4,
    bitmap = 5,
    wacht = 6,
    herhaal = 7,
    cirkel = 8,
};

#ifdef __cplusplus
extern "C"
{
#endif
    void API_draw_text(int x_lup, int y_lup, int color, char* text, char* fontname,
        int fontsize, int fontstyle, int reserved);
    int API_draw_circle(int x0, int y0, int radius, int color, int reserved);
    enum commands ll_get_command();
    char ll_function();
    void ll_handle_commands(enum commands command);

#ifdef __cplusplus
}
#endif
#endif
