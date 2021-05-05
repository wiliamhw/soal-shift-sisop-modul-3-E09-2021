#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>
#include <stdlib.h>

#define DATA_BUFFER 1000
#define MAX_CONNECTIONS 10
#define SUCCESS_MESSAGE "Your message delivered successfully"
#define NOTVALID_MESSAGE "Unable to login until the current logged-in client logged out from the system"

int create_tcp_server_socket();
void setup_epoll_connection(int epfd, int fd, struct epoll_event *event);

int main()
{
    socklen_t addrlen;
    struct sockaddr_in new_addr;
    struct epoll_event connections[MAX_CONNECTIONS], epoll_temp;
    int server_fd, new_fd, ret_val, temp_fd, curr_fd = -1;
    int timeout_msecs = 1500;
    int epfd = epoll_create(1);
    char buf[DATA_BUFFER];
    
    /* Get the socket server fd */
    server_fd = create_tcp_server_socket();
    setup_epoll_connection(epfd, server_fd, &epoll_temp);

    while (1) {
        ret_val = epoll_wait(epfd, connections, MAX_CONNECTIONS, timeout_msecs /*timeout*/);
        printf("Epoll wait ret_val: %d\n", ret_val);

        for (int i = 0; i < ret_val; i++) {
            printf("Connetions on %d index with fd: %d\n", i, connections[i].data.fd);

            // Make new connection
            if (connections[i].data.fd == server_fd) {
                new_fd = accept(server_fd, (struct sockaddr *)&new_addr, &addrlen);
                printf("New_fd: %d\n", new_fd);

                if (new_fd >= 0) {
                    setup_epoll_connection(epfd, new_fd, &epoll_temp);
                    printf("Accepted a new connection with fd: %d\n", new_fd);

                    if (curr_fd == -1) { // Initialize curr_fd
                        curr_fd = new_fd;
                    } 
                    else if (recv(curr_fd, buf, DATA_BUFFER, MSG_PEEK | MSG_DONTWAIT) != 0) { // Logged in
                        send(new_fd, NOTVALID_MESSAGE, sizeof(NOTVALID_MESSAGE), 0);
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
                printf("EPOLLIN section\n");

                if ((temp_fd = connections[i].data.fd) < 0) continue;

                ret_val = recv(connections[i].data.fd, buf, DATA_BUFFER, 0);
                printf("EPPLOIN recv: %d\n", ret_val);

                if (ret_val > 0) {
                    printf("Returned fd is %d [index, i: %d]\n", connections[i].data.fd, i);
                    printf("Received data (len %d bytes, fd: %d): %s\n", ret_val, connections[i].data.fd, buf);
                    ret_val = send(connections[i].data.fd, SUCCESS_MESSAGE, sizeof(SUCCESS_MESSAGE), 0);
                }
                else if (ret_val == 0) {
                    close(curr_fd);
                    curr_fd = -1;
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