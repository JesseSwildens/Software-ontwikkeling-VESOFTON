#include "LL_parser.h"
#include "API_simple_shapes.h"
#include "BL_callbacks.h"
#include "CHAL.h"
#include "stm32_ub_vga_screen.h"

#include <bits/stdc++.h>
#include <cstdio>
#include <iostream> // std::cin, std::
#include <math.h>
#include <queue>
#include <sstream>

using namespace std;

extern std::queue<std::string> incoming_commands_q;
std::queue<std::string> previous_commands_q;
float test = 0;

// #define DEBUG_LL
#define PI 3.1415926
#define MAX_CIRCLE_POINTS 100

enum commands ll_convert_command(string str);

char BL_main_parser()
{
    while (!incoming_commands_q.empty())
    {
        BL_parse_queue(incoming_commands_q);
        incoming_commands_q.pop();
    }

    return 0;
}

char BL_parse_queue(std::queue<std::string>& q)
{
    commands command = nocommand;
    std::string commandString = q.front();
    if (!commandString.empty())
    {
        command = ll_get_command(commandString);
        if (command == nocommand)
        {
            log_message("invalid command on string: " + commandString);
            q.pop();
            return -1;
        }
        else
        {
            vector<string> tokens = ll_tokenize(commandString);
            ll_handle_commands(command, tokens);
        }
    }

    // logic for the repeat function
    if ((q == incoming_commands_q) && (command != herhaal))
        BL_save_repeat_commands(commandString);

    return 0;
}

void BL_save_repeat_commands(std::string str)
{
    if (!str.empty())
    {
        previous_commands_q.push(str);
        if (previous_commands_q.size() > STORAGE_SIZE_REPEAT_COMMANDS)
            previous_commands_q.pop();
    }
}

typedef struct
{
    commands cmd;
    std::function<int(vector<string>)> function;
} cmd_struct_t;

std::vector<cmd_struct_t> cmdhandler = {
    { .cmd = clearscherm, .function = BL_clearscherm },
    { .cmd = lijn, .function = BL_lijn },
    { .cmd = rechthoek, .function = BL_rechthoek },
    { .cmd = cirkel, .function = BL_cirkel },
    { .cmd = bitmap, .function = BL_bitmap },
    { .cmd = herhaal, .function = BL_herhaal },
};

void ll_handle_commands(enum commands command, vector<string> tokens)
{
    for (auto cmd : cmdhandler)
    {
        if (cmd.cmd == command)
            cmd.function(tokens);
    }

    // case (tekst):
    // {
    //     CHAL_UART2_SendString("tekst command\n");
    //     break;
    // }

    // case (wacht):
    // {
    //     CHAL_UART2_SendString("wacht command\n");
    //     break;
    // }

    // case (herhaal):
    // {
    //     CHAL_UART2_SendString("herhaal command\n");
    //     break;
    // }
}
commands ll_get_command(std::string commandString)
{

    // Find the position of the first comma (if any)
    size_t commaPos = commandString.find(',');

    // Extract the first word before the comma (or the entire string if no comma)
    std::string firstWord = commandString.substr(0, commaPos);

    // Trim leading and trailing whitespaces
    size_t start = firstWord.find_first_not_of(" \t");
    size_t end = firstWord.find_last_not_of(" \t");
    if (start != std::string::npos && end != std::string::npos)
    {
        firstWord = firstWord.substr(start, end - start + 1);
    }

#ifdef DEBUG_LL
    const char* back_command_cstr = firstWord.c_str();
    CHAL_UART2_SendString((char*)back_command_cstr);
#endif

    commands command = ll_convert_command(firstWord);
    return command;
}

vector<string> ll_tokenize(std::string line)
{
    vector<string> tokens;
    stringstream check(line);
    string intermediate;
    while (getline(check, intermediate, ','))
    {
        tokens.push_back(BL_remove_white_space(intermediate));
    }
    return tokens;
}

string BL_remove_white_space(string str)
{
    // Trim leading and trailing whitespaces
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");
    if (start != std::string::npos && end != std::string::npos)
    {
        str = str.substr(start, end - start + 1);
    }
    return str;
}

enum commands ll_convert_command(string str)
{
    if (str == "clearscherm")
        return clearscherm;
    if (str == "lijn")
        return lijn;
    if (str == "rechthoek")
        return rechthoek;
    if (str == "tekst")
        return tekst;
    if (str == "bitmap")
        return bitmap;
    if (str == "wacht")
        return wacht;
    if (str == "herhaal")
        return herhaal;
    if (str == "cirkel")
        return cirkel;
    else
        return nocommand;
}
