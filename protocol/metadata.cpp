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

#include "./metadata.h"

/** Constructor.
  *
  */
Metadata::Metadata() {
    DEBUG("Metadata::Constructor()\n");
}

void Metadata::setFileFull(const char *fileName) {
    snprintf(fileFull_ , strlen(fileName)+1 , "%s", fileName);
}

const char* Metadata::getFileFull(void) {
    return fileFull_;
}

void Metadata::setChunkSize(int64_t chunkSize) {
    chunkSize_ = chunkSize;
}

int64_t Metadata::getChunkSize(void) {
    return chunkSize_;
}

void Metadata::setChunks(int64_t chunks) {
    chunks_ = chunks;
}

int64_t Metadata::getChunks(void) {
    return chunks_;
}

unsigned char Metadata::getInUse() {
    return in_use_;
}

void Metadata::setInUse(unsigned char in_use) {
    in_use_ = in_use;
}

int64_t Metadata::getFileSize(void) {
    return fileSize_;
}

void Metadata::setFileSize(int64_t fileSize) {
    fileSize_ = fileSize;
}

void Metadata::setDigest(const unsigned char *digest) {
    memcpy(digest_, digest, sizeof(digest_));
}

const unsigned char* Metadata::getDigest(void) {
    return digest_;
}

void Metadata::create(const char *fileName, int64_t chunkSize, bool save) {
    create(fileName, NULL, chunkSize, save);
}

void Metadata::create(
        const char *fileName,
        const char *metadataName,
        int64_t chunkSize,
        bool save) {
    MD5Utils md5utils;
    struct stat fileInfo;
    int fd;
    int64_t chunks;

    fd = open(fileName , O_RDONLY);
    if (fd == -1) {
        perror("Download File ");
        exit(1);
    }

    fstat(fd, &fileInfo);
    fileSize_ = fileInfo.st_size;
    printf("File Size  : %ld\n" , i64toLong(fileSize_));

    chunks = fileSize_ / chunkSize;
    if ((fileSize_ % chunkSize) != 0) {
        chunks++;
    }

    chunkSize_ = chunkSize;
    setChunkSize(chunkSize);
    chunks_ = chunks;
    snprintf(fileFull_ , strlen(fileName)+1 , "%s", fileName);

    printf("Chunk Size : %ld\n" , i64toLong(chunkSize));
    printf("Chunks     : %ld\n" , i64toLong(chunks));

    md5utils.md5File(fd);
    memcpy(reinterpret_cast<char*>(digest_),
            reinterpret_cast<const char*>(md5utils.getDigest()), DIGEST_SIZE);
    printf("MD5 : %s\n" , md5utils.digestToStr(digest_));

    close(fd);
    in_use_ = 0;
    if (save)
        StoreMetadata(fileName, metadataName, chunkSize, chunks);
}

void Metadata::StoreMetadata(const char *fileName) {
    StoreMetadata(fileName, NULL, chunkSize_, chunks_);
}

void Metadata::StoreMetadata(const char *fileName, int chunkSize, int chunks) {
    StoreMetadata(fileName, NULL, chunkSize, chunks);
}

void Metadata::StoreMetadata(
        const char *fileName,
        const char *metadataName,
        int64_t chunkSize,
        int64_t chunks) {
    char *fileAbsolute;
    char fileMetadata[1024];
    char fileFull[2048];
    struct stat fileStat;

    memset(fileMetadata , 0 , sizeof(fileMetadata));
    memset(fileFull     , 0 , sizeof(fileFull));
    memset(&fileStat    , 0 , sizeof(fileStat));

    for (int i = strlen(fileName) ; i > 0 ; i--) {
        if (fileName[i] == '/') {
            fileAbsolute = strdup(&fileName[i+1]);
            break;
        }
    }

    StrCat(fileFull , fileName);

    if (metadataName == NULL) {
        StrCat(fileMetadata , fileAbsolute);
        StrCat(fileMetadata , ".mdata");
    } else {
        CopyString(fileMetadata, metadataName);
    }

    printf("Storing metadata file...\n");
    printf("File Metadata: <%s>\n" , fileMetadata);
    printf("File Name    : <%s>\n" , fileFull);

    int fd = open(fileMetadata , O_WRONLY | O_CREAT, 0000644);
    if (fd == -1) {
        perror("Meta Data File ");
        exit(1);
    }

    stat(fileFull, &fileStat);
    fileSize_ = fileStat.st_size;

    //  Write metadata header to file
    write(fd , fileFull   , sizeof(fileFull));    // 2048
    write(fd , &fileSize_ , sizeof(fileSize_));   // 8
    write(fd , &chunkSize , sizeof(chunkSize));   // 8
    write(fd , &chunks  , sizeof(chunks));        // 8
    write(fd , digest_  , DIGEST_SIZE);           // 16
    write(fd , &in_use_ , 1);                     // 1 - Total = 2081
    // Don't forget to see if HEADER_SIZE == 2089
    writeControlData(fd);
    close(fd);
}

void Metadata::showChunkError(int64_t i) {
    printf("Chunk %ld - %s\n",
            i64toLong(i), md5Utils_.digestToStr(chunkInfo_[i].md5sum));
}

//  Public Methods
//
void Metadata::LoadMetadata(const char* fileMetadata) {
    chunkSize_ = -1;
    chunks_ = -1;
    memset(fileFull_ , 0 , sizeof(fileFull_));

    int fd = open(fileMetadata , O_RDONLY);
    if (fd == -1) {
        perror("Meta Data File ");
        exit(1);
    }

    read(fd , fileFull_   , sizeof(fileFull_));
    read(fd , &fileSize_  , sizeof(fileSize_));
    read(fd , &chunkSize_ , sizeof(chunkSize_));
    read(fd , &chunks_    , sizeof(chunks_));
    read(fd , digest_     , 16);
    read(fd , &in_use_    , 1);
    // Read Chunk Info

    int chunkArraySize = sizeof(struct ChunkInfo)*getChunks();
    chunkInfo_ = static_cast<ChunkInfo*>(malloc(chunkArraySize));
    memset(chunkInfo_, 0, chunkArraySize);
    read(fd, chunkInfo_, chunkArraySize);
    close(fd);
}

void Metadata::saveMetadata(const char* fileMetadata) {
    int fd = open(fileMetadata , O_WRONLY);
    if (fd == -1) {
        perror("Meta Data File ");
        exit(1);
    }

    write(fd , fileFull_   , sizeof(fileFull_));
    write(fd , &fileSize_  , sizeof(fileSize_));
    write(fd , &chunkSize_ , sizeof(chunkSize_));
    write(fd , &chunks_    , sizeof(chunks_));
    write(fd , digest_     , 16);
    write(fd , &in_use_    , 1);

    int64_t chunkArraySize = sizeof(struct ChunkInfo)*getChunks();
    write(fd, chunkInfo_, chunkArraySize);
    close(fd);
}

void Metadata::writeControlData(int fdCtrl) {
    int64_t chunkArraySize = sizeof(struct ChunkInfo)*getChunks();

    initControlData();

    write(fdCtrl, chunkInfo_, chunkArraySize);
}

void Metadata::initControlData(void) {
    int64_t nChunks = getChunks();
    int64_t chunkArraySize = sizeof(struct ChunkInfo)*nChunks;
    chunkInfo_ = static_cast<ChunkInfo*>(malloc(chunkArraySize));
    memset(chunkInfo_, 0, chunkArraySize);

    for (int i = 0 ; i < nChunks ; i++) {
        chunkInfo_[i].status = CH_PENDING;
        int64_t remain = fileSize_ - (chunkSize_ * i);
        if (remain >= chunkSize_) {
            chunkInfo_[i].size = chunkSize_;
        } else {
            chunkInfo_[i].size = remain;
        }
    }
}

void Metadata::updateControlData(const char *metaFile) {
    int64_t nChunks = getChunks();

    int64_t chunkArraySize = sizeof(struct ChunkInfo)*nChunks;
    int fdCtrl = open(metaFile, O_WRONLY);
    if (fdCtrl == -1) {
        perror("Update ChunkInfo ");
        exit(1);
    }

    lseek(fdCtrl, HEADER_SIZE, SEEK_SET);
    write(fdCtrl, chunkInfo_, chunkArraySize);

    close(fdCtrl);
}

void Metadata::DisplayControlData(const char* metaFile) {
    LoadMetadata(metaFile);

    int64_t nChunks = getChunks();
    printf("Number of chunks : %ld - ", i64toLong(nChunks));
    printf("Status : %s\n" , getInUse() == 0 ? "IDLE" : "DOWNLOADING");
    for (int i = 0 ; i < nChunks ; i++) {
        if (chunkInfo_[i].status == CH_OK)
            PrintColor("O", DC_GREEN);
        if (chunkInfo_[i].status == CH_PENDING)
            PrintColor("P", DC_WHITE);
        if (chunkInfo_[i].status == CH_ERROR)
            PrintColor("E", LC_RED);
        if (chunkInfo_[i].status == CH_DOWNLOAD)
            PrintColor("D", LC_CYAN);
    }
}

void Metadata::DisplayControlDataMD5(const char* metaFile) {
    LoadMetadata(metaFile);

    int64_t nChunks = getChunks();
    printf("Number of chunks : %ld - ", i64toLong(nChunks));
    printf("Status : %s\n" , getInUse() == 0 ? "IDLE" : "DOWNLOADING");
    for (int i = 0 ; i < nChunks ; i++) {
        if (chunkInfo_[i].status == CH_OK)
            PrintColor("O - ", DC_GREEN, 0);
        if (chunkInfo_[i].status == CH_PENDING)
            PrintColor("P - ", DC_WHITE, 0);
        if (chunkInfo_[i].status == CH_ERROR)
            PrintColor("E - ", LC_RED, 0);
        if (chunkInfo_[i].status == CH_DOWNLOAD)
            PrintColor("D - ", LC_CYAN, 0);
        printf("%s", md5Utils_.digestToStr(chunkInfo_[i].md5sum));
        printf(" %ld", (long)chunkInfo_[i].size); // NOLINT
        cout << C_RESET << endl;
    }
}

void Metadata::DisplayControlData(void) {
    int64_t nChunks = getChunks();
    printf("Number of chunks : %ld\n", i64toLong(nChunks));
    for (int i = 0 ; i < nChunks ; i++) {
        if (chunkInfo_[i].status == CH_OK)
            PrintColor("O", DC_GREEN);
        if (chunkInfo_[i].status == CH_PENDING)
            PrintColor("P", DC_WHITE);
        if (chunkInfo_[i].status == CH_ERROR)
            PrintColor("E", LC_RED);
        if (chunkInfo_[i].status == CH_DOWNLOAD)
            PrintColor("D", LC_CYAN);
    }
}

int64_t Metadata::getNextPendingChunk(void) {
    for (int i = 0 ; i < chunks_ ; i++) {
        if ((chunkInfo_[i].status != CH_OK)
                && (chunkInfo_[i].status != CH_DOWNLOAD)) {
            return i;
        }
    }
    return -1;
}

void Metadata::setChunkStatus(
        int64_t chunkNumber,
                ChunkStatus status,
                const unsigned char* digest) {
    chunkInfo_[chunkNumber].status = status;
    if (digest != NULL) {
        memcpy(chunkInfo_[chunkNumber].md5sum, digest, DIGEST_SIZE);
    }
}

int64_t Metadata::getChunkPayLoadSize(int64_t chunkNumber) {
    return chunkInfo_[chunkNumber].size;
}

/** Reset chunks with Downloading status. It may happen if the client aborts
  */
void Metadata::resetDownloadingChunks(const char* metaFile, int force) {
    LoadMetadata(metaFile);
    if (in_use_ != 0 && force == 0) {
        printf("mhclient is still running or was aborted.\n");
        printf("If the client is not running try ");
        printf("--recover/-r --force/-f\n");
        exit(1);
    }
    in_use_ = 0;
    for (int i = 0 ; i < chunks_ ; i++) {
        if (chunkInfo_[i].status == CH_DOWNLOAD ||
                chunkInfo_[i].status == CH_ERROR) {
            chunkInfo_[i].status = CH_PENDING;
        }
    }
    saveMetadata(metaFile);
}

/** Reset all chunk status to CH_PENDING. 
  */
int Metadata::resetAllChunks(const char* metaFile, int force) {
    LoadMetadata(metaFile);
    if (force == 0 && in_use_ != 0) return -1;
    in_use_ = 0;
    for (int i = 0 ; i < chunks_ ; i++) {
        chunkInfo_[i].status = CH_PENDING;
    }
    saveMetadata(metaFile);

    return 0;
}

void Metadata::checkDataChunks(const char* metaFile, const char *fileName) {
    LoadMetadata(metaFile);
    cout << "Check Data" << endl;
    if (in_use_ != 0) {
        printf("Metadata file is in use, status will not be updated.\n");
    }

    int fd = open(fileName, O_RDONLY);
    if (fd == -1) {
        perror("Check Data failed to open file.\n");
        exit(1);
    }

    unsigned char *buffer = static_cast<unsigned char*>(malloc(chunkSize_));

    for (int i = 0 ; i < chunks_ ; i++) {
        int cs = chunkInfo_[i].size;
        if (chunkInfo_[i].status == CH_OK) {
            int br = read(fd, buffer, cs);
            md5Utils_.md5Buffer(buffer, br);
            printf("%s\n", md5Utils_.getDigestStr());
            if (memcmp(md5Utils_.getDigest(),chunkInfo_[i].md5sum, 16) != 0) { // NOLINT
                chunkInfo_[i].status = CH_ERROR;
                printf("Corrupted chunks:\n");
                showChunkError(i);
            }
        } else {
            lseek(fd, cs, SEEK_CUR);
        }
    }

    free(buffer);
    close(fd);
    if (in_use_ == 0) {
        cout << "updating metadata" << endl;
        saveMetadata(metaFile);
    }
}

void Metadata::writeMetadataHeader(int sd) {
    write(sd, digest_    , DIGEST_SIZE);
    write(sd, &fileSize_ , sizeof(fileSize_));
    write(sd, fileFull_  , FNAME_SIZE);
    write(sd, &chunkSize_, sizeof(chunkSize_));
    write(sd, &chunks_   , sizeof(chunks_));
}

void Metadata::readMetadataHeader(int sd) {
    so_read(sd, digest_    , DIGEST_SIZE);
    so_read(sd, &fileSize_ , sizeof(fileSize_));
    so_read(sd, fileFull_  , FNAME_SIZE);
    so_read(sd, &chunkSize_, sizeof(chunkSize_));
    so_read(sd, &chunks_   , sizeof(chunks_));
}
