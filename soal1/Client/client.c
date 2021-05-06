#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>

#define DATA_BUFFER 500
#define CURR_DIR "/home/frain8/Documents/Sisop/Modul_3/soal_shift_3/soal1/Client"

int client_fd;
const int SIZE_BUFFER = sizeof(char) * DATA_BUFFER;
char inputPath[DATA_BUFFER];
bool _inputPath = false;

int create_tcp_client_socket();
void *handleInput();
void *handleOutput();
void getInput(int fd, char *input);
void sendFile(int fd);

int main()
{
    pthread_t tid[2];
    client_fd = create_tcp_client_socket();

    pthread_create(&(tid[0]), NULL, &handleOutput, NULL);
    pthread_create(&(tid[1]), NULL, &handleInput, NULL);

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);

    close(client_fd);
    return 0;
}

void *handleInput()
{
    chdir(CURR_DIR);
    char message[DATA_BUFFER];
    send(client_fd, message, sizeof(message), 0);

    while (1) {
        scanf("%s", message);
        send(client_fd, message, sizeof(message), 0);
        if (_inputPath) {
            strcpy(inputPath, message);
            sendFile(client_fd);
            _inputPath = false;
        }
        // printf("Successfully sent data: %s\n", message);
    }
}

void *handleOutput() 
{
    chdir(CURR_DIR);
    char message[DATA_BUFFER];

    while (1) {
        getInput(client_fd, message);
        printf("%s", message);
        if (strcmp(message, "Filepath: ") == 0) {
            _inputPath = true;
        }
        fflush(stdout);
    }
}

void sendFile(int fd)
{
    // printf("\nSending %s to server!\n", inputPath);
    FILE *fp = fopen(inputPath, "r");
    char buf[DATA_BUFFER] = {0};

    if (fp) {
        send(fd, "File found", SIZE_BUFFER, 0);
        while (fgets(buf, DATA_BUFFER, fp)) {
            if (send(fd, buf, sizeof(buf), 0) == -1) {
                // printf("Error in sending file\n\n");
                send(fd, "Error in sending file", SIZE_BUFFER, 0);
                break;
            }
            memset(buf, 0, SIZE_BUFFER);
        }
        // printf("Send file finished\n\n");
        send(fd, "Send file finished", SIZE_BUFFER, 0); 
        fclose(fp);
    } else {
        // printf("File not found\n\n");
        send(fd, "File not found", SIZE_BUFFER, 0);
    }
}

void getInput(int fd, char *input)
{
    if (recv(fd, input, DATA_BUFFER, 0) == 0) {
        printf("Server shutdown\n");
        exit(EXIT_SUCCESS);
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