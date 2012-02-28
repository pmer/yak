#include <netdb.h>      /* gethostbyname, other Internet functions */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>     /* memset */
#include <unistd.h>

#include "bool.h"
#include "diagnostic.h"
#include "socket.h"

#define IRC_MSG_MAX_LEN 512 /* defined in RFC 2818 2.3.1 */

static int sock;

void establish_connection(char *host, int port)
{
	struct sockaddr_in addr;
	struct hostent *he;

	info("connecting to irc://%s:%d", host, port);

	he = gethostbyname(host);
	if (!he) {
		perror("gethostbyname");
		safe_shutdown_and_die(1);
	}
	endhostent();

	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		perror("socket");
		safe_shutdown_and_die(1);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr = *(struct in_addr*)he->h_addr_list[0];
	addr.sin_port = htons(port);
	if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		perror("connect");
		safe_shutdown_and_die(1);
	}

	info("connected");

	add_shutdown_fn(close_connection);
}

void close_connection()
{
	close(sock);
}

void sock_sendline(char *format, ...)
{
	va_list args;
	char buffer[IRC_MSG_MAX_LEN];

	/* log */
	printf("> ");
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	putchar('\n');

	/* send */
	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);

	if (
		send(sock, buffer, strlen(buffer), 0) == -1 ||
		send(sock, "\n", 1, 0) == -1
	) {
		perror("send");
		safe_shutdown_and_die(1);
	}
}

/**
 * sock_drainline - drain chars from the socket until we hit '\n'
 */
static void sock_drainline()
{
	int ret;
	char c = 0;

	while (c != '\n') {
		ret = recv(sock, &c, 1, 0);
		if (ret == -1) {
			perror("recv");
			safe_shutdown_and_die(1);
		}
		else if (ret == 0) {
			info("server closed the socket, quitting");
			safe_shutdown_and_die(0);
		}
	}
}

char *sock_readline()
{
	static char line[IRC_MSG_MAX_LEN];
	int ret, i = 0;
	char c = 0;

	while (c != '\n') {
		ret = recv(sock, &c, 1, 0);
		if (ret == -1) {
			perror("recv");
			safe_shutdown_and_die(1);
		}
		else if (ret == 0) {
			info("server closed the socket, quitting");
			safe_shutdown_and_die(0);
		}
		else if (c == '\r')
			; /* passive-agressively ignore :-) */
		else
			line[i++] = c;

		if (i == IRC_MSG_MAX_LEN) {
			err("server sent line >%d chars, forbidden by RFC, "
				"ignoring excess", IRC_MSG_MAX_LEN);
			sock_drainline();
		}
	}

	line[i-1] = '\0';
	printf("< %s\n", line);
	return line;
}

