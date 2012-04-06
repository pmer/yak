#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "bool.h"
#include "diagnostic.h"
#include "ircproto.h"
#include "socket.h"
#include "thread.h"
#include "yak.h"
#include "plg/eval/eval.h"
#include "plg/loggedin/loggedin.h"

/* fails if before stdio.h ?? */
#include <readline/readline.h>

static char chan[512] = "";

static void cd(char *line, char **caps, int ncaps)
{
	if (ncaps > 1 && *caps[1])
		info("cd: must have one argument");
	
	char *infostr = malloc(16 + strlen(caps[0]));
	strcpy(infostr, "now talking on ");
	strcat(infostr, caps[0]);
	info(infostr);
	free(infostr);
	
	strcpy(chan, caps[0]);
}

static void nick(char *line, char **caps, int ncaps)
{
	if (ncaps > 1 && *caps[1])
		info("nick: must have one argument");
	
	ircproto_nick(bot_nick = caps[0]);
}

static void msg(char *line, char **caps, int ncaps)
{
	ircproto_privmsg(caps[0], "%s", caps[1]);
}

static void quit(char *line, char **caps, int ncaps)
{
	safe_shutdown_and_die(0);
}

static void say(char *line)
{
	if (*chan)
		ircproto_privmsg(chan, "%s", line);
	else
		info("please '/cd' into a channel first");
}

static void *uimain(void *unused)
{
	char *line;

	while (line = readline("> ")) {
		mutex_on();
		if(line[0] == '/') {
			if (!eval_emit(line + 1)) {
				char *infostr = malloc(18 + strlen(line));
				strcpy(infostr, "invalid command: ");
				strcat(infostr, line);
				info(infostr);
				free(infostr);
			}
		} else {
			say(line);
		}
		mutex_off();
		free(line);
	}
	printf("\n");
	safe_shutdown_and_die(0);
}

static void create_uithread()
{
	pthread_t thread;

	nthreads++;
	pthread_create(&thread, NULL, uimain, NULL);
}

int init()
{
	FILE *newsocklog;

	/* redirect socklog so the terminal remains relatively uncluttered */
	newsocklog = fopen("socket.log", "w");
	if (!newsocklog) {
		err("ui: couldn't open socket.log for write");
		return 1;
	}
	socklog = newsocklog;

	eval_register(cd, "^cd (\\S+)(.*)?");
	eval_register(nick, "^nick (\\S+)(.*)?");
	eval_register(msg, "^msg (\\S+) (.*)");
	eval_register(quit, "^quit$");

	onlogin(create_uithread);
	
	return 0;
}

void finish()
{

}

