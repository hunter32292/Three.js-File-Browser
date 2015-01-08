#ifndef CHIPPEWA_H
#define CHIPPEWA_H

#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <sys/queue.h>
#include <unistd.h>

/* Constants */

#define WHITESPACE	" \t\n"

/* Global Variables */

extern char *Port;
extern char *MimeTypesPath;
extern char *DefaultMimeType;
extern char *RootPath;

/* Logging Macros */

#ifdef NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...)   fprintf(stderr, "[%5d] DEBUG %10s:%-4d " M "\n", getpid(), __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#define fatal(M, ...)   fprintf(stderr, "[%5d] FATAL %10s:%-4d " M "\n", getpid(), __FILE__, __LINE__, ##__VA_ARGS__); exit(EXIT_FAILURE)
#define log(M, ...)     fprintf(stderr, "[%5d] LOG   %10s:%-4d " M "\n", getpid(), __FILE__, __LINE__, ##__VA_ARGS__)

/* HTTP Request */

struct header_t {
    char *name;
    char *value;

    TAILQ_ENTRY(header_t) headers;
};

TAILQ_HEAD(headers_t, header_t);

struct request_t {
    int   fd;
    FILE *socket;
    char *method;
    char *uri;
    char *path;
    char *query;

    char host[NI_MAXHOST];
    char port[NI_MAXHOST];

    struct headers_t headers;
};

struct request_t *  accept_request(int sfd);
void		    free_request(struct request_t *request);
int		    parse_request(struct request_t *request);

/* HTTP Request Handlers */

typedef enum {
    REQUEST_BROWSE,
    REQUEST_FILE,
    REQUEST_CGI,
    REQUEST_ERROR,
} request_type_t;

typedef enum {
    HTTP_STATUS_OK,			/* 200 OK */
    HTTP_STATUS_BAD_REQUEST,		/* 400 Bad Request */
    HTTP_STATUS_NOT_FOUND,		/* 404 Not Found */
    HTTP_STATUS_INTERNAL_SERVER_ERROR,	/* 500 Internal Server Error */
} http_status_t;

http_status_t	    handle_request(struct request_t *request);

/* HTTP Server */

void		    single_server(int sfd);
void		    forking_server(int sfd);

/* Socket */

int		    socket_listen(const char *port);

/* Utilities */

#define chomp(s)    (s)[strlen(s) - 1] = '\0'
#define streq(a, b) (strcmp((a), (b)) == 0)

char *		    determine_mimetype(const char *path);
char *		    determine_request_path(const char *uri);
request_type_t	    determine_request_type(const char *path);
const char *        http_status_string(http_status_t status);
char *		    skip_nonwhitespace(char *s);
char *		    skip_whitespace(char *s);

#endif

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
