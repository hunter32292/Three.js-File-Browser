/* socket.c: Simple Socket Functions */

#include "chippewa.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

/**
 * Allocate socket, bind it, and listen to specified port.
 **/
int
socket_listen(const char *port)
{
    struct addrinfo  hints;
    struct addrinfo *results;
    int    socket_fd = -1;

    /* Lookup server address information */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE;

    if (getaddrinfo(NULL, port, &hints, &results) < 0) {
        return (-1);
    }

    /* For each server entry, allocate socket and try to connect */
    for (struct addrinfo *p = results; p != NULL; p = p->ai_next) {
	/* Allocate socket */
        if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
            debug("Unable to make socket: %s", strerror(errno));
            continue;
        }

	/* Bind socket */
        if (bind(socket_fd, p->ai_addr, p->ai_addrlen) < 0) {
            debug("Unable to bind: %s", strerror(errno));
            close(socket_fd);
            continue;
       }

       /* Listen to socket */
        if (listen(socket_fd, SOMAXCONN) < 0) {
            debug("Unable to listen: %s", strerror(errno));
            close(socket_fd);
            continue;
        }
        goto success;
    }

    socket_fd = -1;

success:
    freeaddrinfo(results);
    return (socket_fd);
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
