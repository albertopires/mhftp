/*
 * Copyright (C) 2016 Alberto Pires de Oliveira Neto
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "./semaphore.h"

/** init: 1 create a new semaphore, 0 use an existing one.
  */
Semaphore::Semaphore(int init) {
    DEBUG("Semaphore::Constructor\n");

    error_   = 0;
    init_id_ = 1237;
    unsigned int seed = getpid();
    do {
        if (init) sem_id_ = semget(init_id_ , 1 , 0666 | IPC_CREAT | IPC_EXCL);
        else
            sem_id_ = semget(init_id_ , 1 , 0666);
        if (sem_id_ == -1) init_id_ = rand_r(&seed);
    } while ((errno == EEXIST) && (sem_id_ == -1));
    error_ = 0;
    if (sem_id_ == -1) {
        error_ = 1;
        perror("Semaphore::Constructor ");
        return;
    }

    if (init) {
        if (!set_semvalue()) {
            printf("Failed to initialize semaphore\n");
            error_ = 1;
        }
    }
}

Semaphore::~Semaphore(void) {
    DEBUG("Semaphore::Destructor\n");
    del_semvalue();
}

int Semaphore::set_semvalue(void) {
    union semun sem_union;

    sem_union.val = 1;
    if (semctl(sem_id_ , 0 , SETVAL , sem_union) == -1) return(0);
    return(1);
}

void Semaphore::del_semvalue(void) {
    union semun sem_union;

    if (semctl(sem_id_ , 0 , IPC_RMID , sem_union) == -1)
        fprintf(stderr , "Failed to delete semaphore\n");
}

int Semaphore::semaphore_get_lock(void) {
    struct sembuf sem_b;

    sem_b.sem_num = 0;
    sem_b.sem_op  = -1; /* P */
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id_ , &sem_b , 1) == -1) {
        fprintf(stderr , "semaphore_p failed\n");
        perror("Semaphore::semaphore_p ");
        return(0);
    }
    return(1);
}

int Semaphore::semaphore_release_lock(void) {
    struct sembuf sem_b;

    sem_b.sem_num = 0;
    sem_b.sem_op  = 1; /* V */
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id_ , &sem_b , 1) == -1) {
        fprintf(stderr , "semaphore_v failed\n");
        perror("Semaphore::semaphore_v ");
        return(0);
    }
    return(1);
}

int Semaphore::serror(void) {
    return(error_);
}
