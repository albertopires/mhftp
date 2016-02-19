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

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "./tools.h"
#include "./protocol/metadata.h"

void createMetaData(char *fileName , int *chunk_size_ptr);
void showMetaData(const char* fileMetaData);

int main(int argc , char *argv[]) {
    printf("initFile %s\n\n", VER);

    if (argc < 2) {
        printf("initFile <create/show> <file>\n");
        printf(" or\n");
        printf("initFile <create> <file> <chunk_size>\n");
        exit(1);
    }

    if (strcmp("create", argv[1]) == 0) {
        int chunk_size = (argc == 4 ? atoi(argv[3]) : 0);
        createMetaData(argv[2], chunk_size != 0 ? &chunk_size : NULL);
    }

    if (strcmp("show", argv[1]) == 0) {
        showMetaData(argv[2]);
    }

    exit(0);
}

void createMetaData(char *fileName , int *chunk_size_ptr) {
    int chunk_size = CHUNK_SIZE;
    Metadata metaData;

    if (chunk_size_ptr != NULL) {
        chunk_size = *chunk_size_ptr;
    }

    metaData.create(fileName, chunk_size, true);
}

void showMetaData(const char* fileMetaData) {
    MD5Utils md5utils;
    Metadata metaData;

    printf("File Metadata: <%s>\n\n" , fileMetaData);

    metaData.LoadMetadata(fileMetaData);

    printf("File Name  : <%s>\n"  , metaData.getFileFull());
    printf("File size  : <%ld>\n" , i64toLong(metaData.getFileSize()));
    printf("Chunk Size : %ld\n"   , i64toLong(metaData.getChunkSize()));
    printf("Chunks     : %ld\n"   , i64toLong(metaData.getChunks()));
    printf("MD5        : %s\n" , md5utils.digestToStr(metaData.getDigest()));
}

