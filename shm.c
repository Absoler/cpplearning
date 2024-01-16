#include <strings.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define SHM_SIZE 0x100

int main(int argc, char *argv[]) {
    int shmid;
    key_t key;
    pid_t pid;
    char *shmaddr;
    int err;

    key = ftok("./", 623);
    shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget fails.");
        exit(shmid);
    }
    shmaddr = shmat(shmid, NULL, 0);

    // mutex part
    pthread_mutexattr_t mattr;
    pthread_mutex_t *m;
    int mutex_mid;
    pthread_mutexattr_init(&mattr);
    pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
    mutex_mid = shmget(IPC_PRIVATE, sizeof(pthread_mutex_t), 0600);
    m = (pthread_mutex_t*)shmat(mutex_mid, NULL, 0);
    pthread_mutex_init(m, &mattr);

    pid = fork();
    if (pid != 0) {
        // father writes data
        err = pthread_mutex_lock(m);

        printf("before clear %p %s\n", shmaddr, shmaddr);
        bzero(shmaddr, SHM_SIZE);

        sprintf(shmaddr, "hello ");
        for (int i = 0; i < 60000; i++);
        sprintf(shmaddr + 6, "shared");
        for (int i = 0; i < 60000; i++);
        sprintf(shmaddr + 12, " memory!");
        err = pthread_mutex_unlock(m);

    } else if (pid == 0) {
        // child reads data
        err = pthread_mutex_lock(m);
        printf("read %p: %s\n", shmaddr, shmaddr);
        err = pthread_mutex_unlock(m);

    } else {
        perror("fork fails\n");
    }

   
    shmdt(shmaddr);
    if (pid != 0) {
        shmctl(shmid, IPC_RMID, NULL);
        pthread_mutexattr_destroy(&mattr);
        pthread_mutex_destroy(m);
    }

    return 0;
}