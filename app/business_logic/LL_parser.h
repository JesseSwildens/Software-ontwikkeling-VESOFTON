#ifndef LL_PARSER_H
#define LL_PARSER_H

#ifdef __cplusplus
#include <string>
#include <vector>
#endif

#define STORAGE_SIZE_REPEAT_COMMANDS 50

enum commands
{
    nocommand = 0,
    clearscherm = 1,
    lijn = 2,
    rechthoek = 3,
    tekst = 4,
    bitmap = 5,
    wacht = 6,
    herhaal = 7,
    cirkel = 8,
};

#ifdef __cplusplus
extern "C"
{
#endif
    char ll_function();

#ifdef __cplusplus
}
void ll_handle_commands(enum commands command, std::vector<std::string> tokens);
enum commands ll_get_command(std::string commandString);
std::vector<std::string> ll_tokenize(std::string line);
std::string BL_remove_white_space(std::string str);
void BL_save_repeat_commands(std::string str);
#endif
#endif
