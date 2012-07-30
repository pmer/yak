#ifndef IRCPROTO_H
#define IRCPROTO_H

/* Stateless IRC protocol interface. */

/*
 * Numeric commands.
 * 001-099 are client => server commands
 * 200-399 are server => client responses
 * 400-499 are error messages
 */
#define RPL_WELCOME          1 /* our name */
#define RPL_YOURHOST         2 /* server name, server version */
#define RPL_CREATED          3 /* server start date/time */
#define RPL_MYINFO           4 /* info about the server */
#define RPL_ISUPPORT         5 /* info about server technicalities */
#define RPL_LUSERCLIENT    251 /* numbers of users on IRC network */
#define RPL_NAMREPLY       353 /* partial list of nicknames in a channel */
#define RPL_ENDOFNAMES     366 /* end of NAMES reply */
#define ERR_UNKNOWNCOMMAND 421 /* server does not know a command we sent */
#define ERR_NICKNAMEINUSE  433 /* our requested nick is already taken */
#define ERR_YOUREBANNED    465 /* we are banned from the server */

#define RPL_LOGGEDIN RPL_LUSERCLIENT /* we've finished logging in */

void ircproto_parse_message(char *msg, char **prefix, char **cmd, int *ncmd,
	char **params);
void ircproto_read_message(char **prefix, char **cmd, int *ncmd,
	char **params);

void ircproto_parse_prefix(char *prefix,
	char *nick, int nicksz,
	char *user, int usersz,
	char *host, int hostsz);

void ircproto_pong(char *id);
void ircproto_nick(char *nick);
void ircproto_user(char *username, char *real_name);
void ircproto_identify(char *nickserv_nick, char *password);
void ircproto_oper(char *oper_username, char *password);
void ircproto_join(char *chan_name);
void ircproto_part(char *chan_name);
void ircproto_privmsg(char *recipient, char *format, ...);
void ircproto_privmsg_errno(char *recipient, char *format, ...);
void ircproto_notice(char *recipient, char *format, ...);
void ircproto_topic(char *chan_name, char *format, ...);
void ircproto_kick(char *chan_name, char *nick, char *format, ...);
void ircproto_mode(char *chan_name, char *modes);

#endif
