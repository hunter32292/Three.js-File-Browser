/* handler.c: HTTP Request Handlers */

#include "chippewa.h"

#include <errno.h>
#include <limits.h>
#include <string.h>

#include <dirent.h>
#include <unistd.h>

/* Internal Declarations */
http_status_t handle_browse_request(struct request_t *request);
http_status_t handle_file_request(struct request_t *request);
http_status_t handle_cgi_request(struct request_t *request);
http_status_t handle_error(struct request_t *request, http_status_t status);

/** 
 * Handle HTTP Request
 *
 * This parses a request, determines the request path, determines the request
 * type, and then dispatches to the appropriate handler type.
 *
 * On error, handle_error should be used with an appropriate HTTP status code.
 **/
http_status_t
handle_request(struct request_t *r)
{
    http_status_t result;
    result = HTTP_STATUS_OK;
    /* Parse request */
    /* TODO */

    if(parse_request(r) < 0){
        printf("Unable to parse request\n");
        result = HTTP_STATUS_BAD_REQUEST;
        handle_error(r, result);
    }

    /* Determine request path */
    /* TODO */
        r->path = malloc(sizeof(char*) * 10);
        printf("r->uri -> %s\n", r->uri);
        r->path = determine_request_path(r->uri);
        printf("***********************************r->path = %s\n", r->path);
    if( r->path == NULL){
        printf("handling error\n");
        result = HTTP_STATUS_NOT_FOUND;
        handle_error(r, result);
    }else{
        printf("r->path -> %s\n", r->path);

    /* Dispatch to appropriate request handler type */
    /* TODO */
        request_type_t type = determine_request_type(r->path);
        if(type == REQUEST_FILE){
            result = handle_file_request(r);
        }else if(type == REQUEST_BROWSE){
            result = handle_browse_request(r);
        }else if(type == REQUEST_CGI){
            result = handle_cgi_request(r);
        }else if(type == REQUEST_ERROR){
            result = handle_error(r, result);
        }
    }
    log("HTTP REQUEST STATUS: %s", http_status_string(result));
    return (result);
}

/**
 * Handle browse request
 *
 * This lists the contents of a directory in HTML.
 *
 * If the path cannot be opened or scanned as a directory, then handle error 
 * with HTTP_STATUS_NOT_FOUND.
 **/
http_status_t
handle_browse_request(struct request_t *r)
{
    printf("Browse handler\n");
    struct dirent **entries;
    int n;
    /* Open a directory for reading or scanning */
    /* TODO */
    printf("Path -> %s\n", r->path);
    n = scandir(r->path, &entries, 0, alphasort);
    if(n < 0){
                                printf("In handle error when n < 0\n");
        handle_error(r, HTTP_STATUS_NOT_FOUND);
    }else{
        /* Write HTTP Header with OK Status and text/html Content-Type */
        /* TODO */
        printf("Printing handler\n");
        //r->socket = popen("scripts/cgi.html.sh","r");
                
       // if (write(r->fd, header, sizeof(header)) != sizeof(header)){
       //     printf("error in writing on stream socket\n");
       // }

        fputs("HTTP/1.0 200 OK\r\n", r->socket);
        fputs("Content-Type: text/html\r\n", r->socket);        
        fputs("\r\n", r->socket);

        // TODO: I'm going to add the function that will write the html and store the script for the three js code


        fputs("<html>\r\n", r->socket);
        for(int i = 0; i < n; i++){
            char tmp[BUFSIZ];
            snprintf(tmp, BUFSIZ, "<a href=\"/%s\">%s</a>", entries[i]->d_name, entries[i]->d_name);
            fputs(tmp, r->socket);
            //fputs(entries[i]->d_name, r->socket);
            fputs("<br>", r->socket);
        }
        fputs("</html>\r\n", r->socket);
    }
    /* Flush socket, return OK */
    /* TODO */
    fflush(r->socket);
    return (HTTP_STATUS_OK);
}

/**
 * Handle file request
 *
 * This opens and streams the contents of the specified file to the socket.
 *
 * If the path cannot be opened for reading, then handle error with
 * HTTP_STATUS_NOT_FOUND.
 **/
http_status_t
handle_file_request(struct request_t *r)
{
                printf("File handler\n");
    FILE *fs;
    char buffer[BUFSIZ];
    char *mimetype = NULL;
    //size_t nread;
                
    /* Open file for reading */
    /* TODO */
    fs = fopen(r->path, "r");
    if(fs == NULL){
        printf("Unable to open file for reading");
        goto fail;
    }
    fputs("HTTP/1.0 200 OK\r\n", r->socket);
    fputs("Content-Type: text/html\r\n", r->socket);
    fputs("\r\n", r->socket);
    fputs("<html>\r\n", r->socket);
    while(fgets(buffer, BUFSIZ, fs)){
        fputs(buffer, r->socket);
        fputs("<br/>", r->socket);
    }
    fputs("</html>\r\n", r->socket);

     /*   
    char content_type[100];
    mimetype = determine_mimetype(r->path);
    snprintf(content_type, 100, "Content-Type: %s", mimetype);
    if (write(r->fd, content_type, sizeof(content_type)) != sizeof(content_type)){
        printf("error in writing on stream socket\n");
    }
    char* header = "<html>";
    if (write(r->fd, header, sizeof(header)) != sizeof(header)){
        printf("error in writing on stream socket\n");
    }
   // request_type_t request = determine_request_type(r->path); *******
   // if (write(r->fd, request, sizeof(request)) != sizeof(request)){
   //     printf("error in writing request on socket\n");
   // }
                
        const char* status = http_status_string(handle_request(r));
    if (write(r->fd, status, sizeof(status)) != sizeof(status)){
        printf("error in writing on stream socket\n");
    }
        
        
    char* footer = "</html>";
    if (write(r->fd, footer, sizeof(footer)) < 0){
        printf("error in writing on stream socket\n");
    }*/
    fclose(fs);
    fflush(r->socket);
    free(mimetype);
        
    return (HTTP_STATUS_OK);

fail:
    /* Close file, free mimetype, return INTERNAL_SERVER_ERROR */
    fclose(fs);
    free(mimetype);
    return (HTTP_STATUS_INTERNAL_SERVER_ERROR);
}

/**
 * Handle file request
 *
 * This popens and streams the results of the specified executables to the
 * socket.
 *
 * If the path cannot be popened, then handle error with
 * HTTP_STATUS_INTERNAL_SERVER_ERROR.
 **/
http_status_t
handle_cgi_request(struct request_t *r)
{

                printf("CGI handler\n");
    FILE *pfs;
    char buffer[BUFSIZ];
    struct header_t *header;
    fputs("HTTP/1.0 200 OK\r\n", r->socket);

    /* Export CGI environment variables from request:
     * http://en.wikipedia.org/wiki/Common_Gateway_Interface */
    /* TODO */
     if(setenv("REQUEST_URI", r->uri, 1) <0){
        fprintf(stderr, "setenv failed on BASEPATH");
    }
    if(setenv("REQUEST_METHOD", r->method, 1) < 0){
         fprintf(stderr, "setenv failed on FULLPATH");
    }
    if(setenv("QUERY_STRING", r->query, 1) < 0){
         fprintf(stderr, "setenv failed on EVENT");
    }
    /* Export CGI environment variables from request headers */
    /* TODO */
    TAILQ_FOREACH(header, &(r->headers), headers){
        if(strcmp(header->name, "Host") == 0){
            if(setenv("HTTP_HOST", header->name, 1) < 0){
                fprintf(stderr, "setenv failed on TIMESTAMP");        
            }
        }else if(strcmp(header->name, "User-Agent") == 0){
            if(setenv("HTTP_USER_AGENT", header->name, 1) < 0){
                fprintf(stderr, "setenv failed on TIMESTAMP");        
            }

        }else if(strcmp(header->name, "Accept") == 0){
            if(setenv("HTTP_ACCEPT", header->name, 1) < 0){
                fprintf(stderr, "setenv failed on TIMESTAMP");        
            }

        }else if(strcmp(header->name, "Accept-Language") == 0){
            if(setenv("HTTP_ACCEPT_LANGUAGE", header->name, 1) < 0){
                fprintf(stderr, "setenv failed on TIMESTAMP");        
            }

        }else if(strcmp(header->name, "Accept-Encoding") == 0){
            if(setenv("HTTP_ACCEPT_ENCODING", header->name, 1) < 0){
                fprintf(stderr, "setenv failed on TIMESTAMP");        
            }

        }else if(strcmp(header->name, "Connection") == 0){
            if(setenv("HTTP_CONNECTION", header->name, 1) < 0){
                fprintf(stderr, "setenv failed on TIMESTAMP");        
            }
        }else if(strcmp(header->name, "Accept-Charset") == 0){
            if(setenv("HTTP_ACCEPT_CHARSET", header->name, 1) < 0){
                fprintf(stderr, "setenv failed on TIMESTAMP");        
            }
        }
    }
    /* POpen CGI Script */
    /* TODO */
    pfs = popen(r->path, "r");
    if(pfs == NULL){
        printf("Unable to open script\n");
    }else{   
        /* Copy data from popen to socket */
        while(fgets(buffer, BUFSIZ, pfs)){
            fputs(buffer, r->socket);
        }
    }
    /* Close popen, flush socket, return OK */
    fclose(pfs);
    fflush(r->socket);
    return (HTTP_STATUS_OK);
}

/**
 * Handle displaying error page
 *
 * This writes an HTTP status error code and then generates an HTML message to
 * notify the user of the error.
 **/
http_status_t
handle_error(struct request_t *r, http_status_t status)
{
    const char *status_string = http_status_string(status);

    /* Write HTTP Header */
    char tmp[BUFSIZ];
    snprintf(tmp, BUFSIZ, "HTTP/1.0 %s \r\n", status_string); 
    fputs(tmp , r->socket);
    fputs("Content-Type: text/html\r\n", r->socket);        
    fputs("\r\n", r->socket);
    fputs("<html>\r\n", r->socket);
    /* Write HTML Description of Error*/
    fputs(status_string, r->socket);
    fputs("</html>\r\n", r->socket);
    /* Return specified status */
    return (status);
}


void write_file_to_browser(struct request_t *r){
 
    char * line = NULL;
    size_t length = 0;
    ssize_t read;
    File fp* = fopen("three/periodictable.html","r");

    if (ifp == NULL) {
        fprintf(stderr, "Could not open file\n");
        exit(1);
    }   

    /*      HTML-HEADER-TAG        */
    fputs("HTTP/1.0 200 OK\r\n", r->socket);
    fputs("Content-Type: text/html\r\n", r->socket);        
    fputs("\r\n", r->socket);


    while ((read = getline(&line, &len, fp)) != -1) {    
        printf("Retrieved line of length %zu :\n", read);
        printf("%s", line);
        fputs(line,r->socket);
        if(strstr(line,"var table = [") != NULL){
            write_individuel_files(r);
        }

    }


}

void write_individuel_files(struct request_t *r){

        /*       WRITE FILES TO PAGE        */     
        // Example file: JPG, FILENAME, FileSize, Modified, Created
        for(int i = 0; i < n; i++){
                char tmp[BUFSIZ];
                snprintf(tmp, BUFSIZ, "<a href=\"/%s\">%s</a>", entries[i]->d_name, entries[i]->d_name);
                fputs(tmp, r->socket);
                //fputs(entries[i]->d_name, r->socket);
                fputs("<br>", r->socket);
        }

}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
