#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>

void main()
{
        key_t key = 1234;
        int matrixA[4][3], matrixB[3][6];
        int (*value)[4][6];

        int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
        value = shmat(shmid, NULL, 0);

        printf("Input Matrix A:\n");
        for (int i=0; i<4; i++) {
            for (int j=0; j<3; j++) {
                scanf("%d", &matrixA[i][j]);
            }
        }

        printf("Input Matrix B:\n");
        for (int i=0; i<3; i++) {
            for (int j=0; j<6; j++) {
                scanf("%d", &matrixB[i][j]);
            }
        }

        int tempRest;
        for (int i=0; i<4; i++) {
            for (int j=0; j<6; j++) {
                tempRest = 0;
                for (int k=0; k<3; k++) {
                    tempRest += matrixA[i][k] * matrixB[k][j];
                }
                (*value)[i][j] = tempRest;
            }
        }

        printf("Output Program a :\n");
        for (int i=0; i<4; i++) {
            for (int j=0; j<6; j++) {
                printf("%d", (*value)[i][j]);
                if (j < 5) printf(" ");
                else printf("\n");
            }
        }


        sleep(60);

        shmdt(value);
        shmctl(shmid, IPC_RMID, NULL);
}
