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

#ifndef PROTOCOL_MHPROTO_H_
#define PROTOCOL_MHPROTO_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/shm.h>

#include <iostream>

#include "../con_tcp.h"
#include "../crypt/md5Utils.h"
#include "./metadata.h"
#include "./chunkInfo.h"
#include "./chunk.h"
#include "./semaphore.h"

#define DOWNLOAD_CHUNK    1
#define DOWNLOAD_METADATA 2
#define UPLOAD_CHUNK      3
#define UPLOAD_INIT       4

#define MH_OK 0
#define MH_FILE_NOT_FOUND 1

#define MH_RETRY 10

#define NPROC   10

using std::cout;
using std::endl;

class MhProtoCommons {
 public:
     static int64_t *getPayLoadArray(
             int64_t fileSize,
             int64_t chunkSize,
             int64_t nChunks);
};

#endif  // PROTOCOL_MHPROTO_H_
