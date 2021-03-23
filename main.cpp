#define _GNU_SOURCE 1
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define USER_LIMIT 5
#define BUFFER_SIZE 64
#define FD_LIMIT 65535
#define CONNECTION_REQUEST_QUEUE_SIZE 10
#define MAX_EVENT_NUMBER 1024


int main(int argc, char* argv[]) {
    // parse cmd
    // read config
    // start server
    return 0;
}
