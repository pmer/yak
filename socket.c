#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "bool.h"
#include "diagnostic.h"
#include "socket.h"

#define IRC_MSG_MAX_LEN 512 /* defined in RFC 2818 2.3.1 */

static int sock;
FILE *socklog;

void establish_connection(char *host, char *port)
{
	struct addrinfo hints, *servinfo, *p;
	int rv;
	
	info("connecting to irc://%s:%s", host, port);

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	rv = getaddrinfo(host, port, &hints, &servinfo);
	if (rv != 0) {
		err("getaddrinfo: %s", gai_strerror(rv));
		safe_shutdown_and_die(1);
	}

	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("socket");
			continue;
		}

		if (connect(sock, p->ai_addr, p->ai_addrlen) == -1) {
			close(sock);
			perror("connect");
			continue;
		}

		break;
	}

	if (p == NULL) {
		/* connection failed */
		fprintf(stderr, "failed to connect\n");
		safe_shutdown_and_die(1);
	}

	info("connected");
	freeaddrinfo(servinfo);
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
	fprintf(socklog, "> ");
	va_start(args, format);
	vfprintf(socklog, format, args);
	va_end(args);
	putc('\n', socklog);
	fflush(socklog);

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
	fprintf(socklog, "< %s\n", line);
	fflush(socklog);
	return line;
}

void sock_init()
{
	socklog = stdout;
}
