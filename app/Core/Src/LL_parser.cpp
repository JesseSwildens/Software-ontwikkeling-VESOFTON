#include "LL_parser.h"
#include "CHAL.h"
#include "stm32_ub_vga_screen.h"

#include <bits/stdc++.h>
#include <cstdio>
#include <iostream> // std::cin, std::
#include <math.h>
#include <queue>
#include <sstream>
#include <vector>

using namespace std;

extern std::queue<std::string> incoming_commands_q;
float test = 0;

// #define DEBUG_LL
#define PI 3.1415926
#define MAX_CIRCLE_POINTS 100

enum commands ll_convert_command(string str);

char ll_function()
{
    commands command = ll_get_command();
    if (command == nocommand)
    {
        CHAL_UART2_SendString((char*)"unknown command\n");
        return -1;
    }
    ll_handle_commands(command);
    return 0;
}

void ll_handle_commands(enum commands command)
{
    switch (command)
    {
    case (nocommand):
    {
        CHAL_UART2_SendString((char*)"unknown command\n");
        break;
    }
    case (clearscherm):
    {
        CHAL_UART2_SendString((char*)"clearscherm command\n");
        break;
    }
    case (lijn):
    {
        CHAL_UART2_SendString((char*)"lijn command\n");
        break;
    }

    case (rechthoek):
    {
        CHAL_UART2_SendString((char*)"rechthoek command\n");
        break;
    }

    case (tekst):
    {
        CHAL_UART2_SendString((char*)"tekst command\n");
        break;
    }

    case (bitmap):
    {
        CHAL_UART2_SendString((char*)"bitmap command\n");
        break;
    }

    case (wacht):
    {
        CHAL_UART2_SendString((char*)"wacht command\n");
        break;
    }

    case (herhaal):
    {
        CHAL_UART2_SendString((char*)"herhaal command\n");
        break;
    }

    case (cirkel):
    {
        CHAL_UART2_SendString((char*)"cirkel command\n");
        break;
    }
    }
}
commands ll_get_command()
{
    // while (!incoming_commands_q.empty())
    // {

    std::string commandString = incoming_commands_q.front();

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

    incoming_commands_q.pop();
    // }
    commands command = ll_convert_command(firstWord);
    return command;
}

void ll_tokenize(string line)
{
    vector<string> tokens;
    stringstream check(line);
    string intermediate;
    while (getline(check, intermediate, ','))
    {
        tokens.push_back(intermediate);
    }
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
