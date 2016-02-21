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
    const char *file_desc_;
    const char *meta_file_;
    MD5Utils md5utils_;
    int sd_;          // Socket Descriptor
    int *client_sd_;  // Array of client sockets
    int numClient_;   // Number of client sockets
    int pid_[NPROC];
    int monitor_pid_;
    int kill_proc_;
    bool main_proc_;

    ChunkInfo *chunkInfo;
    Metadata metadata_;
    class Semaphore *sem_;

    void DownloadFileFromServer(
        int cd_sd,
        const char *metaFile,
        const char *localFile);
    void RequestChunk(int cd_sd , int64_t chunkNumber);
    void createError(const char *file);

 public:
    MhProtoClient(int *sd , int numClient);
    void SndCmd(int cd_sd , unsigned char cmd);
    const char *getFileName(void);
    const char *getFileDesc(void);
    void DownloadFileFromServer(const char *metaFile, const char *localFile);
    void DownloadMetadataFromServer(int smd, const char *remote_file);
    void KillClient(void);
    ~MhProtoClient(void);
};

#endif  // PROTOCOL_MHPROTO_CLIENT_H_
