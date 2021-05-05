#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>

int create_tcp_client_socket();

int main()
{
    char message[1000];
    int ret_val;
    int client_fd = create_tcp_client_socket();

    while (1) {
        /* Next step: send some data */
        gets(message);
        ret_val = send(client_fd, message, sizeof(message), 0);
        printf("Successfully sent data (len %d bytes): %s\n",
               ret_val, message);
        ret_val = recv(client_fd, message, sizeof(message));
        puts(message);
    }

    /* Last step: close the socket */
    close(client_fd);
    return 0;
}

int create_tcp_client_socket()
{
    struct sockaddr_in saddr;
    int fd, ret_val;
    int opt = 1;
    struct hostent *local_host; /* need netdb.h for this */

    /* Step1: create a TCP socket */
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd == -1) {
        fprintf(stderr, "socket failed [%s]\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    printf("Created a socket with fd: %d\n", fd);

    /* Let us initialize the server address structure */
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(7000);
    local_host = gethostbyname("127.0.0.1");
    saddr.sin_addr = *((struct in_addr *)local_host->h_addr);

    /* Step2: connect to the TCP server socket */
    ret_val = connect(fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (ret_val == -1) {
        fprintf(stderr, "connect failed [%s]\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return fd;
}