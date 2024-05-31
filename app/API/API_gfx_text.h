#include "API_fonts.h"

#include "fonts/Adafruit_GFX.h"

#include <stdint.h>

#include <functional>
#include <memory>
#include <string>
#include <vector>

class API_gfx_text
{
private:
    int m_text_size = 1;
    int m_xpos = 0, m_ypos = 0, m_cursor_x = 0, m_cursor_y = 0;
    int m_x_max = 0, m_y_max = 0;
    uint8_t m_color = 0;

    std::unique_ptr<GFXfont> m_selected_font = nullptr;
    std::vector<GFXfont> m_fonts = { FreeMono9pt7b, FreeMonoBold9pt7b, FreeMonoBoldOblique9pt7b, FreeMonoOblique9pt7b };

    std::function<void(std::string)> m_logger;

    /**
     * @brief Drawing the character. Position is determined by the cursor set and previous characters already drawn.
     *
     * @param c Character to be drawn
     *
     * @return 1 if the character is drawn but wrapped around to the cursor start position. 0 If no exceptions were found and -1 if an unrecoverable error occurred.
     */
    int draw_character(const char c);

public:
    /**
     * @brief Default constructor
     *
     * @param xy_max maximum of the screen. (Bounding box)
     */
    API_gfx_text(int x_max, int y_max, std::function<void(std::string)> logger)
    {
        m_x_max = x_max;
        m_y_max = y_max;
        m_logger = logger;
    }

    /**
     * @brief Default destructor
     */
    ~API_gfx_text() { }

    /**
     * @brief Registering callback for sending logging messages
     *
     * @param Function Callable function that handles the logging messages
     *
     * @return None
     */
    void register_logging_callback(std::function<void(std::string)> logger) { m_logger = logger; }

    /**
     * @brief Set the cursor to where the characters need to be drawn
     *
     * @param xy cursor position
     *
     * @return none
     */
    void set_cursor(int x, int y)
    {
        m_cursor_x = x;
        m_cursor_y = y;

        m_xpos = x;
        m_ypos = y;
    }

    /**
     * @brief Selecting the font
     *
     * @param fontname Name of the chosen font
     * @param fontstyle Style (Italic, normal, etc..)
     *
     * @return 0 if succesfull, otherwise -1
     */
    int select_font(std::string fontname, int fontstyle);

    /**
     * @brief Setting the scaling number for the text (E.g. size of 2 = 2x scaling of pixels)
     *
     * @param size Scaling number
     *
     * @return none
     */
    void set_font_size(int size) { m_text_size = size; };

    /**
     * @brief Setting the text color to be drawn
     *
     * @param color Color of the text
     *
     * @return none
     */
    void set_text_color(uint8_t color) { m_color = color; };

    /**
     * @brief Getting the name of the selected font
     *
     * @param None
     *
     * @return The name of the font, In case no font is selected "Unkown" is returned
     */
    unsigned char* get_selected_font_name(void)
    {
        if (m_selected_font != nullptr)
        {
            return m_selected_font->name;
        }

        return (unsigned char*)"Unkown";
    }

    friend API_gfx_text& operator<<(API_gfx_text& api, const char* text)
    {
        if (text == NULL)
        {
            return api;
        }

        while (*text) // Unsafe method of writing the string.
                      // As the << operator only allows single argument
                      // next to the corresponding Class or struct for which this operation is performed.
                      // Therefore this is the only possible method.
        {
            api.draw_character(*text);
        }
        return api;
    }

    friend API_gfx_text& operator<<(API_gfx_text& api, std::string text)
    {
        for (size_t i = 0; i < text.size(); i++)
        {
            api.draw_character(text[i]);
        }

        return api;
    }
};