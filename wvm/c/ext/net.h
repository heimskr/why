#ifndef EXT_NET_H_
#define EXT_NET_H_

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
