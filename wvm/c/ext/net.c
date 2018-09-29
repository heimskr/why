/******************************************************
 * Defines operations allowing socket communications. *
 ******************************************************/

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include "../wvm.h"
#include "net.h"

int xn_socket = -1;
bool xn_connected = false;

void op_xn_init(word instruction) {
	xn_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	xn_connected = false;
	INC();
}

void op_xn_connect(word instruction) {
	// TODO: implement exceptions :~)
	RREGS();

	struct addrinfo *result;
	struct sockaddr_in *addr;

	if (xn_socket == -1)
		DIE("\033[31mERROR\033[0m: Networking not initialized.\n");
	
	char *addr_str = wvm_get_string(rsv);

	printf("Connecting to \033[1m%s:%d\033[0m.\n", addr_str, (uint16_t) rtv);

	int error = getaddrinfo(addr_str, NULL, NULL, &result);
	free(addr_str);

	if (error == EAI_SYSTEM) {
		perror("getaddrinfo");
		exit(1);
	} else if (error) {
		fprintf(stderr, "\033[31mERROR\033[0m: Unable to parse address. %s\n", strerror(error));
		exit(1);
	}

	if (result == NULL)
		DIE("\033[31mERROR\033[0m: Unable to resolve address.\n");

	addr = (struct sockaddr_in *) result->ai_addr;
	addr->sin_port = htons((uint16_t) rtv);

	error = connect(xn_socket, (struct sockaddr *) addr, sizeof(struct sockaddr));
	if (error) {
		fprintf(stderr, "\033[31mERROR\033[0m: Unable to connect (%d). %s\n", errno, strerror(errno));
		exit(1);
	}

	free(result);
	free(addr);

	xn_connected = true;
	INC();
}

void op_xn_send(word instruction) {
	RREGS();

	if (!xn_connected)
		DIE("\033[31mERROR\033[0m: Socket not connected.\n");

	char *str = wvm_get_string(rsv);
	int len = strlen(str);

	// printf("Sending \"%s\" (%d)\n", str, len);

	send(xn_socket, str, len, 0);
	free(str);



	INC();
}

void op_xn_recv(word instruction) {
	RREGS();

	if (xn_socket == -1)
		DIE("\033[31mERROR\033[0m: Networking not initialized.\n");

	int max = rsv;
	word dest = rdv;
	char *buf = calloc(max + 1, sizeof(char));

	int bytes = recv(xn_socket, buf, max, 0);
	if (bytes == -1) {
		DIE("\033[31mERROR\033[0m: Couldn't receive from socket.\n");
	}

	int i;
	for (i = 0; buf[i] != '\0' && i < max; i++) {
		memory[dest + i] = buf[i];
	}

	memory[dest + i] = '\0';

	INC();
}
