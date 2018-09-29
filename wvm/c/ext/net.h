#ifndef __EXT_NET_H__
#define __EXT_NET_H__

#include <arpa/inet.h>
#include <stdint.h>
#include <sys/socket.h>
#include "../ops.h"
#include "../instruction.h"

void op_xn_init(word instruction);
void op_xn_connect(word instruction);
void op_xn_send(word instruction);
void op_xn_recv(word instruction);

#endif
