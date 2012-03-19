#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "bool.h"
#include "diagnostic.h"
#include "ircproto.h"
#include "socket.h"
#include "thread.h"
#include "../eval/eval.h"

/* fails if before stdio.h ?? */
#include <readline/readline.h>

static char chan[512] = "";

static void cd(char *line, char **caps, int ncaps)
{
	if (ncaps > 1 && caps[1][0])
		info("cd: must have one argument");
	strcpy(chan, caps[0]);
}

static void say(char *line, char **caps, int ncaps)
{
	int len = strlen(line);

	if (len > 1 && line[0] == 'c' && line[1] == 'd')
		return;
	if (*chan)
		ircproto_privmsg(chan, "%s", line);
	else
		info("say: must 'cd' into a channel first");
}

static void *uimain(void *unused)
{
	char *line;

	/* wait for startup to finish */
	mutex_on();
	mutex_off();

	sleep(2);

	while (line = readline("> ")) {
		eval_emit(line);
		free(line);
	}
	want_quit = true;
}

int init()
{
	pthread_t thread;
	FILE *newsocklog;

	/* redirect socklog so the terminal remains relatively uncluttered */
	newsocklog = fopen("socket.log", "w");
	if (!newsocklog) {
		err("ui: couldn't open socket.log for write");
		return 1;
	}
	socklog = newsocklog;

	eval_register(cd, "^cd (\\S+)(.*)?");
	eval_register(say, ".");

	nthreads++;
	pthread_create(&thread, NULL, uimain, NULL);
	return 0;
}

void finish()
{
}
