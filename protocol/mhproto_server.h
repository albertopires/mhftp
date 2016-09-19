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

#ifndef PROTOCOL_MHPROTO_SERVER_H_
#define PROTOCOL_MHPROTO_SERVER_H_

#include "./mhproto.h"

class MhProtoServer {
 private:
    unsigned char upload_digest_[DIGEST_SIZE];
    const char *file_name_;
    const char *file_desc_;
    const char *meta_file_;
    char local_file_[2048];
    MD5Utils md5utils_;
    int sd_;  // Socket Descriptor
    int64_t  *payLoadArray_;
    int64_t  chunk_size_;
    Metadata metadata_;

    void SendChunkToClient(void);
    void SendMetadataToClient(void);
    void ReceiveChunkFromClient(void);
    void InitUpload(void);
    Chunk *LoadChunkFromFile(int fd , int64_t chunkSize);

 public:
    explicit MhProtoServer(int sd, int64_t chunk_size);
    void RcvCmd(void);
    const char *getFileName(void);
    const char *getFileDesc(void);
    static MhProtoServer *GetServerInstance(int sd);
    ~MhProtoServer(void);
};

#endif  // PROTOCOL_MHPROTO_SERVER_H_
