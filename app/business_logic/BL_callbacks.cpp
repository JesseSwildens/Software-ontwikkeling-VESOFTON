#include "BL_callbacks.h"
#include "API_simple_shapes.h"
#include "CHAL.h"
#include <iostream>

using namespace std;

void BL_base_log_message(std::string str, int line, std::string filename)
{
    CHAL_UART2_SendString("[" + filename + ":" + std::to_string(line) + "] " + str + "\n");
}

int BL_clearscherm(vector<string> tokens)
{
    log_message("clearscherm command");
    if (tokens.size() > 2)
        log_message("error: wrong argument count for clearscherm command");
    if (BL_istype(tokens[1]))
        log_message("error: first argument of command is of incorrect type");
    API_VGA_FillScreen(0x49);
    return 0;
}

int BL_lijn(vector<string> tokens)
{
    log_message("lijn command");
    if (tokens.size() > 2)
    {
        log_message("error: wrong argument count for clearscherm command");
    }
    return 0;
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