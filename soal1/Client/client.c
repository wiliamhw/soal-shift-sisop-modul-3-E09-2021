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

const int SIZE_BUFFER = sizeof(char) * DATA_BUFFER;
char inputPath[DATA_BUFFER];
bool _inputPath = false;

int create_tcp_client_socket();
void *handleInput(void *client_fd);
void *handleOutput(void *client_fd);
void getServerInput(int fd, char *input);
void sendFile(int fd);
void writeFile(int fd);

int main()
{
    pthread_t tid[2];
    int client_fd = create_tcp_client_socket();

    pthread_create(&(tid[0]), NULL, &handleOutput, (void *) &client_fd);
    pthread_create(&(tid[1]), NULL, &handleInput, (void *) &client_fd);

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);

    close(client_fd);
    return 0;
}

void *handleInput(void *client_fd)
{
    chdir(CURR_DIR);
    int fd = *(int *) client_fd;
    char message[DATA_BUFFER];
    send(fd, message, sizeof(message), 0);

    while (1) {
        gets(message);
        send(fd, message, sizeof(message), 0);
        if (_inputPath) {
            strcpy(inputPath, message);
        }
        // printf("Successfully sent data: %s\n", message);
    }
}

void *handleOutput(void *client_fd) 
{
    chdir(CURR_DIR);
    int fd = *(int *) client_fd;
    char message[DATA_BUFFER];

    while (1) {
        getServerInput(fd, message);
        printf("%s", message);
        
        if (strcmp(message, "Filepath: ") == 0) {
            _inputPath = true;
        } else if (strcmp(message, "\nStart sending file\n") == 0) {
            sendFile(fd);
            _inputPath = false;
        } else if (strcmp(message, "Error, file is already uploaded\n") == 0) {
            _inputPath = false;
        } else if (strcmp(message, "\nStart receiving file\n") == 0) {
            writeFile(fd);
        } 
        fflush(stdout);
    }
}

void sendFile(int fd)
{
    printf("Sending [%s] file to server!\n", inputPath);
    FILE *fp = fopen(inputPath, "r");
    char buf[DATA_BUFFER] = {0};

    if (fp) {
        send(fd, "File found", SIZE_BUFFER, 0);
        while (fgets(buf, DATA_BUFFER, fp)) {
            if (send(fd, buf, sizeof(buf), 0) == -1) {
                printf("Error in sending file\n");
                send(fd, "Error in sending file", SIZE_BUFFER, 0);
                break;
            }
            memset(buf, 0, SIZE_BUFFER);
        }
        printf("Send file finished\n");
        send(fd, "Send file finished", SIZE_BUFFER, 0); 
        fclose(fp);
    } else {
        printf("File not found\n");
        send(fd, "File not found", SIZE_BUFFER, 0);
    }
}

void writeFile(int fd)
{
    char buf[DATA_BUFFER] = {0};
    int ret_val = recv(fd, buf, DATA_BUFFER, 0);
    FILE *fp = fopen(buf, "w+");
    memset(buf, 0, SIZE_BUFFER);

    while ((ret_val = recv(fd, buf, DATA_BUFFER, 0)) != 0) {
        if (strcmp(buf, "Send file finished") == 0) {
            puts(buf);
            break;
        } else if (ret_val <= 0) {
            printf("Connection lost\n.");
            break;
        }
        fprintf(fp, "%s", buf);
        memset(buf, 0, SIZE_BUFFER);
    }
    fclose(fp);
}


void getServerInput(int fd, char *input)
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