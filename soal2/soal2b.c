#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>

void *factorial(void* arg)
{
    int *data = (int *)arg;
    int k = 1;
    if (data[0] >= data[1]) {
        for (int a=data[0]; a>(data[0] - data[1]); a--) {
            k *= a;
        }
    }
    if (data[1] > data[0]) {
        for (int a=data[0]; a>=1; a--) {
            k *= a;
        }
    }
    if (data[0] == 0 || data[1] == 0) {
        k = 0;
    }
      
    int *p = (int*)malloc(sizeof(int));
    *p = k;

    pthread_exit(p);
}

void main()
{
        key_t key = 1234;
        int matrixBaru[4][6];
        int (*value)[4][6];
        int r = 4, c = 6;

        int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
        value = shmat(shmid, NULL, 0);
    
        printf("Input Matrix Baru:\n");
        for (int i=0; i<4; i++) {
            for (int j=0; j<6; j++) {
                scanf("%d", &matrixBaru[i][j]);
            }
        }
        
        int max = r * c;
        pthread_t *threads;
        threads = (pthread_t*)malloc(max * sizeof(pthread_t));

        int count = 0;
        int* data = NULL;

        for (int i=0; i<r; i++) {
            for (int j=0; j<c; j++) {
                data = (int *)malloc((max)*sizeof(int));
                data[0] = (*value)[i][j];
                data[1] = matrixBaru[i][j];
                pthread_create(&threads[count++], NULL, factorial, (void*)(data));
            }
        }

        printf("Matrix Hasil :\n");
        for (int i=0; i<max; i++) {
            void *k;
            pthread_join(threads[i], &k);

            int *p = (int *)k;
            printf("%d", *p);
            if ((i + 1) % c == 0) printf("\n");
            else printf(" ");
        }
        

        sleep(60);

        shmdt(value);
        shmctl(shmid, IPC_RMID, NULL);
}
