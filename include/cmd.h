#ifndef _CMD_H_
#define _CMD_H_

#include "global.h"

void exec_cmd(CMD cmd);
CMD get_cmd(int argc, char** argv);

#endif // _CMD_H_