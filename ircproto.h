#ifndef IRCPROTO_H
#define IRCPROTO_H

/* Stateless IRC wire-protocol handler. */

/*
 * Numeric commands.
 * 001-099 are client-server specific IRC commands
 * 200-399 are server responses to client commands
 */
#define RPL_WELCOME          1 /* our name */
#define RPL_YOURHOST         2 /* server name, server version */
#define RPL_CREATED          3 /* server start date/time */
#define RPL_MYINFO           4 /* info about the server */
#define ERR_UNKNOWNCOMMAND 421 /* server does not know a command we sent */
#define ERR_NICKNAMEINUSE  433 /* our requested nick is already taken */
#define ERR_YOUREBANNED    465 /* we are banned from the server */

void ircproto_parse_message(char *msg, char **prefix, char **cmd, int *ncmd,
	char **params);
void ircproto_read_message(char **prefix, char **cmd, int *ncmd,
	char **params);

void ircproto_pong(char *id);
void ircproto_nick(char *nick);
void ircproto_user(char *username, char *real_name);
void ircproto_identify(char *nickserv_nick, char *password);
void ircproto_oper(char *oper_username, char *password);
void ircproto_join(char *chan_name);
void ircproto_part(char *chan_name);
void ircproto_privmsg(char *recipient, char *format, ...);
void ircproto_topic(char *chan_name, char *format, ...);
void ircproto_kick(char *chan_name, char *nick, char *format, ...);
void ircproto_mode(char *chan_name, char *modes);

#endif

