#include "LL_parser.h"
#include "ASM_CHAL.h"
#include "BL_callbacks.h"
#include "BL_video_streaming.h"
#include "CHAL.h"

#include <bits/stdc++.h>
#include <cstdio>
#include <iostream> // std::cin, std::
#include <math.h>
#include <queue>
#include <sstream>

#define BUFFER_SIZE 16384

using namespace std;

extern std::queue<std::string> incoming_commands_q;
std::deque<std::string> previous_commands_q;
uint8_t rx_buff[BUFFER_SIZE];
extern uint8_t tempMainBuffer[BUFFER_SIZE];
extern uint8_t eventflagUART;
float test = 0;
char video_stream_flag = 0;
extern int offset;

// #define DEBUG_LL

enum commands ll_convert_command(string str);

// todo change "2" to VIDEOSTREAM
char BL_main_parser()
{
    if (video_stream_flag == 0)
    {
        CHAL_push_to_q(rx_buff, BUFFER_SIZE); // pushes incomng uart into a queue
        while (!incoming_commands_q.empty())
        {
            char tmp = BL_parse_queue(incoming_commands_q);
            incoming_commands_q.pop();
            if (tmp == 2)
            {
                CHAL_disable_DMA(DMA1_Stream5); // to change the NDTR register the DMA NEEDS to be disabled first.
                DMA1_Stream5->NDTR = BUFFER_SIZE; // reset RX-buff pointer to start
                CHAL_enable_DMA(DMA1_Stream5); // restart the DMA for UART reception
                memset(rx_buff, 0, BUFFER_SIZE); // reset rx_buff for new reception
                break;
            }
        }
    }
    else
    {
        // offset = 0; // set pointer of tempmainbuffer to zero for every new line
        ASMCHAL_event_call_back(rx_buff, BUFFER_SIZE);
        BL_video_stream(tempMainBuffer, BUFFER_SIZE);
        CHAL_disable_DMA(DMA1_Stream5); // to change the NDTR register the DMA NEEDS to be disabled first.
        DMA1_Stream5->NDTR = BUFFER_SIZE; // reset RX-buff pointer to start
        CHAL_enable_DMA(DMA1_Stream5); // restart the DMA for UART reception
        memset(rx_buff, 0, BUFFER_SIZE); // reset rx_buff for new reception
        memset(tempMainBuffer, 0, BUFFER_SIZE); // reset rx_buff for new reception
    }

    return 0;
}

char BL_parse_queue(std::queue<std::string>& q)
{
    std::string commandString = q.front();
    int tmp = BL_parse_single_string(commandString);
    if (tmp == -1)
        q.pop();
    if (tmp == 2)
        return 2;

    // logic for the repeat function
    if ((q == incoming_commands_q) && (ll_get_command(commandString) != herhaal))
        BL_save_repeat_commands(commandString);

    return 0;
}

int BL_parse_single_string(std::string str)
{
    commands command = nocommand;
    std::string commandString = str;
    if (!commandString.empty())
    {
        command = ll_get_command(commandString);
        if (command == nocommand)
        {
            log_message("invalid command on string: " + commandString);
            return -1;
        }
        if (command == hyperdrive)
        {
            BL_hyperdrive();
            video_stream_flag = 1;
            return 2;
        }
        vector<string> tokens = ll_tokenize(commandString);
        ll_handle_commands(command, tokens);
    }
    return 0;
}

void BL_save_repeat_commands(std::string str)
{
    if (!str.empty())
    {
        previous_commands_q.push_front(str);
        if (previous_commands_q.size() > STORAGE_SIZE_REPEAT_COMMANDS)
            previous_commands_q.pop_back();
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
    { .cmd = wacht, .function = BL_wacht },
};

void ll_handle_commands(enum commands command, vector<string> tokens)
{
    for (auto cmd : cmdhandler)
    {
        if (cmd.cmd == command)
            cmd.function(tokens);
    }
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
    if (str == "hyperdrive")
        return hyperdrive;
    else
        return nocommand;
}
