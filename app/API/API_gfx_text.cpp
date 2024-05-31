#include "API_gfx_text.h"

#include "API_simple_shapes.h"

#include <iostream>

int API_gfx_text::select_font(std::string _fontname, int fontstyle)
{
    std::vector<GFXfont> m_filteredFonts;

    for (auto font : m_fonts)
    {
        std::string fontname((char*)font.name);
        if (fontname.find(_fontname) != std::string::npos)
        {
            m_filteredFonts.push_back(font);
        }
    }

    if ((m_filteredFonts.size() == 0) || (m_filteredFonts.size() < (unsigned int)fontstyle))
    {
        int size = m_filteredFonts.size();
        m_logger("Font style not found: " + std::to_string(size));
        return -1;
    }

    m_selected_font = std::make_unique<GFXfont>(m_filteredFonts[fontstyle]);

    return 0;
}

/**
 * @brief Drawing the character. Position is determined by the cursor set and previous characters already drawn.
 *
 * @param c Character to be drawn
 *
 * @return 1 if the character is drawn but wrapped around to the cursor start position. 0 If no exceptions were found and -1 if an unrecoverable error occurred.
 */
int API_gfx_text::draw_character(const char c)
{
    if (m_selected_font == nullptr)
    {
        m_logger("No font is selected");
        return -1;
    }

    if (c == '\n')
    {
        // Wrap around to the next line
        m_xpos = m_cursor_x;
        m_ypos += m_text_size * m_selected_font->yAdvance;
        return 0;
    }
    else if (c == '\r')
    {
        return 0; // '\r' is ignored as a character
    }

    if ((c < m_selected_font->first) || (c > m_selected_font->last))
    {
        m_logger("Unkown character: " + std::to_string(c));
        return -1; // Unkown character
    }

    int ret = 0;
    GFXglyph* char_glyph = m_selected_font->glyph + (c - m_selected_font->first);

    if ((m_xpos + ((int16_t)char_glyph->xOffset + char_glyph->width) * m_text_size) > m_x_max)
    {
        // Character bounds is outside the screen. Wrap around
        m_xpos = m_cursor_x;
        m_ypos += m_text_size * m_selected_font->yAdvance;
        ret = 1;
    }

    uint8_t bit = 0, bits = 0;
    uint16_t bitmap_offset_temp = char_glyph->bitmapOffset;

    for (uint8_t y = 0; y < char_glyph->height; y++)
    {
        for (uint8_t x = 0; x < char_glyph->width; x++)
        {
            if (!(bit++ & 7))
            {
                bits = m_selected_font->bitmap[bitmap_offset_temp++];
            }
            if (bits & 0x80)
            {
                API_draw_rectangle(m_xpos + ((int16_t)char_glyph->xOffset + x) * m_text_size, m_ypos + ((int16_t)char_glyph->yOffset + y) * m_text_size, m_text_size, m_text_size, m_color, 1, 0, 0);
            }
            bits <<= 1;
        }
    }

    m_xpos += char_glyph->xAdvance * m_text_size;

    return ret;
}