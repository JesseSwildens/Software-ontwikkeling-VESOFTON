#include "BL_callbacks.h"
#include "API_graphics.h"
#include "BL_parser.h"
#include "CHAL.h"

#if __cplusplus
extern "C"
{
#include "arrow_down.h"
#include "arrow_left.h"
#include "arrow_right.h"
#include "arrow_up.h"
#include "bitmap.h"
#include "bitmap_dvd.h"
}
#endif

#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <queue>
#include <type_traits>
#include <unordered_map>
#include <variant>

#define SYSTICK_LOAD (SystemCoreClock / 1000000U)
#define SYSTICK_DELAY_CALIB (SYSTICK_LOAD >> 1)

/**
 * @brief Delay in us function using systick
 *
 */
#define DELAY_US(us)                                                \
    do                                                              \
    {                                                               \
        uint32_t start = SysTick->VAL;                              \
        uint32_t ticks = (us * SYSTICK_LOAD) - SYSTICK_DELAY_CALIB; \
        while ((start - SysTick->VAL) < ticks)                      \
            ;                                                       \
    } while (0)

/**
 * @brief Delay in ms function using systick
 *
 */
#define DELAY_MS(ms)                                \
    do                                              \
    {                                               \
        for (uint32_t i = 0; i < (uint32_t)ms; ++i) \
        {                                           \
            DELAY_US(1000);                         \
        }                                           \
    } while (0)

// #define BL_DEBUG_COMMANDS
#define ECHO_REPEATS

using namespace std;

using ArgType = std::variant<int, std::string>;
extern std::deque<std::string> previous_commands_q;

/**
 * @brief template for checking the layout of the command
 *
 */
struct CommandTemplate
{
    std::string name;
    std::vector<std::string> expectedTypes;
};

/**
 * @brief checks if sting is number
 *
 * @param s string to check
 *
 * @return 1 if number, 0 if not
 *
 */
bool isNumber(const std::string& s)
{
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

/**
 * @brief Helper function to determine the type of an argument
 *
 * @param arg string to check
 *
 * @return argtype
 */
ArgType getArgType(const std::string& arg)
{
    if (isNumber(arg))
    {
        return std::stoi(arg);
    }
    else
    {
        return arg;
    }
}

/**
 * @brief function to check the order and length of an incomming command
 *
 * @param tokens vector<strings> of tokens of an incomming command
 * @param cmdTemplate a template to check against for order of argtypes
 *
 * @return true if length and order matches the template
 */
bool validateArguments(const std::vector<std::string>& tokens, const CommandTemplate& cmdTemplate)
{
    if (tokens.size() != cmdTemplate.expectedTypes.size() + 1)
    { // +1 for the command name
        return false;
    }
    for (size_t i = 1; i < tokens.size(); ++i)
    { // Start from 1 to skip the command name
        ArgType actualArg = getArgType(tokens[i]);
        ArgType expectedArg = getArgType(cmdTemplate.expectedTypes[i - 1]);

        if (actualArg.index() != expectedArg.index())
        { // Compare the type indices
            return false;
        }
    }
    return true;
}

/**
 * @brief prints message + error origin
 *
 * @param str print string
 * @param line line origin of call
 * @param filename filename origin of call
 *
 * @return void
 */
void BL_base_log_message(std::string str, int line, std::string filename)
{
    CHAL_UART2_SendString("[" + filename + ":" + std::to_string(line) + "] " + str + "\n");
}

/**
 * @brief takes the color command string, processes it and returns the color integer associated with that color
 *
 * @param color check for valid colors
 *
 * @return if valid, return a color code, else return -1
 */
auto BL_get_valid_color(string color)
{
    // might not be beautifull but it is O(1) averge so it is really fast.
    static const std::unordered_map<std::string, int> color_map = {
        { "zwart", VGA_COL_BLACK },
        { "blauw", VGA_COL_BLUE },
        { "groen", VGA_COL_GREEN },
        { "rood", VGA_COL_RED },
        { "wit", VGA_COL_WHITE },
        { "cyan", VGA_COL_CYAN },
        { "magenta", VGA_COL_MAGENTA },
        { "geel", VGA_COL_YELLOW },
        { "lichtblauw", VGA_COL_LIGHT_BLUE },
        { "grijs", VGA_COL_GRAY },
        { "bruin", VGA_COL_BROWN },
        { "lichtcyaan", VGA_COL_LIGHT_CYAN },
    };

    auto it = color_map.find(color);
    if (it != color_map.end())
    {
        return (it->second);
    }
    else
    {
        log_message("error: invalid color argument");
        return -1;
    }
}

/**
 * @brief takes the fontstyle command string, processes it and returns the fontstyle integer associated with that fontstyle
 *
 * @param fontstyle check for valid fontstyle
 *
 * @return if valid, return a color code, else return -1
 */
auto BL_get_valid_fontstyle(string font_style)
{
    // might not be beautifull but it is O(1) averge so it is really fast.
    static const std::unordered_map<std::string, int> font_style_map = {
        { "normaal", 0 },
        { "cursief", 1 },
        { "vet", 2 },
    };

    auto it = font_style_map.find(font_style);
    if (it != font_style_map.end())
    {
        return (it->second);
    }
    else
    {
        log_message("error: invalid font_style argument");
        return -1;
    }
}

/**
 * @brief function for clearing the screen
 *
 * @param  tokens vector<strings> of tokens of an incomming command
 *
 * @return if invalid arguments, return -1 else return 0
 */
int BL_clearscherm(vector<string> tokens)
{
#ifdef BL_DEBUG_COMMANDS
    log_message("clearscherm command");
#endif
    CommandTemplate clearScreenTemplate = { "clearscherm", { std::string() } };

    if (!validateArguments(tokens, clearScreenTemplate))
    {
        log_message("error: invalid arguments for clearscherm command");
        return -1;
    }

    auto color = BL_get_valid_color(tokens[1]);
    API_clearscreen(color);
    return 0;
}

/**
 * @brief function for drawing line
 *
 * @param  tokens vector<strings> of tokens of an incomming command
 *
 * @return if invalid arguments, return -1 else return 0
 */
int BL_lijn(vector<string> tokens)
{
#ifdef BL_DEBUG_COMMANDS
    log_message("lijn command");
#endif
    CommandTemplate lijnTemplate = { "lijn", { "0", "0", "0", "0", std::string(), "0" } };

    if (!validateArguments(tokens, lijnTemplate))
    {
        log_message("error: invalid arguments for lijn command");
        return -1;
    }

    auto color = BL_get_valid_color(tokens[5]);
    API_draw_line(stoi(tokens[1]), stoi(tokens[2]), stoi(tokens[3]), stoi(tokens[4]), color, stoi(tokens[6]), 0);
    return 0;
}

/**
 * @brief function for drawing square
 *
 * @param  tokens vector<strings> of tokens of an incomming command
 *
 * @return if invalid arguments, return -1 else return 0
 */
int BL_rechthoek(vector<string> tokens)
{
#ifdef BL_DEBUG_COMMANDS
    log_message("rechthoek command");
#endif
    CommandTemplate rechthoekTemplate = { "rechthoek", { "0", "0", "0", "0", std::string(), "0" } };
    CommandTemplate rechthoekTemplateAlternate = { "rechthoek", { "0", "0", "0", "0", std::string(), "0", "0" } };

    bool val_arg_norm = (!validateArguments(tokens, rechthoekTemplate));
    bool val_arg_alt = (!validateArguments(tokens, rechthoekTemplateAlternate));

    if (val_arg_norm == val_arg_alt)
    {
        log_message("error: invalid arguments for rechthoek command");
        return -1;
    }

    auto color = BL_get_valid_color(tokens[5]);
    if (!val_arg_norm == true)
    {

        API_draw_rectangle(stoi(tokens[1]), stoi(tokens[2]), stoi(tokens[3]), stoi(tokens[4]), color, stoi(tokens[6]), 1, 1);
    }
    else
    {
        API_draw_rectangle(stoi(tokens[1]), stoi(tokens[2]), stoi(tokens[3]), stoi(tokens[4]), color, stoi(tokens[6]), stoi(tokens[7]), 1);
    }
    return 0;
}

/**
 * @brief function for drawing circle
 *
 * @param  tokens vector<strings> of tokens of an incomming command
 *
 * @return if invalid arguments, return -1 else return 0
 */
int BL_cirkel(vector<string> tokens)
{
#ifdef BL_DEBUG_COMMANDS
    log_message("cirkel command");
#endif
    CommandTemplate cirkelTemplate = { "cirkel", { "0", "0", "0", std::string() } };
    CommandTemplate cirkelTemplateAlternate = { "cirkel", { "0", "0", "0", std::string(), "0" } };

    bool val_arg_norm = (!validateArguments(tokens, cirkelTemplate));
    bool val_arg_alt = (!validateArguments(tokens, cirkelTemplateAlternate));

    if (val_arg_norm == val_arg_alt)
    {
        log_message("error: invalid arguments for cirkel command");
        return -1;
    }

    auto color = BL_get_valid_color(tokens[4]);
    if (!val_arg_norm == true)
    {

        API_draw_circle(stoi(tokens[1]), stoi(tokens[2]), stoi(tokens[3]), color, 0);
    }
    else
    {
        API_draw_circle(stoi(tokens[1]), stoi(tokens[2]), stoi(tokens[3]), color, stoi(tokens[5]));
    }
    return 0;
}

/**
 * @brief function for drawing bitmap
 *
 * @param  tokens vector<strings> of tokens of an incomming command
 *
 * @return if invalid arguments, return -1 else return 0
 */
int BL_bitmap(vector<string> tokens)
{
#ifdef BL_DEBUG_COMMANDS
    log_message("bitmap command");
#endif
    CommandTemplate bitmapTemplate = { "bitmap", { "0", "0", "0" } };

    if (!validateArguments(tokens, bitmapTemplate))
    {
        log_message("error: invalid arguments for bitmap command");
        return -1;
    }
    // void* bitmap = BL_get_valid_bitmap(stoi(tokens[3]));
    // if (bitmap != nullptr)
    API_draw_bitmap(stoi(tokens[1]), stoi(tokens[2]), stoi(tokens[3]));
    // else return -1;
    return 0;
}

/**
 * @brief function for drawing bitmap
 *
 * @param  tokens vector<strings> of tokens of an incomming command
 *
 * @return if invalid arguments, return -1 else return 0
 */
int BL_tekst(vector<string> tokens)
{
#ifdef BL_DEBUG_COMMANDS
    log_message("tekst command");
#endif
    CommandTemplate tekstTemplate = { "tekst", { "0", "0", std::string(), std::string(), std::string(), "0", std::string() } };
    CommandTemplate tekstTemplateAlternate = { "tekst", { "0", "0", std::string(), std::string(), std::string(), "0", std::string(), "0" } };

    bool val_arg_norm = (!validateArguments(tokens, tekstTemplate));
    bool val_arg_alt = (!validateArguments(tokens, tekstTemplateAlternate));

    if (val_arg_norm == val_arg_alt)
    {
        log_message("error: invalid arguments for tekst command");
        return -1;
    }
    auto color = BL_get_valid_color(tokens[3]);
    auto font_style = BL_get_valid_fontstyle(tokens[7]);

    if (!val_arg_norm == true)
    {

        API_draw_text(stoi(tokens[1]), stoi(tokens[2]), color, (char*)tokens[4].c_str(), (char*)tokens[5].c_str(), stoi(tokens[6]), font_style, 0);
    }
    else
    {
        API_draw_text(stoi(tokens[1]), stoi(tokens[2]), color, (char*)tokens[4].c_str(), (char*)tokens[5].c_str(), stoi(tokens[6]), font_style, stoi(tokens[8]));
    }
    return 0;
}

/**
 * @brief function for repeating last command (MAX 50 commands)
 *
 * @param  tokens vector<strings> of tokens of an incomming command
 *
 * @return if invalid arguments, return -1 else return 0
 */
int BL_herhaal(std::vector<std::string> tokens)
{
#ifdef BL_DEBUG_COMMANDS
    log_message("herhaal command");
#endif
    CommandTemplate herhaalTemplate = { "herhaal", { "0", "0" } };

    // Validate the tokens length
    if (tokens.size() < 3)
    {
        log_message("error: not enough arguments for herhaal command");
        return -1;
    }

    int inner_loop_count = std::stoi(tokens[1]);
    int outer_loop_count = std::stoi(tokens[2]);

    if (inner_loop_count > static_cast<int>(previous_commands_q.size()))
    {
        log_message("error: not enough previous commands to repeat");
        return -1;
    }

    if (!validateArguments(tokens, herhaalTemplate) || inner_loop_count > STORAGE_SIZE_REPEAT_COMMANDS)
    {
        log_message("error: invalid arguments for herhaal command");
        return -1;
    }

    for (int j = 0; j < outer_loop_count; ++j)
    {
        for (int i = 0; i < inner_loop_count; ++i)
        {
            if (previous_commands_q.empty())
            {
                log_message("error: previous_commands_q is empty");
                return -1;
            }
            std::string front_command = previous_commands_q[(inner_loop_count - 1) - i];
#ifdef ECHO_REPEATS
            log_message("repeated command: " + front_command);
#endif
            BL_parse_single_string(front_command); // Process the command
        }
    }

    return 0;
}

/**
 * @brief function for waiting
 *
 * @param  tokens vector<strings> of tokens of an incomming command
 *
 * @return if invalid arguments, return -1 else return 0
 */
int BL_wacht(vector<string> tokens)
{
#ifdef BL_DEBUG_COMMANDS
    log_message("wacht command");
#endif
    CommandTemplate wachtTemplate = { "wacht", { "0" } };

    if (!validateArguments(tokens, wachtTemplate))
    {
        log_message("error: invalid arguments for cirkel command");
        return -1;
    }

    return API_wait(std::stoi(tokens[1]));
}
