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

static struct evalctx *ectx;
static char chan[512] = "";

static void cd(char *line, char **caps, int ncaps)
{
	if (ncaps > 1 && *caps[1])
		info("cd: must have one argument");

	info("now talking on %s", caps[0]);

	strcpy(chan, caps[0]);
}

static void nick(char *line, char **caps, int ncaps)
{
	if (ncaps > 1 && *caps[1])
		info("nick: must have one argument");

	strcpy(bot_nick, caps[0]);
	ircproto_nick(caps[0]);
	/* TODO: nick reply should set bot_nick */
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
		if (line[0] == '/') {
			if (!eval_emit(ectx, line + 1)) {
				info("invalid command: %s", line);
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

	ectx = eval_create();
	eval_register(ectx, cd, "^cd (\\S+)(.*)?");
	eval_register(ectx, nick, "^nick (\\S+)(.*)?");
	eval_register(ectx, msg, "^msg (\\S+) (.*)");
	eval_register(ectx, quit, "^quit$");

	onlogin(create_uithread);

	return 0;
}

void finish()
{
	eval_destroy(ectx);
}
