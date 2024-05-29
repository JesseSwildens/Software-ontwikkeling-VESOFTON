#ifndef BL_CALLBACKS_H
#define BL_CALLBACKS_H

#include <string>
#include <vector>

#ifndef __FILE_NAME__
#define __FILE_NAME__ "testfile"
#endif

#define log_message(message) (BL_base_log_message(message, __LINE__, __FILE_NAME__))

int BL_clearscherm(std::vector<std::string> tokens);
int BL_lijn(std::vector<std::string> tokens);
int BL_istype(std::string s);

void BL_base_log_message(std::string, int, std::string);

#endif
