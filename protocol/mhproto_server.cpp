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

#include "../protocol/mhproto_server.h"

// Private Methods
void MhProtoServer::SendChunkToClient(void) {
    int br;
    int64_t chunkNumber;
    char chunkNumberStr[7];
    Chunk *chunk;

    int64_t chsize;
    int64_t nchunks;
    int64_t fileSize;

    metadata_.ReadMetadataHeader(sd_);

    chsize = metadata_.getChunkSize();
    nchunks = metadata_.getChunks();
    fileSize = metadata_.getFileSize();

    payLoadArray_ = Metadata::getPayLoadArray(fileSize, chsize, nchunks);

    DEBUG("MhProtoServer::sendChunkToClient() - <%s>\n",
            metadata_.getFileFull());

    int fd = open(metadata_.getFileFull(), O_RDONLY);
    if (fd == -1) {
        perror("MhProtoServer::sendChunkToClient can not open file ");
        exit(1);
    }

    int waitForClient = MH_RETRY;
    while (waitForClient) {
        memset(chunkNumberStr, 0, sizeof(chunkNumberStr));
        br = so_read(sd_, (unsigned char*)chunkNumberStr, sizeof(chunkNumberStr)); // NOLINT
        if (br <= 0)
            break;
        if (br != sizeof(chunkNumberStr)) {
            printf("MhProtoServer::sendChunkToClient : Wrong chunk size");
            printf(" (%d).\n", br);
            waitForClient--;
        }
        chunkNumber = atoi(chunkNumberStr);
        // If no more chunks to send
        if (chunkNumber == -1) {
            waitForClient = 0;
            break;
        }

        off_t filePos = (chunkNumber*metadata_.getChunkSize());
        lseek(fd, filePos , SEEK_SET);
        chunk = LoadChunkFromFile(fd, payLoadArray_[chunkNumber]);
        DEBUG("MhProtoServer::sendChunkToClient : ");
        DEBUG("sending chunk number %ld - %ld bytes\n",
                i64toLong(chunkNumber), i64toLong(payLoadArray_[chunkNumber]));
        write(sd_, chunk->getMD5Sum(), DIGEST_SIZE);
        int64_t dataSize = chunk->getDataSize();
        write(sd_, &dataSize, sizeof(int64_t));
        write(sd_, chunk->getData(), dataSize);

        if (chunk != NULL) delete chunk;
    }

    close(fd);
}

void MhProtoServer::SendMetadataToClient(void) {
    int16_t path_size;
    int16_t response;
    char file_path[2048];

    DEBUG("MhProtoServer::SendMetadataToClient()\n");

    so_read(sd_, reinterpret_cast<unsigned char*>(&path_size), sizeof(int16_t)); // NOLINT
    DEBUG("Path Size : %d\n", path_size);

    memset(file_path, 0, sizeof(file_path));
    so_read(sd_,
            reinterpret_cast<unsigned char*>(&file_path[0]),
            path_size);
    DEBUG("File : <%s>\n" , file_path);

    if (fileExists(file_path)) {
        response = MH_OK;
    } else {
        response = MH_FILE_NOT_FOUND;
        write(sd_, &response, sizeof(int16_t));
        return;
    }

    write(sd_, &response, sizeof(int16_t));
    DEBUG("SendMetadataToClient->response : %d\n", response);

    Metadata metadata;
    metadata.create(file_path, NULL, CHUNK_SIZE, false);
    metadata.WriteMetadataHeader(sd_);
}

Chunk *MhProtoServer::LoadChunkFromFile(int fd, int64_t chunkSize) {
    Chunk *chunk;

    chunk = new Chunk(chunkSize);
    unsigned char *data = chunk->getData();
    read(fd, data, chunkSize);
    chunk->generateCheckSum();

    return chunk;
}

void MhProtoServer::ReceiveChunkFromClient(void) {
    DEBUG("MhProtoServer::ReceiveChunkFromClient()\n");

    unsigned char *buffer = static_cast<unsigned char*>(malloc(MAX_CHUNK_SIZE));
    int fd;
    int64_t chunkSize;
    int64_t chunkNumber = 0;

    fd = open(local_file_, O_WRONLY);

    while (chunkNumber != -1) {
        int br = so_read(sd_, &chunkNumber ,  sizeof(int64_t));
        if (br <= 0) chunkNumber = -1;
        if (chunkNumber == -1) break;
        so_read(sd_, &chunkSize, sizeof(int64_t));
        so_read(sd_, buffer,     chunkSize);

        off_t filePos = (chunkNumber*chunkSizeStd_);
        // hex_dump(buffer, 200);
        lseek(fd, filePos, SEEK_SET);
        write(fd, buffer, chunkSize);
    }

    close(fd);
    free(buffer);
}

void MhProtoServer::InitUpload(void) {
    char local_file[2048];

    DEBUG("MhProtoServer::InitUpload()\n");

    int16_t path_size;
    int64_t file_size;
    memset(local_file, 0, sizeof(local_file));

    so_read(sd_, &path_size,     sizeof(int16_t));
    so_read(sd_, local_file,     path_size);
    so_read(sd_, &chunkSizeStd_, sizeof(int64_t));
    so_read(sd_, &file_size,     sizeof(int64_t));

    so_read(sd_, upload_digest_, DIGEST_SIZE);
    CopyString(local_file_, AbsoluteFile(local_file));

    DEBUG("upload.Path Size : %d\n"  , path_size);
    DEBUG("upload.File Name : <%s>\n", local_file);
    DEBUG("upload.File_Name : <%s>\n", local_file_);
    DEBUG("upload.Chunk Size: %ld\n" , i64toLong(chunkSizeStd_));
    DEBUG("upload.File Size : %ld\n" , i64toLong(file_size));
    hex_dump(upload_digest_, DIGEST_SIZE);

    // create file
    int fd = open(local_file_, O_WRONLY | O_CREAT, 0000644);
    close(fd);
}

// Public Methods
MhProtoServer::MhProtoServer(int sd) {
    sd_ = sd;
}

void MhProtoServer::RcvCmd(void) {
    int br;
    unsigned char cmd;
    br = so_read(sd_, &cmd, 1);
    if ( br <= 0 ) {
        exit(0);
    }
    DEBUG("server:cmd : <%x>\n" , cmd);
    switch (cmd) {
        case DOWNLOAD_CHUNK : SendChunkToClient();
            break;
        case DOWNLOAD_METADATA : SendMetadataToClient();
            break;
        case UPLOAD_CHUNK: ReceiveChunkFromClient();
            break;
        case UPLOAD_INIT: InitUpload();
            break;
        default:
            break;
    }
}

const char *MhProtoServer::getFileName(void) {
    return file_name_;
}

const char *MhProtoServer::getFileDesc(void) {
    return file_desc_;
}

MhProtoServer::~MhProtoServer(void) {
    if (file_desc_ != NULL)
        free(const_cast<char*>(file_desc_));
}
