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

#ifndef PROTOCOL_CHUNK_H_
#define PROTOCOL_CHUNK_H_

#include <stdio.h>
#include <stdlib.h>
#include "../tools.h"
#include "../crypt/md5Utils.h"

/** Basic unit of data to be transffered.
 */
class Chunk {
 private:
    unsigned char md5sum_[16];
    int64_t size_;
    unsigned char *data_;

 public:
    explicit Chunk(int64_t size);
    unsigned char *getData(void);
    unsigned char *getMD5Sum(void);
    void generateCheckSum(void);
    int64_t getDataSize(void);
    ~Chunk(void);
};

#endif  // PROTOCOL_CHUNK_H_
