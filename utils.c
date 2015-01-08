/* utils.c: chippewa utilities */

#include "chippewa.h"

#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <libgen.h>

#include <sys/stat.h>
#include <unistd.h>

/**
 * Determine mime-type from file extension
 *
 * This function first finds the file's extension and then scans the contents
 * of the MimeTypesPath file to determine which mimetype the file has.
 *
 * The MimeTypesPath file (typically /etc/mime.types) consists of rules in the
 * following format:
 *
 *  <MIMETYPE>      <EXT1> <EXT2> ...
 *
 * This function simply checks the file extension version each extension for
 * each mimetype and returns the mimetype on the first match.
 *
 * If no extension exists or no matching mimetype is found, then return 
 * DefaultMimeType.
 *
 * This function returns an allocated string that must be free'd.
 **/
char *
determine_mimetype(const char *path)
{
                
    //char *ext;
    char *mimetype;
    char *token;
    char buffer[BUFSIZ];
    FILE *fs = NULL;
    printf("path -> %s\n", path);
    memcpy(&token, &path, sizeof(path));
    char *name = basename(token);
    printf("name -> %s\n", name);
    if(strstr(name, "cgi") != NULL){
        mimetype = "text/cgi";
        goto done;
    }else{
        printf("Path to open %s\n",path);
        struct stat file_stat;
        if(stat(path, &file_stat) < 0){
            printf("Error stat file %s\n", path);
        }   
        if(S_ISREG(file_stat.st_mode)){
            mimetype = "text/html";
            fs = fopen(path, "r");
            if(fs == NULL){
                printf("Unable to open file -> %s\n", path);
                goto fail;
        //exit(EXIT_FAILURE);
            }else{
            /* Scan file for matching file extensions */
                goto done; 
            }
        }else{
            mimetype = "text/plain";
        }
    }
        /*printf("name -> %s\n", name);
        token = strtok(name, ".");
        mimetype = strtok(NULL, token);
        printf("mimetype -> %s\n", mimetype);
        if(mimetype == NULL){
            goto fail;
        }
    }
    //token = path;
     Find file extension */
    //token = strtok(token, ".");
    //mimetype = strtok(NULL, token); //Should contain file extention (mimetype)
    
    /* Open MimeTypesPath file */
    /* TODO */



     
fail:
    mimetype = DefaultMimeType;

done:
    if (fs) {
        fclose(fs);
    }
    return strdup(mimetype);
}

/**
 * Determine actual filesystem path based on RootPath and URI
 *
 * This function uses realpath(3) to generate the realpath of the
 * file requested in the URI.
 *
 * As a security check, if the real path does not begin with the RootPath, then
 * return NULL.
 *
 * Otherwise, return a newly allocated string containing the real path.  This
 * string must later be free'd.
 **/
char *
determine_request_path(const char *uri)
{
    char path[BUFSIZ];
    //char* temp = ".";
    char real[BUFSIZ];
    printf("URI -> %s\n", uri);
    printf("in determine request path\n");
    if(strcmp(uri,"/")==0){
        printf("/ part\n");
        memcpy(path, ".", strlen(".")+1);
    }else{
        printf("random URI part\n");
        //goto done;
        memcpy(path,uri,strlen(uri)+1);
    }
    char *name = basename(path);
    /*char *token = strtok(path, "/");
    token = strtok(NULL, token);
    memcpy(path, token, strlen(token)+1);*/
    printf("PATH-> %s\n", path);
    printf("NAME-> %s\n", name);
    struct stat file_stat;
    if(stat(name, &file_stat) < 0){
        printf("Error stat file %s\n", name);
    }
    if(S_ISREG(file_stat.st_mode)){
        char *res = realpath(name, real);
        char real2[BUFSIZ];
        char *res2= realpath(RootPath, real2);
    if(res){
        printf("Source is at %s\n", real);
    }else{
        perror("realpath");
        return NULL;
        //exit(EXIT_FAILURE);
    }
    if(strncmp(res, res2, strlen(RootPath)) != 0 ){
        return NULL;
    }
    printf("res %s res2 %s\n", res, res2);

    return (strdup(real));

    }else{
        printf("path -> %s\n", path);
        
        char *res = realpath(name, real);
        printf("NOT A FILE, RES -> %s\n", res);
        return strdup(real);
    }
        
    return NULL;
    }

/**
 * Determine request type from path
 *
 * Based on the file specified by path, determine what type of request
 * this is:
 *
 *  1. REQUEST_BROWSE: Path is a directory.
 *  2. REQUEST_CGI:    Path is an executable file.
 *  3. REQUEST_FILE:   Path is a readable file.
 *  4. REQUEST_ERROR:  Everything else is an error.
 **/
request_type_t
determine_request_type(const char *path)
{
    struct stat s;
    request_type_t type;

    if(stat(path, &s) < 0){
        perror(path);
    }else{
        if(S_ISDIR(s.st_mode)){
            type = REQUEST_BROWSE;
        }else if((S_IXUSR & s.st_mode)){
            printf("Is executable\n");
            type = REQUEST_CGI;
        }else if(S_ISREG(s.st_mode)){
            type = REQUEST_FILE;
        }else{
            type = REQUEST_ERROR;
        }
    }

    return (type);
}

/**
 * Return static string corresponding to HTTP Status code
 *
 * http://en.wikipedia.org/wiki/List_of_HTTP_status_codes
 **/
const char *
http_status_string(http_status_t status)
{
    const char *status_string;

    /* TODO */
    if(status == HTTP_STATUS_OK){
        status_string = "200 OK\r\n";
    }else if(status == HTTP_STATUS_BAD_REQUEST){
        status_string = "400 Bad Request\r\n";
    }else if(status == HTTP_STATUS_NOT_FOUND){
        status_string = "404 Not Found :(\r\n";
    }else if(status == HTTP_STATUS_INTERNAL_SERVER_ERROR){
        status_string = "500 Internal Server Error\r\n";
    }else{
        status_string = "Unknown HTTP Status\r\n";
    }
    return (status_string);
}

/**
 * Advance string pointer pass all nonwhitespace characters
 **/
char *
skip_nonwhitespace(char *s)
{
    while (*s && !isspace(*s)) {
        s++;
    }

    return (s);
}

/**
 * Advance string pointer pass all whitespace characters
 **/
char *
skip_whitespace(char *s)
{
    while (*s && isspace(*s)) {
        s++;
    }

    return (s);
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
