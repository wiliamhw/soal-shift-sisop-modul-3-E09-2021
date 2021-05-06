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
#define CURR_DIR "/home/frain8/Documents/Sisop/Modul_3/soal_shift_3/soal1/Server"

int curr_fd = -1;
const int SIZE_BUFFER = sizeof(char) * DATA_BUFFER;

// Essential
int create_tcp_server_socket();
void setup_epoll_connection(int epfd, int fd, struct epoll_event *event);
void *routes(void *argv);

// Controller
void login(char *buf, int fd);
void regist(char *buf, int fd);

// Helper
int getInput(char *buf, int fd);
int getCredentials(int fd, char *id, char *password);
bool isRegistered(FILE *fp, char *id);
bool isValid(FILE *fp, char *id, char *password);

int main()
{
    socklen_t addrlen;
    struct sockaddr_in new_addr;
    struct epoll_event connections[MAX_CONNECTIONS], epoll_temp;
    pthread_t tid;
    char buf[DATA_BUFFER], argv[DATA_BUFFER + 2];
    int new_fd, ret_val, temp_fd, temp_ret_val;

    int timeout_msecs = 1500;
    int epfd = epoll_create(1);
    int server_fd = create_tcp_server_socket();
    setup_epoll_connection(epfd, server_fd, &epoll_temp);

    while (1) {
        ret_val = epoll_wait(epfd, connections, MAX_CONNECTIONS, timeout_msecs /*timeout*/);
        for (int i = 0; i < ret_val; i++) {
            if (connections[i].data.fd == server_fd) {
                new_fd = accept(server_fd, (struct sockaddr *)&new_addr, &addrlen);
                if (new_fd >= 0) {
                    setup_epoll_connection(epfd, new_fd, &epoll_temp);
                    printf("Accepted a new connection with fd: %d\n", new_fd);
                    pthread_create(&tid, NULL, &routes, (void *) &new_fd);
                } else {
                    fprintf(stderr, "Accept failed [%s]\n", strerror(errno));
                }
            }
        }
    } /* while(1) */
    return 0;
}

void *routes(void *argv)
{
    int fd = *(int *) argv;
    char cmd[DATA_BUFFER];

    while (recv(fd, cmd, DATA_BUFFER, MSG_PEEK | MSG_DONTWAIT) != 0) {
        send(fd, "\nSelect command:\n1. Login\n2. Register\n", DATA_BUFFER, 0);
        if (getInput(cmd, fd) == 0) break;

        if (strcmp(cmd, "login") == 0 || strcmp(cmd, "1") == 0) {
            login(cmd, fd);
        } else if (strcmp(cmd, "register") == 0 || strcmp(cmd, "2") == 0) {
            regist(cmd, fd);
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

void login(char *buf, int fd)
{
    if (curr_fd != -1) {
        send(fd, "Server is busy. Wait until other client has logout.\n", SIZE_BUFFER, 0);
        return;
    }
    char id[DATA_BUFFER], password[DATA_BUFFER];
    sprintf(buf, "%s/%s", CURR_DIR, "akun.txt");

    FILE *fp = fopen(buf, "a+");
    if (getCredentials(fd, id, password) == 0) return;

    if (isValid(fp, id, password)) {
        send(fd, "Login success\n", SIZE_BUFFER, 0);
        curr_fd = fd;
    } else {
        send(fd, "Wrong id or password\n", SIZE_BUFFER, 0);
    }
    fclose(fp);
}

void regist(char *buf, int fd)
{
    char id[DATA_BUFFER], password[DATA_BUFFER];
    sprintf(buf, "%s/%s", CURR_DIR, "akun.txt");

    FILE *fp = fopen(buf, "a+");
    if (getCredentials(fd, id, password) == 0) return;

    if (isRegistered(fp, id)) {
        send(fd, "Id is already registered\n", SIZE_BUFFER, 0);
    } else {
        fprintf(fp, "%s:%s\n", id, password);
        send(fd, "Register success\n", SIZE_BUFFER, 0);
    }
    fclose(fp);
}

int getCredentials(int fd, char *id, char *password)
{
    send(fd, "Insert id: ", SIZE_BUFFER, 0);
    if (getInput(id, fd) == 0) return 0;
    send(fd, "Insert password: ", SIZE_BUFFER, 0);
    if (getInput(password, fd) == 0) return 0;
    return 1;
}

int getInput(char *buf, int fd)
{
    int count, ret_val;
    ioctl(fd, FIONREAD, &count);
    count /= DATA_BUFFER;
    for (int i = 0; i <= count; i++) {
        ret_val = recv(fd, buf, DATA_BUFFER, 0);
        if (ret_val == 0)
            break;
    }
    return ret_val;
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