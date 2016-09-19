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

#ifndef PROTOCOL_MHPROTO_CLIENT_H_
#define PROTOCOL_MHPROTO_CLIENT_H_

#include "./mhproto.h"

class MhProtoClient {
 private:
    const char *file_name_;
    const char *meta_file_;
    MD5Utils md5utils_;
    int sd_;          // Socket Descriptor
    int *client_sd_;  // Array of client sockets
    int numClient_;   // Number of client sockets
    int pid_[NPROC];
    int monitor_pid_;
    int *kill_proc_;
    int shmid_;
    int shm_key_;
    bool main_proc_;
    bool verbose_;
    int64_t *payLoadArray_;
    int64_t chunk_size_;

    ChunkInfo *chunkInfo;
    Metadata metadata_;
    class Semaphore *sem_;

    void DownloadFileFromServer(
        int cd_sd,
        const char *metaFile,
        const char *localFile);
    void RequestChunk(int cd_sd , int64_t chunkNumber);
    void UploadFileToServer(int cd_sd, const char *localFile);
    void createError(const char *file);
    void CreateSharedMemory(void);
    void DestroySharedMemory(void);
    void ClearResources(void);

 public:
    MhProtoClient(int *sd , int numClient, int64_t chunk_size, bool verbose);
    void SndCmd(int cd_sd , unsigned char cmd);
    const char *getFileName(void);
    void DownloadFileFromServer(const char *metaFile, const char *localFile);
    void DownloadMetadataFromServer(
            int smd,
            const char *remote_file,
            int64_t chunk_size);
    void UploadFileToServer(const char *localFile);
    void InitUploadOnServer(int smd, const char *local_file, bool create);
    void KillClient(void);
    int getKillProc(void);
    ~MhProtoClient(void);
};

#endif  // PROTOCOL_MHPROTO_CLIENT_H_
