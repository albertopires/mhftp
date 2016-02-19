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

#include "./chunk.h"

/** Constructor.
  *
  */
Chunk::Chunk(int64_t size) {
    data_ = (unsigned char*)malloc(size);
    size_ = size;
    if (data_ == NULL) {
        printf("Fatal Error: Fail in chunck memory allocation.\n");
        exit(1);
    }
}

unsigned char* Chunk::getData(void) {
    return data_;
}

unsigned char* Chunk::getMD5Sum(void) {
    return md5sum_;
}

void Chunk::generateCheckSum() {
    MD5Utils md5Utils;

    md5Utils.md5Buffer(data_, size_);
    memcpy(
            reinterpret_cast<char*>(md5sum_),
            md5Utils.getDigest(),
            sizeof(md5sum_));
}

int64_t Chunk::getDataSize() {
    return size_;
}

