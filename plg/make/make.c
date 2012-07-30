#include <stdlib.h>
#include <unistd.h>

#include "auth.h"
#include "callback.h"
#include "diagnostic.h"
#include "ircproto.h"
#include "socket.h"
#include "yak.h"

/*
 * In a nutshell, runs "make && ./yak".
 */
static void make(char *usr, char *src, char *msg)
{
	pid_t pid;
	int status;

	if (!auth_isowner(usr)) {
		ircproto_privmsg(src, "err: not owner");
		return;
	}

	/*
	 * first, fork and exec 'make'
	 */
	pid = fork();
	if (pid == -1) {
		ircproto_privmsg_errno(src, "fork");
	}
	else if (pid == 0) {
		execlp("make", "make", (char *)NULL);
		/* should not execute past this */
		ircproto_privmsg_errno(src, "exec(make)");
		exit(1);
	}
	else {
		waitpid(pid, &status, 0);
		/*
		 * if that works, then exec ourself with our original argv (and
		 * hope we haven't changed directories since startup)
		 */
		if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
			/* cannot call safe_shutdown() because this itself is a
			 * plugin and would get unloaded */
			close_connection();
			execvp(argv[0], argv);
			/* should not execute past this */
			/* would like to ircproto_privmsg_errno, but we just closed connection :( */
			err_errno("exec(myself)");
			exit(1);
		}
		else {
			ircproto_privmsg(src, "err: build failed");
		}
	}
}

int init()
{
	callback_register_privmsg_str(make, "make");
	return 0;
}

void finish()
{
}
