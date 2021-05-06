#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/ioctl.h>

#define DATA_BUFFER 500
#define MAX_CONNECTIONS 10
#define SUCCESS_MESSAGE "Your message delivered successfully"
#define CURR_DIR "/home/frain8/Documents/Sisop/Modul_3/soal_shift_3/soal1/Server"
#define SELECT_PROS "\nSelect command:\n1. Login\n2. Register\n"

int curr_fd = -1;

int create_tcp_server_socket();
void setup_epoll_connection(int epfd, int fd, struct epoll_event *event);
void *command(void *argv);
int getInput(char *buf, int fd);

bool isRegistered(FILE *fp, char *id);
bool isValid(FILE *fp, char *id, char *password);

void logReg(char *buf, int fd, bool isLogin);

int main()
{
    socklen_t addrlen;
    struct sockaddr_in new_addr;
    struct epoll_event connections[MAX_CONNECTIONS], epoll_temp;
    pthread_t tid;
    char buf[DATA_BUFFER], argv[DATA_BUFFER + 2];
    int server_fd, new_fd, ret_val, temp_fd, temp_ret_val;
    int timeout_msecs = 1500;
    int epfd = epoll_create(1);
    
    /* Get the socket server fd */
    server_fd = create_tcp_server_socket();
    setup_epoll_connection(epfd, server_fd, &epoll_temp);

    while (1) {
        ret_val = epoll_wait(epfd, connections, MAX_CONNECTIONS, timeout_msecs /*timeout*/);

        for (int i = 0; i < ret_val; i++) {

            // Make new connection
            if (connections[i].data.fd == server_fd) {
                new_fd = accept(server_fd, (struct sockaddr *)&new_addr, &addrlen);
                if (new_fd >= 0) {
                    setup_epoll_connection(epfd, new_fd, &epoll_temp);
                    printf("Accepted a new connection with fd: %d\n", new_fd);
                    pthread_create(&tid, NULL, &command, (void *) &new_fd);
                } else {
                    fprintf(stderr, "accept failed [%s]\n", strerror(errno));
                }
            }
        }
    } /* while(1) */

    /* Last step: Close all the sockets */
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (connections[i].data.fd > 0) {
            close(connections[i].data.fd);
        }
    }
    return 0;
}

int getInput(char *buf, int fd)
{
    int count, ret_val;
    ioctl(fd, FIONREAD, &count);
    count /= DATA_BUFFER;
    for (int i = 0; i <= count; i++) {
        ret_val = recv(fd, buf, DATA_BUFFER, 0);
    }
    return ret_val;
}

void *command(void *argv)
{
    int fd = *(int *) argv;
    char cmd[DATA_BUFFER];
    int retval;

    while (recv(fd, cmd, DATA_BUFFER, MSG_PEEK | MSG_DONTWAIT) != 0) {
        send(fd, SELECT_PROS, sizeof(SELECT_PROS), 0);
        getInput(cmd, fd);
        if (strcmp(cmd, "login") == 0 || strcmp(cmd, "1") == 0) {
            if (curr_fd == -1) {
                logReg(cmd, fd, true);
            } else {
                send(fd, "Server is busy. Wait until other client has logout.\n", sizeof(char) * DATA_BUFFER, 0);
            }
        } else if (strcmp(cmd, "register") == 0 || strcmp(cmd, "2") == 0) {
            logReg(cmd, fd, false);
        } else {
            send(fd, "Invalid command\n", sizeof(char) * 20, 0);
        }
        sleep(0.001);
    }
    if (fd == curr_fd) {
        curr_fd = -1;
    }
    close(fd);
}

void logReg(char *buf, int fd, bool isLogin)
{
    char id[DATA_BUFFER], password[DATA_BUFFER], path[DATA_BUFFER];
    sprintf(path, "%s/%s", CURR_DIR, "akun.txt");
    FILE *fp = fopen(path, "a+");

    strcpy(id, "Insert id: ");
    send(fd, id, sizeof(id), 0);
    if (getInput(id, fd) == 0) return;
    printf("Received id (fd: %d): %s\n", fd, id);

    strcpy(password, "Insert password: ");
    send(fd, password, sizeof(password), 0);
    if (getInput(password, fd) == 0) return;
    printf("Received password (fd: %d): %s\n", fd, password);

    if (isLogin) {
        if (isValid(fp, id, password)) {
            strcpy(path, "Login success\n");
            send(fd, path, sizeof(path), 0);
            curr_fd = fd;
        } else {
            strcpy(path, "Wrong id or password\n");
            send(fd, path, sizeof(path), 0);
        }
    } else {
        if (isRegistered(fp, id)) {
            strcpy(path, "Id is already registered\n");
            send(fd, path, sizeof(path), 0);
        } else {
            fprintf(fp, "%s:%s\n", id, password);
            strcpy(path, "Register success\n");
            send(fd, path, sizeof(path), 0);
        }
    }
    fclose(fp);
    return;
}

bool isValid(FILE *fp, char *id, char *password)
{
    char db[DATA_BUFFER], input[DATA_BUFFER];
    sprintf(input, "%s:%s", id, password);
    while (fscanf(fp, "%s", db) != EOF) {
        if (strcmp(db, input) == 0) return true;
    }
    return false;
}

bool isRegistered(FILE *fp, char *id)
{
    char db[DATA_BUFFER], *tmp;
    while (fscanf(fp, "%s", db) != EOF) {
        tmp = strtok(db, ":");
        if (strcmp(tmp, id) == 0) return true;
    }
    return false;
}

int create_tcp_server_socket()
{
    struct sockaddr_in saddr;
    int fd, ret_val;
    int opt = 1;

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

    /* Initialize the socket address structure */
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(7000);
    saddr.sin_addr.s_addr = INADDR_ANY;

    /* Step2: bind the socket to port 7000 on the local host */
    ret_val = bind(fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (ret_val != 0) {
        fprintf(stderr, "bind failed [%s]\n", strerror(errno));
        close(fd);
        exit(EXIT_FAILURE);
    }

    /* Step3: listen for incoming connections */
    ret_val = listen(fd, 5);
    if (ret_val != 0) {
        fprintf(stderr, "listen failed [%s]\n", strerror(errno));
        close(fd);
        exit(EXIT_FAILURE);
    }
    return fd;
}

void setup_epoll_connection(int epfd, int fd, struct epoll_event *event)
{
    event->events = EPOLLIN;
    event->data.fd = fd;

    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, event);
}