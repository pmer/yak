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
#include "../loggedin/loggedin.h"

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

	while (line = readline("> ")) {
		mutex_on();
		eval_emit(line);
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
	eval_register(say, ".");

	onlogin(create_uithread);
	return 0;
}

void finish()
{
}
