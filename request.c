/* request.c: HTTP Request Functions */

#include "chippewa.h"

#include <errno.h>
#include <string.h>

#include <unistd.h>

int parse_request_method(struct request_t *r);
int parse_request_headers(struct request_t *r);

/**
 * Accept request from server socket.
 *
 * This function does the following:
 *
 *  1. Allocates a request struct initialized to 0.
 *  2. Initializes the headers list in the request struct.
 *  3. Accepts a client connection from the server socket.
 *  4. Looks up the client information and stores it in the request struct.
 *  5. Opens the client socket stream for the request struct.
 *  6. Returns the request struct.
 *
 * The returned request struct must be deallocated using free_request.
 **/
struct request_t *
accept_request(int sfd)
{
    struct request_t *r = malloc(sizeof(struct request_t));
    struct sockaddr raddr;
    socklen_t rlen;

    /* Allocate request struct (zeroed) */
    memset(r, 0, sizeof(struct request_t));
    /* Initialize headers */
    TAILQ_INIT(&(r->headers));
    
    /* Accept a client */
    rlen  = sizeof(raddr);
    int asd = accept(sfd, &raddr, &rlen);
    r->fd = asd;
    if (r->fd < 0) {
        fprintf(stderr, "Unable to accept client: %s\n", strerror(errno));
        goto fail;
    }
    /* Lookup client information */
    if (getnameinfo(&raddr, rlen, r->host, sizeof(r->host), r->port, sizeof(r->port), NI_NUMERICHOST | NI_NUMERICSERV) != 0) {
        fprintf(stderr, "Unable to lookup client: %s\n", strerror(errno));
        close(r->fd);
        goto fail;
    }
    /* Open socket stream */
    r->socket = fdopen(r->fd, "r+");
    if (r->socket < 0) {
        fprintf(stderr, "Unable to open file descriptor: %s\n", strerror(errno));
        _exit(EXIT_FAILURE);
    }
    log("Accepted request from %s:%s", r->host, r->port);
    return (r);

fail:
    free_request(r);
    return (NULL);
}

/**
 * Deallocate request struct.
 *
 * This function does the following:
 *
 *  1. Closes the request socket stream or file descriptor.
 *  2. Frees all allocated strings in request struct.
 *  3. Frees all of the headers (including any allocated fields).
 *  4. Frees request struct.
 **/
void
free_request(struct request_t *r)
{
    struct header_t *header;

    if (r == NULL) {
        return;
    }

    /* Close socket or fd */
    fclose(r->socket);

    /* Free allocated strings */
    free(r->method);
    free(r->uri);
    //free(r->path);
    free(r->query);

    /* Free headers */
    TAILQ_FOREACH(header, &(r->headers), headers){
        free(header->name);
                                free(header->value);
                                free(header);
    }
    /* Free request */
    free(r);
}

/**
 * Parse HTTP Request.
 *
 * This function first parses the request method, any query, and then the
 * headers, returning 0 on success, and -1 on error.
 **/
int
parse_request(struct request_t *r)
{
    /* Parse HTTP Request Method */
    /* TODO */
    if(parse_request_method(r) < 0){
        printf("Parsing request method failed\n");
        return -1;   
    }

    /* Parse HTTP Requet Headers */
    /* TODO */
    if(parse_request_headers(r) < 0){
        printf("Parsing request headers failed\n");
        return -1;   
    }
    return 0;
}

/**
 * Parse HTTP Request Method and URI
 *
 * HTTP Requests come in the form
 *
 *  <METHOD> <URI>[QUERY] HTTP/<VERSION>
 *
 * Examples:
 *
 *  GET / HTTP/1.1
 *  GET /cgi.script?q=foo HTTP/1.0
 *
 * This function extracts the method, uri, and query (if it exists).
 **/
#define parse_next_token(t, s)          \
    (t)     = skip_nonwhitespace(s);    \
    if (!*(t)) {                        \
        goto invalid;                   \
    }                                   \
    *(t)++  = '\0';                     \
    (t)     = skip_whitespace(t);
int
parse_request_method(struct request_t *r)
{
    char buffer[BUFSIZ];
    char *method;
    char *uri;
    char *query;

    /* Read line from socket */
    if (fgets(buffer, BUFSIZ, r->socket)){
    /* Parse method and uri */
        printf("buffer -> %s\n", buffer);
        method = skip_whitespace(buffer);
        parse_next_token(uri, method);
        printf("method -> %s\n", method);
        //printf("uri -> %s\n", uri);
                                r->method = malloc(sizeof(method));
        r->method = strdup(method);
        //char *token = strtok(NULL, method);
        //printf("method -> %s\n", token);
        //parse_next_token(uri, token);
        //uri = strtok(uri, " ");
        //r->uri = uri;
        //printf("uri -> %s\n", uri);
        //buffer = strtok(NULL, uri);
        if(strchr(uri, '?') != NULL){
            /*printf("uri1 (with query) -> %s\n", uri);
            uri = strtok(uri, "?");
            query = strtok(NULL, uri);
            printf("uri2 (with query) -> %s\n", uri);
            printf("query -> %s\n", query);*/
            query = strstr(uri, "q=");
            query = strtok(query, " ");
            uri = strtok(uri, "?");
            printf("uri (with query) -> %s\n", uri);
            printf("query -> %s\n", query);
            r->uri = malloc(sizeof(uri));
            r->uri = strdup(uri);
            r->query = malloc(sizeof(query));
            r->query = strdup(query);
        }else{
            uri = strtok(uri, " ");
            r->uri = malloc(sizeof(uri));
            r->uri = strdup(uri);
            printf("uri (no query)-> %s\n", uri);
        }
    /* Record method, uri, and query in request struct */
    /* TODO */
    }
    debug("HTTP METHOD: %s", r->method);
    debug("HTTP URI:    %s", r->uri);
    debug("HTTP QUERY:  %s", r->query);

    return (0);
invalid:
    fprintf(stderr, "Invalid request: %s\n", buffer);

fail:
    return (-1);
}

/**
 * Parse HTTP Request Headers
 *
 * HTTP Headers come in the form:
 *
 *  <NAME>: <VALUE>
 *
 * Example:
 *
 *  Host: localhost:8888
 *  User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:29.0) Gecko/20100101 Firefox/29.0
 *  Accept: text/html,application/xhtml+xml
 *  Accept-Language: en-US,en;q=0.5
 *  Accept-Encoding: gzip, deflate
 *  Connection: keep-alive
 *
 * This function parses the stream from the request socket using the following
 * pseudo-code:
 *
 *  while (buffer = read_from_socket() and buffer is not empty):
 *      name, value = buffer.split(':')
 *      header      = new Header(name, value)
 *      headers.append(header)
 **/
int
parse_request_headers(struct request_t *r)
{
    struct header_t *header;
    char buffer[BUFSIZ];
    char *name;
    char *value;
    /* Parse headers from socket */
    printf("parsing headers\n");
    while((fgets(buffer, BUFSIZ, r->socket)) && buffer != NULL){
                                printf("*********************************%s\n",buffer);
        name = skip_whitespace(buffer);
        if( name[0] == '\0'){
                                        return 0;
                                }       
                                parse_next_token(value, name);
        name = strtok(name, ":");
                header = malloc(sizeof(struct header_t));
                                header->name = (char *) malloc(sizeof(name));
                                header->value = (char *) malloc(sizeof(value));
        header->name = strdup(name);
        header->value = strdup(value);
        printf("name %s | value %s\n", header->name, header->value);
        printf("After parsing headers\n");
        TAILQ_INSERT_TAIL(&(r->headers), header, headers);
        printf("After parsing headers2\n");
    }

#ifndef NDEBUG
    TAILQ_FOREACH(header, &r->headers, headers) {
        debug("HTTP HEADER %s = %s", header->name, header->value);
    }
#endif
    return (0);
invalid:
    fprintf(stderr, "Invalid request: %s\n", buffer);

fail:
    return (-1);
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
