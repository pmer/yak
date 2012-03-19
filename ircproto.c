#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "diagnostic.h"
#include "socket.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

void ircproto_parse_message(char *msg, char **prefix, char **cmd,
		int *ncmd, char **params)
{
	char *spc;

	/* The IRC message specification accoording to RFC 2812 is:
	 *
	 * [ ":" prefix SPACE ] command [ params ] crlf
	 */

	/* prefix */
	if (*msg == ':') {
		spc = strchr(msg, ' ');
		*spc = '\0';
		*prefix = msg + 1;
		msg = spc + 1;
	}
	else {
		prefix = NULL;
	}

	/* command */
	spc = strchr(msg, ' ');
	*spc = '\0';
	*cmd = msg;
	*ncmd = atoi(msg);
	msg = spc + 1;


	/* params */
	*params = msg;
}

void ircproto_read_message(char **prefix, char **cmd, int *ncmd, char **params)
{
	ircproto_parse_message(sock_readline(), prefix, cmd, ncmd, params);
}

void ircproto_parse_prefix(char *prefix,
                  char *nick, int nicksz,
                  char *user, int usersz,
                  char *host, int hostsz)
{
	char *bang, *at, *end, *uidx, *hidx;
	int nlen, ulen, hlen; /* len of values held in prefix */
	int ncpy, ucpy, hcpy; /* maximum able to copy, ideally == len */

	bang = strchr(prefix, '!');
	at = strchr(prefix, '@');
	end = prefix + strlen(prefix);

	if (nick) {
		nlen = bang ? bang - prefix : end - prefix;
		ncpy = MIN(nlen, nicksz - 1);
		strncpy(nick, prefix, ncpy);
		nick[ncpy] = '\0';
		if (ncpy != nlen)
			info("parse_prefix: %s: nick overflow", nick);
	}

	if (user) {
		if (bang) {
			uidx = bang + 1;
			ulen = at - uidx;
			ucpy = MIN(ulen, usersz - 1);
			strncpy(user, uidx, ucpy);
			user[ucpy] = '\0';
			if (ucpy != ulen)
				info("parse_prefix: %s: uname overflow", user);
		}
		else
			user[0] = '\0';
	}

	if (host) {
		if (at) {
			hidx = at + 1;
			hlen = end - hidx;
			hcpy = MIN(hlen, hostsz - 1);
			strncpy(host, hidx, hcpy);
			host[hcpy] = '\0';
			if (hcpy != hlen)
				info("parse_prefix: %s: host overflow", host);
		}
		else
			host[0] = '\0';
	}
}


/*
 * IRC commands
 */
void ircproto_pong(char *id)
{
	sock_sendline("PONG %s", id);
}

void ircproto_nick(char *nick)
{
	sock_sendline("NICK %s", nick);
}

void ircproto_user(char *username, char *real_name)
{
	sock_sendline("USER %s 0 * :%s", username, real_name);
}

void ircproto_identify(char *nickserv_nick, char *password)
{
	sock_sendline("PRIVMSG %s IDENTIFY %s", nickserv_nick, password);
}

void ircproto_oper(char *oper_username, char *password)
{
	sock_sendline("OPER %s %s", oper_username, password);
}

void ircproto_join(char *chan_name)
{
	sock_sendline("JOIN %s", chan_name);
}

void ircproto_part(char *chan_name)
{
	sock_sendline("PART %s", chan_name);
}

void ircproto_privmsg(char *recipient, char *format, ...)
{
	va_list args;
	char message[512];

	va_start(args, format);
	vsnprintf(message, sizeof(message), format, args);
	va_end(args);

	sock_sendline("PRIVMSG %s :%s", recipient, message);
}

void ircproto_topic(char *chan_name, char *format, ...)
{
	va_list args;
	char topic[512];

	va_start(args, format);
	vsnprintf(topic, sizeof(topic), format, args);
	va_end(args);

	sock_sendline("TOPIC %s :%s\n", chan_name, topic);
}

void ircproto_kick(char *chan_name, char *nick, char *format, ...)
{
	va_list args;
	char message[512];

	va_start(args, format);
	vsnprintf(message, sizeof(message), format, args);
	va_end(args);

	sock_sendline("KICK %s %s :%s\n", chan_name, nick, message);
}

void ircproto_mode(char *chan_name, char *modes)
{
	sock_sendline("MODE %s %s\n", chan_name, modes);
}
