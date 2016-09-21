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

#ifndef PROTOCOL_SEMAPHORE_H_
#define PROTOCOL_SEMAPHORE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <stdint.h>
#include "../tools.h"

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
/** union semun is defined by including <sys/sem.h> */
#else
/** according to X/OPEN we have to define it ourselves */
union semun {
    int val;                    /**< value for SETVAL */
    struct semid_ds *buf;       /**< buffer for IPC_STAT, IPC_SET */
    uint16_t *array;            /**< array for GETALL, SETALL */
    struct seminfo *__buf;      /**< buffer for IPC_INFO */
};
#endif  // __GNU_LIBRARY__

/** Handle semaphores.
 */
class Semaphore {
 private:
    int   error_;
    int   sem_id_;
    key_t init_id_;

    int  set_semvalue(void);

 public:
    explicit Semaphore(int init);
    ~Semaphore(void);
    void del_semvalue(void);
    int semaphore_get_lock(void);
    int semaphore_release_lock(void);
    int serror(void);
};

#endif  // PROTOCOL_SEMAPHORE_H_
