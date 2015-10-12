#include "includes.h"

#define MAXLINE 1024
#define PORT 9873
#define DEBUG 1

// ***************************************************************************
// * readGetRequest()
// *  This function should read each line of the request sent by the client
// *  and check to see if it contains a GET request (which is the part  we are
// *  interested in).  You know you have read all the lines when you read
// *  a blank line.
// ***************************************************************************
char *readGetRequest(int sockfd) {
    return (char *) NULL;
}


// ***************************************************************************
// * parseGET()
// *  Parse the GET request line and find the filename.  Since HTTP requests
// *  should always be relitive to a particular directory (so you don't 
// *  accidently expose the whole filesystem) you should prepend a path
// *  as well. In this case prepend "." to make the request relitive to
// *  the current directory. 
// *
// *  Note that a real webserver will include other protections to keep
// *  requests from traversing up the path, including but not limited to
// *  using chroot.  Since we can't do that you must remember that as long
// *  as your program is running anyone who knows what port you are using
// *  could get any of your files.
// ***************************************************************************
char *parseGET(char *getRequest) {
    return (char *) NULL;
}


// ***************************************************************************
// * fileExists()
// *  Simple utility function I use to test to see if the file really
// *  exists.  Probably would have been simpler just to put it inline.
// ***************************************************************************
bool fileExists(char *filename) {
    return 0;
}

// ***************************************************************************
// * sendHeader()
// *  Send the content type and rest of the header. For this assignment you
// *  only have to do TXT, HTML and JPG, but you can do others if you want.
// ***************************************************************************
bool sendHeader(int sockfd) {
    return 0;
}

// ***************************************************************************
// * sendFile(int sockfd,char *filename)
// *  Open the file, read it and send it.
// ***************************************************************************
bool sendFile(int sockfd,char *filename) {
    return 0;
}

// ***************************************************************************
// * send404(int sockfd)
// *  Send the whole error page. It can really say anything you like.
// ***************************************************************************
bool send404(int sockfd) {
    return 0;
}


// ***************************************************************************
// * processRequest()
// *  Master function for processing thread.
// *  !!! NOTE - the IOSTREAM library and the cout varibables may or may
// *      not be thread safe depending on your system.  I use the cout
// *      statments for debugging when I know there will be just one thread
// *      but once you are processing multiple rquests it might cause problems.
// ***************************************************************************
void* processRequest(void *arg) {


    // *******************************************************
    // * This is a little bit of a cheat, but if you end up
    // * with a FD of more than 64 bits you are in trouble
    // *******************************************************

    int sockfd = (long)arg;
    printf("fd for this connection: %d\n", sockfd);

    int bufsize = 1024;
    char msg[bufsize];

    int recv_status;
    recv_status = recv(sockfd, msg, bufsize, 0);

    switch (recv_status) {
    case 0:
        fprintf(stderr, "connection closed by client\n");
        return NULL;
    case -1:
        fprintf(stderr,"error, nonblocking socket: %s\n", strerror(errno));
    default:
        printf("message received from client at fd=%d\n", sockfd);
        printf("message length: %d\n", recv_status);
        printf("message contents:\n");
        printf("%s\n", msg);
        break;
    }

    if (DEBUG)
        printf("We are in the thread with fd = %d\n", sockfd);

    // *******************************************************
    // * Now we need to find the GET part of the request.
    // *******************************************************
    char *getRequest = readGetRequest(sockfd);
    if (DEBUG)
        printf("Get request is %s\n", getRequest);


    // *******************************************************
    // * Find the path/file part of the request.
    // *******************************************************
    char *requestedFile = parseGET(getRequest);
    if (DEBUG)
        printf("The file they want is %s\n", requestedFile);

    // *******************************************************
    // * Send the file
    // *******************************************************
    if (fileExists(requestedFile)) {

        // *******************************************************
        // * Build & send the header.
        // *******************************************************
        sendHeader(sockfd);
        if (DEBUG)
            printf("Header sent\n");

        // *******************************************************
        // * Send the file
        // *******************************************************
        sendFile(sockfd,requestedFile);
        if (DEBUG)
            printf("File sent\n");

    } else {
        // *******************************************************
        // * Send an error message 
        // *******************************************************
        if (DEBUG)
            printf("File %s does not exist.\n", requestedFile);
        send404(sockfd);
        if (DEBUG)
            printf("Error message sent.\n");
    }

    if (DEBUG)
        printf("Thread terminating\n");

    return NULL;
}

GArray *insert_unique(GArray *threads, pthread_t *thread) {
    // inserts a unique thread into threads.
    //
    // return value: if thread is in threads, then return the original threads
    // array. otherwise, append thread onto threads and return the updated array

    pthread_t *active_thread;
    for (int i = 0; i < threads->len; ++i) {
        active_thread = g_array_index(threads, pthread_t*, i);

        if (pthread_equal(*thread, *active_thread) != 0) {
            return threads;
        }

        printf("result: %p != %p\n", thread, active_thread);
    }

    // if we're here, then thread doesn't exist in threads, so append it
    g_array_append_val(threads, thread);

    return threads;
}

/****************************************************************************
 *  main
 ****************************************************************************/
int main(int argc, char **argv) {
    // create, bind, establish listening queue, then close a stream type socket

   if (argc != 1) {
        printf("usage %s\n", argv[0]);
        exit(-1);
    }

    if (DEBUG)
        printf("starting server...\n");


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
    int listenfd = -1;
    // and now we call socket(), using the result from out getaddrinfo() call
    if ((listenfd = socket(servinfo->ai_family, servinfo->ai_socktype,
                           servinfo->ai_protocol)) < 0)
    {
        fprintf(stderr, "failed to create listening socket: %s\n", strerror(errno));
        exit(-1);
    }

    /****************************************************************************
     * use the socket descriptor (listenfd), along with the rest of the info
     * we've found, to bind the listening socket to our specified port
     ****************************************************************************/

    if (DEBUG)
        printf("binding socket to port %d...\n", PORT);

    if (bind(listenfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        fprintf(stderr, "error binding socket to local port: %s\n", strerror(errno));
        exit(-1);
    }

    // free the address info struct, we don't need it anymore
    freeaddrinfo(servinfo);


    /****************************************************************************
     * now we'll tell the kernel to listen() for connections on our socket
     * fd (listenfd), which is bind()'d with our port. this creates a listening
     * queue that we'll accept() connections on
     ****************************************************************************/

    if (DEBUG)
        printf("listening for connections...\n");

    int maxconns = 5; // max number of connections allowed in listening queue
    // and now we listen() on our socket
    if (listen(listenfd, maxconns) == -1) {
        fprintf(stderr, "error when trying to listen to bound socket: %s\n", strerror(errno));
        exit(-1);
    }


    /****************************************************************************
     * here comes the mutli-threading -- now that we have a queue waiting for
     * connections, we'll set up a thread for every connection that we accept()
     ****************************************************************************/

    // var to hold the socket descriptor for communication (send() and recv())
    long connfd = -1;
    struct sockaddr_storage client_addr; // to hold the client's addr info
    socklen_t addr_size = sizeof client_addr; // size of client's info struct

    // GArray is a dynamic array type, so let's use that instead of a regular
    // array/pointer to hold our connections (as threads)
    GArray *conns;
    conns = g_array_new(FALSE, FALSE, sizeof(pthread_t*));
    // thread for new connections
    pthread_t *new_conn;
    // and something to hold the return value of pthread_create...
    int pc_status;
    // set detach attr that we can use during thread creation
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // continuous loop to spawn a thread for every connection that is
    // established
    while (true) {
        // 
        if ((connfd = accept(listenfd, (struct sockaddr *)&client_addr, &addr_size)) == -1) {
            fprintf(stderr, "error accepting connection: %s\n", strerror(errno));
            exit(-1);
        }

        if (DEBUG) {
            printf("connection established!\n");
            printf("spawning thread for connection on fd=%ld\n", connfd);
        }

        // allocate space for new connection/thread
        new_conn = malloc(sizeof(pthread_t));
        // ...and create the thread
        pc_status = pthread_create(new_conn, &attr, processRequest, (void *)connfd);

        if (pc_status) {
            fprintf(stderr, "error creating thread, pthread_create exit status: %d\n", pc_status);
            exit(-1);
        }

        conns = insert_unique(conns, new_conn);
    }

    /****************************************************************************
     * we're done, let's clean up a bit
     ****************************************************************************/

    //printf("\ncleaning up...\n");

/*
    int buffer_size = 1024;
    char *write_buffer = malloc(buffer_size * sizeof(char));
    strcpy(write_buffer, "The Matrix has you.\n");
    
    int bytes_sent = 0;
    int msg_len = strlen(write_buffer);
    while (bytes_sent < msg_len) {
        if ((bytes_sent = send(connfd, write_buffer, strlen(write_buffer), 0)) == -1) {
            fprintf(stderr, "error trying to send message to client: %s\n", strerror(errno));
            fprintf(stderr, "number of bytes sent before failing: %d\n",
                    (bytes_sent == -1) ? 0 : bytes_sent);
            exit(-1);
        }
    }

    int recv_status = 1;
    char *read_buffer = malloc(buffer_size * sizeof(char));
    while (recv_status != 0 && !strcmp((const char *)read_buffer, "exit\n")) {
        recv_status = recv(listenfd, read_buffer, buffer_size, 0);

        if (recv_status == -1) {
            fprintf(stderr, "error receiving message from client: %s\n", strerror(errno));
            exit(-1);
        }

        printf("the client sent: %s\n", read_buffer);
    }

    // free memory allocated by malloc()
    //free(write_buffer);
    //free(read_buffer);

*/

    return 0;
}
