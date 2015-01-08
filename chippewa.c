/* chippewa: Simple HTTP Server */

#include "chippewa.h"

#include <errno.h>
#include <stdbool.h>
#include <string.h>

#include <unistd.h>

/* Global Variables */
char *Port	      = "9321";
char *MimeTypesPath   = "/etc/mime.types";
char *DefaultMimeType = "text/plain";
char *RootPath	      = ".";
bool  Forking	      = false;

/**
 * Display usage message.
 */
void
usage(const char *progname)
{
    fprintf(stderr, "usage: %s [fhmMpr]\n", progname);
    fprintf(stderr, "options:\n");
    fprintf(stderr, "    -f            Enable forking mode\n");
    fprintf(stderr, "    -h            Display help message\n");
    fprintf(stderr, "    -m path       Path to mimetypes file\n");
    fprintf(stderr, "    -M mimetype   Default mimetype\n");
    fprintf(stderr, "    -p port       Port to listen on\n");
    fprintf(stderr, "    -r path       Root directory\n");
}

/**
 * Parses command line options and starts appropriate server
 **/
int
main(int argc, char *argv[])
{
    int c;
    int sfd;
    /* Parse command line options */
    while((c = getopt(argc, argv, "h::f::m:M:p:r:")) != -1){
        switch(c){
            case 'h':
                usage("chippewa");
                break;
            case 'f':
                Forking = true;
                break;
            case 'm':
                MimeTypesPath = optarg;
                break;
            case 'M':
                DefaultMimeType = optarg;
                break;
            case 'p':
                Port = optarg;
                break;
            case 'r':
                RootPath = optarg;
                break;
           default:
                fprintf(stderr, "unknown option: %c\n", c);
                usage("chippewa");
                break;
        }
    }
    /* Listen to server socket */
    while(true){
        sfd = socket_listen(Port);
        if(sfd < 0){
            debug("Unable to listen on %s\n", Port);
            exit(EXIT_FAILURE);
        }

        /* Determine real RootPath */
        log("Listening on port %s", Port);
        debug("RootPath        = %s", RootPath);
        debug("MimeTypesPath   = %s", MimeTypesPath);
        debug("DefaultMimeType = %s", DefaultMimeType);
        debug("Forking         = %s", Forking ? "yes" : "no");

        /* Start either forking or single HTTP server */
        if(Forking){
            forking_server(sfd);
        }else{
            single_server(sfd);
        }
    }
    return (EXIT_SUCCESS);
}
