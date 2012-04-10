#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "bool.h"
#include "diagnostic.h"
#include "socket.h"

#define IRC_MSG_MAX_LEN 512 /* defined in RFC 2818 2.3.1 */

static int sock;
SSL *ssl;
FILE *socklog;

void establish_connection(char *host, char *port, char *ssl_method)
{
	struct addrinfo hints, *servinfo, *p;
	SSL_CTX *ctx;
	int rv;

	if (ssl_method != NULL) {
		SSL_library_init();
		SSL_load_error_strings();
	}

	info("connecting to %s://%s:%s", ssl_method == NULL ? "irc" : "ircs", host, port);

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

		if (ssl_method != NULL) {
			/* attempt to establish a secure connection */
			if (strcasecmp(ssl_method, "sslv2") == 0) {
				ctx = SSL_CTX_new(SSLv2_method());
			} else if (strcasecmp(ssl_method, "sslv3") == 0) {
				ctx = SSL_CTX_new(SSLv3_method());
			} else if (strcasecmp(ssl_method, "tlsv1") == 0) {
				ctx = SSL_CTX_new(TLSv1_method());
			} else if (strcasecmp(ssl_method, "sslv23") == 0) {
				ctx = SSL_CTX_new(SSLv23_method());
			} else {
				err("unknown secure connection method: %s", ssl_method);
				safe_shutdown_and_die(1);
			}

			if (ctx == NULL) {
				ERR_print_errors_fp(stderr);
				close(sock);
				continue;
			}

			ssl = SSL_new(ctx);
			if (ssl == NULL) {
				ERR_print_errors_fp(stderr);
				close(sock);
				continue;
			}

			if (SSL_set_fd(ssl, sock) != 1) {
				ERR_print_errors_fp(stderr);
				close(sock);
				continue;
			}

			if (SSL_connect(ssl) != 1) {
				ERR_print_errors_fp(stderr);
				close(sock);
				continue;
			}

			SSL_CTX_free(ctx);
		}
		
		break;
	}

	if (p == NULL) {
		/* connection failed */
		err("connection failed");
		safe_shutdown_and_die(1);
	}

	if (ssl_method == NULL)
		ssl = NULL;

	info("connected");
	freeaddrinfo(servinfo);
	add_shutdown_fn(close_connection);
}

void close_connection()
{
	if (ssl != NULL) {
		SSL_shutdown(ssl);
		SSL_free(ssl);
	}
	close(sock);
}

void sock_send(const void *buffer, size_t length)
{
	if (ssl == NULL) {
		if (send(sock, buffer, length, 0) == -1) {
			perror("send");
			safe_shutdown_and_die(1);
		}
	} else {
		if (SSL_write(ssl, buffer, length) <= 0) {
			ERR_print_errors_fp(stderr);
			safe_shutdown_and_die(1);
		}
	}
}

ssize_t sock_recv(void *buffer, size_t length)
{
	if (ssl == NULL)
		return recv(sock, buffer, length, 0);
	else
		return SSL_read(ssl, buffer, length);
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
	
	sock_send(buffer, strlen(buffer));
	sock_send("\n", 1);
}

/**
 * sock_drainline - drain chars from the socket until we hit '\n'
 */
static void sock_drainline()
{
	int ret;
	char c = 0;

	while (c != '\n') {
		ret = sock_recv(&c, 1);
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
		ret = sock_recv(&c, 1);
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
