/* forking.c: Forking HTTP Server */

#include "chippewa.h"

#include <errno.h>
#include <signal.h>
#include <string.h>

#include <unistd.h>

/**
 * Fork incoming HTTP requests to handle the concurrently.
 *
 * The parent should accept a request and then fork off and let the child
 * handle the request.
 **/
void
forking_server(int sfd)
{
    struct request_t *request;
    pid_t pid;

    /* Accept and handle HTTP request */
    while (1) {
    	/* Accept request */
        if((request = accept_request(sfd)) == NULL){
            printf("Unable to accept request\n");
            continue;
        }
	/* Ignore children */
        signal(SIGCHLD, SIG_IGN);

	/* Fork off child process to handle request */
        pid = fork();
        switch(pid){
        case 0:
            handle_request(request);
            break;
        case -1:
            printf("Fork failed\n");
            break;
        }
    }

    /* Close server socket */
    fclose(request->socket);
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
