#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <stdlib.h>

#define NOTVALID_MESSAGE "Tidak dapat terkoneksi dengan server. Tunggu client lain disconnect terlebih dahulu\n"

int client_fd;

int create_tcp_client_socket();
void *handleInput();
void *handleOutput();

int main()
{
    pthread_t tid[2];
    client_fd = create_tcp_client_socket();

    pthread_create(&(tid[0]), NULL, &handleOutput, NULL);
    pthread_create(&(tid[1]), NULL, &handleInput, NULL);

    sleep(1);
    printf("Pilih input:\n1. Login\n2. Register\n");

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);

    close(client_fd);
    return 0;
}

void *handleInput(void *_fd)
{
    char message[1000];
    while (1) {
        gets(message);
        send(client_fd, message, sizeof(message), 0);
        // printf("\nSuccessfully sent data: %s\n", message);
    }
}

void *handleOutput(void *_fd) 
{
    char message[1000];

    while (1) {
        recv(client_fd, message, sizeof(message), 0);
        printf("%s", message);

        if (strcmp(message, NOTVALID_MESSAGE) == 0) {
            exit(EXIT_SUCCESS);
        }
    }
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