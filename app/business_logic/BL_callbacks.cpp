#include "BL_callbacks.h"
#include "API_simple_shapes.h"
#include "CHAL.h"
#include "LL_parser.h"
#include "bitmap.h"
#include "bitmap_calib_large.h"
#include "bitmap_dvd.h"
#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <queue>
#include <type_traits>
#include <unordered_map>
#include <variant>

// #define BL_DEBUG_COMMANDS
#define ECHO_REPEATS

using namespace std;
// Define possible argument types
using ArgType = std::variant<int, std::string>;
extern std::deque<std::string> previous_commands_q;

struct CommandTemplate
{
    std::string name;
    std::vector<std::string> expectedTypes;
};

bool isNumber(const std::string& s)
{
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

// Helper function to determine the type of an argument
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

void BL_base_log_message(std::string str, int line, std::string filename)
{
    CHAL_UART2_SendString("[" + filename + ":" + std::to_string(line) + "] " + str + "\n");
}

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
        { "geel", VGA_COL_YELLOW }
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

int BL_rechthoek(vector<string> tokens)
{
#ifdef BL_DEBUG_COMMANDS
    log_message("rechthoek command");
#endif
    CommandTemplate rechthoekTemplate = { "lijn", { "0", "0", "0", "0", std::string(), "0" } };

    if (!validateArguments(tokens, rechthoekTemplate))
    {
        log_message("error: invalid arguments for rechthoek command");
        return -1;
    }

    auto color = BL_get_valid_color(tokens[5]);
    API_draw_rectangle(stoi(tokens[1]), stoi(tokens[2]), stoi(tokens[3]), stoi(tokens[4]), color, stoi(tokens[6]), 1, 1);
    return 0;
}

int BL_cirkel(vector<string> tokens)
{
#ifdef BL_DEBUG_COMMANDS
    log_message("cirkel command");
#endif
    CommandTemplate cirkelTemplate = { "lijn", { "0", "0", "0", std::string() } };

    if (!validateArguments(tokens, cirkelTemplate))
    {
        log_message("error: invalid arguments for cirkel command");
        return -1;
    }

    auto color = BL_get_valid_color(tokens[4]);
    API_draw_circle(stoi(tokens[1]), stoi(tokens[2]), stoi(tokens[3]), color, 0);
    return 0;
}

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
    void* bitmap = BL_get_valid_bitmap(stoi(tokens[3]));
    if (bitmap != nullptr)
        API_VGA_DrawBitmap((unsigned char*)bitmap, 32, 32, stoi(tokens[1]), stoi(tokens[2]));
    else
        return -1;
    return 0;
}

// it now also repeats errors :), every error should exclude from queue
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

void* BL_get_valid_bitmap(int bitmap)
{
    void* bitmaps[] = {
        const_cast<unsigned char*>(bitmap_calib_large),
        bitmap_dvd,
        bitmap_calib
    };

    if (bitmap < 0 || bitmap >= int(sizeof(bitmaps) / sizeof(bitmaps[0])))
    {
        log_message("Invalid bitmap index");
        return nullptr;
    }

    return bitmaps[bitmap];
}

int BL_istype(string s)
{
    for (int i = 0; i < (int)s.length(); i++)
    {
        if (isdigit(s[i]))
        {
            return 1;
        }
    }
    return 0;
}
