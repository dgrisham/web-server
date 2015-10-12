#include "includes.h"

#define MAXLINE 1024
#define PORT 9873
#define DEBUG 1

void send_to_host(int sockfd) {
    int bufsize = 1024;
    char msg[bufsize];

    strcpy(msg, "Wake up, Neo.\nThe Matrix has you.");
    int msglen = strlen(msg);

    send(sockfd, msg, msglen, 0);
}

int main() {

    /****************************************************************************
     * we need to get the address info for get the info we need for bind/etc.
     * we'll do that with getaddrinfo()
     *****************************************************************************/

    int gai_status;            // to hold the return value of getaddrinfo()
    struct addrinfo hints;     // to hold inputs to getaddrinfo()
    struct addrinfo *servinfo; // to hold outputs from getaddrinfo()

    // zero out our input struct...
    bzero(&hints, sizeof hints);
    /* set the inputs...*/
    // set address family to AF_INET, for ipv4
    hints.ai_family = AF_INET;
    // socket type as SOCK_STREAM, for a sequential connection-based stream
    hints.ai_socktype = SOCK_STREAM;
    // set the AI_PASSIVE flag, since we want the server socket ip to be that of
    // the localhost of thie machine
    hints.ai_flags = AI_PASSIVE;

    // (one of the inputs to getaddrinfo() is the PORT number, but as a string,
    // so let's get that
    char port_str[5];
    sprintf(port_str, "%d", PORT);
    // and finally, call getaddrinfo()
    if ((gai_status = getaddrinfo(NULL, (const char *)port_str, &hints, &servinfo)) != 0) {
        fprintf(stderr, "error getting address info: %s\n", gai_strerror(gai_status));
        exit(1);
    }


    /****************************************************************************
     * create the socket and save the file descriptor that's returned
     ****************************************************************************/

    if (DEBUG)
        printf("creating listening socket...\n");

    // create a file descriptor to hold the fd for the socket (once created)
    int sockfd = -1;
    // and now we call socket(), using the result from out getaddrinfo() call
    if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype,
                           servinfo->ai_protocol)) < 0)
    {
        fprintf(stderr, "failed to create listening socket: %s\n", strerror(errno));
        exit(-1);
    }

    /* 
     * we won't explicitly bind the socket here and just let connect() pick a
     * port to listen on 
     */


    /****************************************************************************
     * attempt to connect to the host
     ****************************************************************************/

    int connect_status;
    if ((connect_status = connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen)) == -1) {
        fprintf(stderr, "error connecting to host: %s\n", strerror(errno));
        exit(-1);
    }

    send_to_host(sockfd);

    freeaddrinfo(servinfo);

    return 0;
}
