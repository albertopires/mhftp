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

#ifndef PROTOCOL_CHUNKINFO_H_
#define PROTOCOL_CHUNKINFO_H_

#include <stdio.h>
#include <stdlib.h>

enum ChunkStatus { CH_OK, CH_PENDING, CH_ERROR , CH_DOWNLOAD };

/** Basic unit of data to be transffered.
 */
struct ChunkInfo {
    unsigned char md5sum[16];
    int64_t size;
    ChunkStatus status;
};

#endif  // PROTOCOL_CHUNKINFO_H_
