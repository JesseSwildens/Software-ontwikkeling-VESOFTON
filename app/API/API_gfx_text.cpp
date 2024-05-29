#include "API_gfx_text.h"

int API_gfx_text::select_font(std::string fontname, int fontstyle)
{

    for (auto iterator = m_fonts.begin(); auto font : m_fonts)
    {
        if (fontname.find((char*)font.name) != std::string::npos)
        {
            m_filteredFonts.push_back(font);
        }

        iterator++;
    }

    if ((m_filteredFonts.size() == 0) || (m_filteredFonts.size() < (unsigned int)fontstyle))
    {
        return -1;
    }

    m_selected_font = &m_filteredFonts[fontstyle];

    return 0;
}