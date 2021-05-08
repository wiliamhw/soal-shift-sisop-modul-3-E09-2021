#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <netinet/in.h>

#define DATA_BUFFER 500
#define MAX_CONNECTIONS 10
#define CURR_DIR "/home/frain8/Documents/Sisop/Modul_3/soal_shift_3/soal1/Server"

int curr_fd = -1;
const int SIZE_BUFFER = sizeof(char) * DATA_BUFFER;

// Socket setup
int create_tcp_server_socket();
void setup_epoll_connection(int epfd, int fd, struct epoll_event *event);

// Routes & controller
void *routes(void *argv);
void login(char *buf, int fd);
void regist(char *buf, int fd);
void add(char *buf, int fd);
void download(char *buf, int fd);

// Helper
int getInput(int fd, char *prompt, char *storage);
int getCredentials(int fd, char *id, char *password);
int writeFile(int fd, char *dirname, char *targetFileName);
int sendFile(int fd, char *filename);
char *getFileName(char *filePath);
bool validLogin(FILE *fp, char *id, char *password);
bool isRegistered(FILE *fp, char *id);
bool alreadyDownloaded(FILE *fp, char *filename);

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
    chdir(CURR_DIR);

    while (recv(fd, cmd, DATA_BUFFER, MSG_PEEK | MSG_DONTWAIT) != 0) {
        public route
        if (fd != curr_fd) {
            if (getInput(fd, "\nSelect command:\n1. Login\n2. Register\n", cmd) == 0) break;

            if (strcmp(cmd, "login") == 0 || strcmp(cmd, "1") == 0) {
                login(cmd, fd);
            } else if (strcmp(cmd, "register") == 0 || strcmp(cmd, "2") == 0) {
                regist(cmd, fd);
            } else {
                send(fd, "Invalid command\n", sizeof(char) * 20, 0);
            }
        } else { // protected route
            if (getInput(fd, "\nSelect command:\n1. Add\n2. Download <filename with extension>\n", cmd) == 0) break;

            if (strcmp(cmd, "add") == 0 || strcmp(cmd, "1") == 0) {
                add(cmd, fd);
            } else {
                char *tmp = strtok(cmd, " ");
                char *tmp2 = strtok(NULL, " ");
                if (strcasecmp(tmp, "download") == 0 && tmp2) {
                    strcpy(cmd, tmp2);
                    download(cmd, fd);
                } else {
                    send(fd, "Invalid command\n", sizeof(char) * 20, 0);
                }
            }
        // }
        sleep(0.001);
    }
    if (fd == curr_fd) {
        curr_fd = -1;
    }
    printf("Close connection with fd: %d\n", fd);
    close(fd);
}

/****   Controllers   *****/
void download(char *buf, int fd)
{
    FILE *fp = fopen("files.tsv", "a+");
    if (alreadyDownloaded(fp, buf)) {
        sendFile(fd, buf);
    } else {
        send(fd, "Error, file hasn't been downloaded\n", SIZE_BUFFER, 0);
    }
    fclose(fp);
}

void add(char *buf, int fd)
{
    char *dirName = "FILES";
    char publisher[DATA_BUFFER], year[DATA_BUFFER], client_path[DATA_BUFFER];
    if (getInput(fd, "Publisher: ", publisher) == 0) return;
    if (getInput(fd, "Tahun Publikasi: ", year) == 0) return;
    if (getInput(fd, "Filepath: ", client_path) == 0) return;

    FILE *fp = fopen("files.tsv", "a+");
    char *fileName = getFileName(client_path);

    if (alreadyDownloaded(fp, fileName)) {
        send(fd, "Error, file is already uploaded\n", SIZE_BUFFER, 0);
    } else {
        send(fd, "\nStart sending file\n", SIZE_BUFFER, 0);
        mkdir(dirName, 0777);
        if (writeFile(fd, dirName, fileName) == 0) {
            fprintf(fp, "%s|%s|%s\n", fileName, publisher, year);
            printf("Store file finished\n");
        } else {
            printf("Error occured when receiving file\n");
        }
    }
    fclose(fp);
}

void login(char *buf, int fd)
{
    if (curr_fd != -1) {
        send(fd, "Server is busy. Wait until other client has logout.\n", SIZE_BUFFER, 0);
        return;
    }
    char id[DATA_BUFFER], password[DATA_BUFFER];
    FILE *fp = fopen("akun.txt", "a+");

    if (getCredentials(fd, id, password) != 0) {
        if (validLogin(fp, id, password)) {
            send(fd, "Login success\n", SIZE_BUFFER, 0);
            curr_fd = fd;
        } else {
            send(fd, "Wrong id or password\n", SIZE_BUFFER, 0);
        }
    }
    fclose(fp);
}

void regist(char *buf, int fd)
{
    char id[DATA_BUFFER], password[DATA_BUFFER];
    FILE *fp = fopen("akun.txt", "a+");

    if (getCredentials(fd, id, password) != 0) {
        if (isRegistered(fp, id)) {
            send(fd, "Id is already registered\n", SIZE_BUFFER, 0);
        } else {
            fprintf(fp, "%s:%s\n", id, password);
            send(fd, "Register success\n", SIZE_BUFFER, 0);
        }
    }
    fclose(fp);
}

/*****  HELPER  *****/
int sendFile(int fd, char *filename)
{
    char buf[DATA_BUFFER] = {0};
    int ret_val;
    printf("Sending [%s] file to client!\n", filename);
    strcpy(buf, filename);
    sprintf(filename, "FILES/%s", buf);
    FILE *fp = fopen(filename, "r");

    if (!fp) {
        printf("File not found\n");
        send(fd, "File not found\n", SIZE_BUFFER, 0);
        return -1;
    }
    send(fd, "\nStart receiving file\n", SIZE_BUFFER, 0);
    send(fd, buf, SIZE_BUFFER, 0);

    // Transfer size
    fseek(fp, 0L, SEEK_END);
    int size = ftell(fp);
    rewind(fp);
    sprintf(buf, "%d", size);
    send(fd, buf, SIZE_BUFFER, 0);

    while ((ret_val = fread(buf, 1, DATA_BUFFER, fp)) > 0) {
        send(fd, buf, ret_val, 0);
    }
    recv(fd, buf, DATA_BUFFER, 0);
    printf("Send file finished\n");
    fclose(fp);
    return 0;
}

char *getFileName(char *filePath)
{
    char *ret = strrchr(filePath, '/');
    if (ret) return ret;
    else return filePath;
}

int writeFile(int fd, char *dirname, char *targetFileName)
{
    int ret_val, size;
    char buf[DATA_BUFFER], in[1];

    // Make sure that client has the file
    ret_val = recv(fd, buf, DATA_BUFFER, 0);
    if (ret_val == 0 || strcmp(buf, "File found") != 0) {
        if (ret_val == 0) printf("Connection to client lost\n");
        else puts(buf);
        return -1;
    }
    recv(fd, buf, SIZE_BUFFER, 0);
    size = atoi(buf);

    printf("Store [%s] file from client\n", targetFileName);
    sprintf(buf, "%s/%s", dirname,targetFileName);
    FILE *fp = fopen(buf, "w+");

    while (size-- > 0) {
        if ((ret_val = recv(fd, in, 1, 0)) < 0)
            return ret_val;
        fwrite(in, 1, 1, fp);
    }
    ret_val = 0;
    printf("Storing file finished\n");
    fclose(fp);
    return ret_val;
}

int getCredentials(int fd, char *id, char *password)
{
    if (getInput(fd, "Insert id: ", id) == 0) return 0;
    if (getInput(fd, "Insert password: ", password) == 0) return 0;
    return 1;
}

int getInput(int fd, char *prompt, char *storage)
{
    send(fd, prompt, SIZE_BUFFER, 0);

    // Get input
    int count, ret_val;
    ioctl(fd, FIONREAD, &count);
    count /= DATA_BUFFER;
    for (int i = 0; i <= count; i++) {
        ret_val = recv(fd, storage, DATA_BUFFER, 0);
        if (ret_val == 0) break;
    }
    printf("Input: [%s]\n", storage);
    return ret_val;
}

bool validLogin(FILE *fp, char *id, char *password)
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

bool alreadyDownloaded(FILE *fp, char *filename)
{
    char db[DATA_BUFFER], *tmp;
    while (fscanf(fp, "%s", db) != EOF) {
        tmp = strtok(db, "|");
        if (strcmp(tmp, filename) == 0) return true;
    }
    return false;
}

/****   SOCKET SETUP    *****/
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
