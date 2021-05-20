#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>
#include <dirent.h>

#define SIZE_BUFFER 400

typedef struct {
    char path[SIZE_BUFFER];
    FILE *fp;
} FileInfo;

pthread_t tid[5000];
FileInfo file_info[5000];
bool success[5000];

void *moveFile(void *buf);
void _moveFile(FILE *src_fp, const char *target_path);
void listFilesRecursively(char *base_path, int *i, int depth);

/** Helpers **/
char *getFileName(char *file_path);
char *getExtension(const char *file_name);
char *toLower(char *str);
bool validFileInfo(FileInfo *file_info, char *file_path);

int main(int argc, char *argv[])
{
    int i = 0;
    if (argc < 3) {
        if (strcmp(argv[1], "*") == 0) {
            listFilesRecursively(".", &i, 0);
        } else {
            printf("Tidak ada flag/file yang akan dikelompokan\n");
            return 1;
        } 
    } else {
        argc -= 2;
        if (strcmp(argv[1], "-f") == 0) {
            for (; i < argc; i++) {
                if (validFileInfo(&file_info[i], argv[i + 2])) {
                    pthread_create(&tid[i], NULL, &moveFile, (void *) &file_info[i]);
                    printf("File %d: Berhasil Dikategorisasikan\n", i + 1);
                    success[i] = true;
                } else {
                    printf("File %d: Sad, gagal :(\n", i + 1);
                    success[i] = false;
                } 
            }
        } else if (strcmp(argv[1], "-d") == 0) {
            listFilesRecursively(argv[2], &i, 0);
        }
        else {
            printf("Flag tidak valid\n");
            return 1;
        }
    }
    for (int j = 0; j <= i; j++) {
        if (!success[j]) continue;
        pthread_join(tid[j], NULL);
    }
    return 0;
}

void listFilesRecursively(char *base_path, int *i, int depth)
{
    char path[1000];
    struct dirent *dc; // Store one dir content

    DIR *dir = opendir(base_path);
    if (dir == NULL) {
        if (depth == 0) printf("Yah, gagal disimpan :(\n");
        return;
    }
    while ((dc = readdir(dir)) != NULL) {
        if (strcmp(dc->d_name, ".") != 0 && strcmp(dc->d_name, "..") != 0) {

            // Construct new path from the base path
            strcpy(path, base_path);
            strcat(path, "/");
            strcat(path, dc->d_name);

            if (validFileInfo(&file_info[*i], path)) {
                pthread_create(&tid[*i], NULL, &moveFile, (void *) &file_info[*i]);
                success[*i] = true;
                (*i)++;
            } else {
                success[*i] = false;
            } 
            listFilesRecursively(path, i, depth + 1);
        }
    }
    if (depth == 0) printf("Direktori sukses disimpan!\n");
    closedir(dir);
}

void *moveFile(void *buf)
{
    char target_path[SIZE_BUFFER], filename[SIZE_BUFFER / 2], ext[SIZE_BUFFER / 2];
    FileInfo src_info = *(FileInfo *) buf;

    strcpy(filename, toLower(getFileName(src_info.path)));
    strcpy(ext, toLower(getExtension(filename)));

    mkdir(ext, 0777);
    sprintf(target_path, "%s/%s", ext, filename);
    _moveFile(src_info.fp, target_path);
    fclose(src_info.fp);
    remove(src_info.path);
}

void _moveFile(FILE *src_fp, const char *target_path)
{
    FILE *target_fp = fopen(target_path, "w+");
    char buf;

    while (fread(&buf, 1, sizeof(char), src_fp) > 0) {
        fwrite(&buf, 1, sizeof(char), target_fp);
    }
    fclose(target_fp);
}

/*** Helpers ***/
char *toLower(char *str)
{
    if (strcasecmp(str, "Hidden") != 0 && strcasecmp(str, "Unknown") != 0) {
        for(int i = 0; str[i]; i++){
            str[i] = tolower(str[i]);
        }
    }
    return str;
}

char *getExtension(const char *file_name)
{
    char temp[SIZE_BUFFER];
    char *result = NULL;
    strcpy(temp, file_name);

    if (temp[0] == '.') {
        result = "Hidden";
    } else {
        char *ext = strchr(temp, '.');
        if (ext) result = ext + 1;
        else result = "Unknown";
    }
    return result;
}

char *getFileName(char *file_path)
{
    char temp[SIZE_BUFFER];
    strcpy(temp, file_path);

    char *ret = strrchr(temp, '/');
    if (ret) return ret + 1;
    else return file_path;
}

bool validFileInfo(FileInfo *file_info, char *file_path)
{
    struct stat path_stat;
    stat(file_path, &path_stat);
    bool isDir = S_ISDIR(path_stat.st_mode);

    strcpy(file_info->path, file_path);
    file_info->fp = fopen(file_info->path, "r");
    return (file_info->fp && !isDir);
}
