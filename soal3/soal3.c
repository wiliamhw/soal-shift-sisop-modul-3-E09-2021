#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>

#define SIZE_BUFFER 400
#define CURR_DIR "/home/frain8/Documents/Sisop/Modul_3/soal_shift_3/soal3"

typedef struct {
    char path[SIZE_BUFFER];
    FILE *fp;
} FileInfo;

void *moveFile(void *buf);
void _moveFile(FILE *src_fp, const char *target_path);
char *getFileName(char *file_path);
char *getExtension(const char *file_name);
char *toLower(char *str);
bool isDir(const char *file_path);
bool validFileInfo(FileInfo *file_info, char *file_path);

int main(int argc, char *argv[])
{
    chdir(CURR_DIR);
    if (argc < 3) {
        printf("Tidak ada flag/file yang akan dikelompokan\n");
        return 1;
    }
    argc -= 2;
    pthread_t tid[argc];
    FileInfo file_info[argc];
    bool success[argc];

    if (strcmp(argv[1], "-f") == 0) {
        for (int i = 0; i < argc; i++) {
            if (validFileInfo(&file_info[i], argv[i + 2])) {
                pthread_create(&tid[i], NULL, &moveFile, (void *) &file_info[i]);
                printf("File %d: Berhasil Dikategorisasikan\n", i + 1);
                success[i] = true;
            } else {
                printf("File %d: Sad, gagal :(\n", i + 1);
                success[i] = false;
            } 
        }
    } 
    else {
        printf("Flag tidak valid\n");
        return 1;
    }
    for (int i = 0; i < argc; i++) {
        if (!success[i]) continue;
        pthread_join(tid[i], NULL);
        fclose(file_info[i].fp);
    }
    return 0;
}

bool validFileInfo(FileInfo *file_info, char *file_path)
{
    strcpy(file_info->path, file_path);
    file_info->fp = fopen(file_info->path, "r");
    return (file_info->fp && !isDir(file_info->path));
}

void *moveFile(void *buf)
{
    chdir(CURR_DIR);
    char target_path[SIZE_BUFFER], filename[SIZE_BUFFER / 2], ext[SIZE_BUFFER / 2];
    FileInfo src_info = *(FileInfo *) buf;

    strcpy(filename, toLower(getFileName(src_info.path)));
    strcpy(ext, toLower(getExtension(filename)));

    mkdir(ext, 0777);
    sprintf(target_path, "%s/%s", ext, filename);
    _moveFile(src_info.fp, target_path);
}

void _moveFile(FILE *src_fp, const char *target_path)
{
    FILE *target_fp = fopen(target_path, "w+");
    char buf;

    while ((buf = fgetc(src_fp)) != EOF) {
        fputc(buf, target_fp);
    }
    fclose(target_fp);
}

char *toLower(char *str)
{
    for(int i = 0; str[i]; i++){
        if (str[i] == ' ') continue;
        str[i] = tolower(str[i]);
    }
    return str;
}

char *getExtension(const char *file_name)
{
    char temp[SIZE_BUFFER];
    strcpy(temp, file_name);

    char *ext = strchr(temp, '.');
    if (ext) return ext + 1;
    else return " ";
}

char *getFileName(char *file_path)
{
    char temp[SIZE_BUFFER];
    strcpy(temp, file_path);

    char *ret = strrchr(temp, '/');
    if (ret) return ret + 1;
    else return file_path;
}

bool isDir(const char *file_path)
{
    struct stat path_stat;
    stat(file_path, &path_stat);
    return (S_ISDIR(path_stat.st_mode));
}
