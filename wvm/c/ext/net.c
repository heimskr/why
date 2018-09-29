/******************************************************
 * Defines operations allowing socket communications. *
 ******************************************************/

#include <netdb.h>
#include <stdio.h>
#include "../wvm.h"
#include "net.h"

int xn_socket = -1;
bool xn_connected = false;

void op_xn_init(word instruction) {
	printf("Attempting op_xn_init.\n");
	xn_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	xn_connected = false;
	INC();

	printf("op_xn_init successful.\n");
}

void op_xn_connect(word instruction) {
	// TODO: implement exceptions :~)
	RREGS();

	printf("Attempting op_xn_connect.\n");

	struct addrinfo *result;
	struct sockaddr_in *addr;

	if (xn_socket == -1)
		DIE("Networking not initialized.\n");
	
	char *addr_str = wvm_get_string(rsv);

	int error = getaddrinfo(addr_str, NULL, NULL, &result);
	free(addr_str);

	if (error == EAI_SYSTEM) {
		perror("getaddrinfo");
		exit(1);
	} else if (error)
		DIE("Unable to parse address.\n");

	if (result == NULL)
		DIE("Unable to resolve address.\n");

	addr = (struct sockaddr_in *) result->ai_addr;
	addr->sin_port = htons((uint16_t) rtv);
	freeaddrinfo(result);

	if (connect(xn_socket, (struct sockaddr *) addr, sizeof(struct sockaddr_in)))
		DIE("Unable to connect.\n");

	free(addr);

	printf("op_xn_connect successful.\n");

	xn_connected = true;

	INC();
}

void op_xn_send(word instruction) {
	RREGS();

	printf("Attempting op_xn_send.\n");

	if (!xn_connected)
		DIE("Socket not connected.\n");

	char *str = wvm_get_string(rsv);
	int len = strlen(str);

	send(xn_socket, str, len, 0);
	free(str);

	INC();
}
