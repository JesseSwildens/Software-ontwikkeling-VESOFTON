#include "API_fonts.h"

#include <stdint.h>

#include <string>
#include <vector>

class API_gfx_text
{
private:
    int m_text_size = 1;
    int m_xpos = 0, m_ypos = 0;
    uint8_t m_color = 0;
    GFXfont* m_selected_font = nullptr;
    std::vector<GFXfont> m_filteredFonts;
    std::vector<GFXfont> m_fonts = { FreeMono9pt7b, FreeMonoBold9pt7b, FreeMonoBoldOblique9pt7b, FreeMonoOblique9pt7b };

public:
    API_gfx_text() { }
    ~API_gfx_text() { }

    void set_cursor(int x, int y)
    {
        m_xpos = x;
        m_ypos = y;
    }

    int select_font(std::string, int);
    void set_font_size(int size) { m_text_size = size; };
    void set_text_color(uint8_t color) { m_color = color; };

    unsigned char* get_selected_font_name(void)
    {
        if (m_selected_font != nullptr)
        {
            return m_selected_font->name;
        }
        return (unsigned char*)"Unkown";
    }

    friend API_gfx_text& operator<<(API_gfx_text& api, char* text)
    {
        (void)text;
        return api;
    }

    friend API_gfx_text& operator<<(API_gfx_text& api, std::string text)
    {
        (void)text;
        return api;
    }
};