/* single.c: Single User HTTP Server */

#include "chippewa.h"

#include <errno.h>
#include <string.h>

#include <unistd.h>

/**
 * Handle one HTTP request at a time
 **/
void
single_server(int sfd)
{
    struct request_t *request;

    /* Accept and handle HTTP request */

    while (1) {
    	/* Accept request */
        if((request = accept_request(sfd)) == NULL){
            printf("Unable to accept request\n");
            continue;
        }
	/* Handle request */
        handle_request(request);
        
	/* Free request */
        free_request(request);
    }

    /* Close server socket */
    fclose(request->socket);
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
