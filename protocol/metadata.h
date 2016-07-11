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

#ifndef PROTOCOL_METADATA_H_
#define PROTOCOL_METADATA_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include "../tools.h"
#include "../con_tcp.h"
#include "../crypt/md5Utils.h"
#include "./chunkInfo.h"

#define HEADER_SIZE  2103
#define HEADER_SIG   "MHFTP_METADATA"
#define SIG_HDR_SIZE 14
#define CHUNK_SIZE   1048576  // Default chunk size: 1MB

/** File Metadata
 */
class Metadata {
 private:
    unsigned char digest_[DIGEST_SIZE];
    int64_t fileSize_;
    char fileFull_[FNAME_SIZE];
    char fileMetadata_[FNAME_SIZE];
    int64_t chunkSize_;
    int64_t chunks_;
    ChunkInfo *chunkInfo_;
    unsigned char in_use_;
    MD5Utils md5Utils_;

    void StoreMetadata(const char *fileName);
    void StoreMetadata(const char *fileName, int chunkSize, int chunks);
    void StoreMetadata(
            const char *fileName,
            const char *metadataName,
            int64_t chunkSize,
            int64_t chunks);
    void writeControlData(int fdCtrl);
    void showChunkError(int64_t i);

 public:
    Metadata();
    void create(const char *fileName , int64_t chunkSize, bool save);
    void create(
            const char *fileName,
            const char *metadataName,
            int64_t chunkSize,
            bool save);
    void initControlData(void);
    void setDigest(const unsigned char *digest);
    void setFileFull(const char *fileName);
    void setChunks(int64_t chunks);
    void setChunkSize(int64_t chunkSize);
    void setFileSize(int64_t fileSize);
    const char* getFileFull(void);
    int64_t getFileSize(void);
    int64_t getChunkSize(void);
    int64_t getChunks(void);
    unsigned char getInUse();
    void setInUse(unsigned char in_use);
    const unsigned char* getDigest(void);
    const char* getFileMetadata(void);
    void LoadMetadata(void);
    void LoadMetadata(const char* fileMetadata);
    void SaveMetadata(void);
    void SaveMetadata(const char* fileMetadata);
    void DisplayControlData(const char* metaFile);
    void DisplayControlDataMD5(const char* metaFile);
    void DisplayControlData(void);
    int64_t getNextPendingChunk(void);
    void setChunkStatus(
        int64_t chunkNumber,
        ChunkStatus status,
        const unsigned char* digest);
    void UpdateControlData(void);
    void UpdateControlData(const char *metaFile);
    int64_t getChunkPayLoadSize(int64_t chunkNumber);
    void resetDownloadingChunks(const char* metaFile, int force);
    int resetAllChunks(const char* metaFile, int force);
    void CheckDataChunks(const char* metaFile, const char *fileName);
    void WriteMetadataHeader(int sd);
    void ReadMetadataHeader(int sd);
    static int64_t *getPayLoadArray(
            int64_t fileSize,
            int64_t chunkSize,
            int64_t nChunks);
    bool HasMetadataFile(const char *fileName);
    bool HasMetadataFile(const char *fileName, bool loadMetadata);
};

#endif  // PROTOCOL_METADATA_H_
