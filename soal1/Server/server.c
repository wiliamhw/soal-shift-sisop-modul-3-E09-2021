#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <pthread.h>

#define DATA_BUFFER 500
#define MAX_CONNECTIONS 5
#define SUCCESS_MESSAGE "Your message delivered successfully"
#define NOTVALID_MESSAGE "Tidak dapat terkoneksi dengan server. Tunggu client lain disconnect terlebih dahulu\n"

int curr_fd = -1;

int create_tcp_server_socket();
void setup_epoll_connection(int epfd, int fd, struct epoll_event *event);
void *handleIO(void *_fd);
void command(char *cmd, int fd);
void regist(char *buf, int fd);

int main()
{
    socklen_t addrlen;
    struct sockaddr_in new_addr;
    struct epoll_event connections[MAX_CONNECTIONS], epoll_temp;
    pthread_t tid;
    char buf[DATA_BUFFER];
    int server_fd, new_fd, ret_val, temp_fd;
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

                    if (curr_fd == -1) { // Initialize curr_fd
                        curr_fd = new_fd;
                    } 
                    else if (recv(curr_fd, buf, DATA_BUFFER, MSG_PEEK | MSG_DONTWAIT) != 0) { // A client is logged in
                        send(new_fd, NOTVALID_MESSAGE, sizeof(NOTVALID_MESSAGE), 0);
                        printf("Disconnect connection with fd: %d\n", new_fd);
                        close(new_fd);
                    } 
                    else { // No client is logged in
                        curr_fd = new_fd;
                    }
                } else {
                    fprintf(stderr, "accept failed [%s]\n", strerror(errno));
                }
            } // Get input from connection
            else if (connections[i].events & EPOLLIN) {
                if ((temp_fd = connections[i].data.fd) < 0) continue;
                pthread_create(&tid, NULL, &handleIO, (void *) &connections[i].data.fd);
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

void *handleIO(void *_fd)
{
    char buf[DATA_BUFFER];
    int fd = *(int *) _fd;
    int ret_val = recv(fd, buf, DATA_BUFFER, MSG_PEEK | MSG_DONTWAIT);

    if (ret_val > 0) {
        ret_val = recv(fd, buf, DATA_BUFFER, 0);
        // printf("Returned fd is %d\n", fd);
        printf("Received data (len %d bytes, fd: %d): %s\n", ret_val, fd, buf);
        fflush(stdin);
        command(buf, fd);
    }
    else if (ret_val == 0) {
        close(curr_fd);
        curr_fd = -1;
    }
}

void command(char *cmd, int fd)
{
    if (strcmp(cmd, "register") == 0 || strcmp(cmd, "1") == 0) {
        return regist(cmd, fd);
    }
}

void regist(char *buf, int fd)
{
    char id[DATA_BUFFER], password[DATA_BUFFER];

    strcpy(id, "Masukan id: ");
    send(fd, id, sizeof(id), 0);
    recv(fd, id, DATA_BUFFER, 0);
    printf("Received id (fd: %d): %s\n", fd, id);

    strcpy(password, "Masukan id: ");
    send(fd, password, sizeof(password), 0);
    recv(fd, password, DATA_BUFFER, 0);
    printf("Received password (fd: %d): %s\n", fd, password);
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