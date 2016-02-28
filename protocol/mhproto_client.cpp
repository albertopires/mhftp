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

#include "./mhproto_client.h"

// Private Methods
void MhProtoClient::DownloadFileFromServer(
        int cd_sd,
        const char *metaFile,
        const char *localFile) {
    unsigned char *rcv_buffer = NULL;
    unsigned char rcv_md5sum[16];
    int64_t rcv_dataSize;
    MD5Utils md5Utils;
    rcv_buffer = static_cast<unsigned char*>(malloc(MAX_CHUNK_SIZE));

    SndCmd(cd_sd, DOWNLOAD_CHUNK);

    metadata_.LoadMetadata(metaFile);
    metadata_.writeMetadataHeader(cd_sd);

    int fd_download = open(localFile , O_WRONLY | O_CREAT , 0000644);
    if (fd_download == -1) {
        perror("open download file ");
        exit(1);
    }

    int64_t chunkNumber = -1;
    do {
        DEBUG("Before lock : %ld - \033[1m\033[32m%d\033[0m\n",
                i64toLong(chunkNumber),
                getpid());
        sem_->semaphore_get_lock();
        metadata_.LoadMetadata(metaFile);
        chunkNumber = metadata_.getNextPendingChunk();
        if (chunkNumber == -1) {
            sem_->semaphore_release_lock();
            break;
        }
        metadata_.setChunkStatus(chunkNumber, CH_DOWNLOAD, NULL);
        metadata_.updateControlData(metaFile);
        DEBUG("Next Pending Chunk : %ld\n", i64toLong(chunkNumber));
        sem_->semaphore_release_lock();

        // Receive chunk
        RequestChunk(cd_sd, chunkNumber);

        DEBUG("Next Pending Chunk-v : %ld\n", i64toLong(chunkNumber));
        so_read(cd_sd, rcv_md5sum, DIGEST_SIZE);
        DEBUG("MD5       : <%s>\n", md5Utils.digestToStr(rcv_md5sum));

        so_read(cd_sd, (unsigned char*)&rcv_dataSize, sizeof(int64_t));
        DEBUG("Data Size : %ld\n", i64toLong(rcv_dataSize));

        so_read(cd_sd, rcv_buffer, rcv_dataSize);
        // hex_dump((char*)rcv_buffer,200);
        off_t filePos = (chunkNumber*metadata_.getChunkSize());
        lseek(fd_download, filePos, SEEK_SET);
        write(fd_download, rcv_buffer, rcv_dataSize);

        sem_->semaphore_get_lock();
        metadata_.LoadMetadata(metaFile);
        metadata_.setChunkStatus(chunkNumber, CH_OK, rcv_md5sum);
        metadata_.updateControlData(metaFile);
        sem_->semaphore_release_lock();
    } while (chunkNumber != -1 && *kill_proc_ == 0);
    close(fd_download);
    if (rcv_buffer != NULL) free(rcv_buffer);
    DEBUG("Process: no more chunks to download.\n");
}

void MhProtoClient::RequestChunk(int cd_sd , int64_t chunkNumber) {
    char chunkNumberStr[7];

    memset(chunkNumberStr, 0, sizeof(chunkNumberStr));

    snprintf(chunkNumberStr, sizeof(chunkNumberStr), "%ld",
        i64toLong(chunkNumber));
    write(cd_sd, chunkNumberStr, sizeof(chunkNumberStr));
}

/** Corrupt file for test purposes.
 */
void MhProtoClient::createError(const char *file) {
    int fd = open(file, O_WRONLY);

    if (fd == -1) {
        perror("Fail to open file for test.");
        exit(1);
    }

    write(fd, "Error", strlen("Error"));

    close(fd);
}

void MhProtoClient::CreateSharedMemory(void) {
    unsigned int seed = getpid();
    shm_key_ = 1234;
    int shm_size = sizeof(int);
    do {
        shmid_ = shmget(shm_key_, shm_size, 0611 | IPC_CREAT | IPC_EXCL);
        if (shmid_ == -1) shm_key_ = rand_r(&seed);
    } while ((errno == EEXIST) && (shmid_ == -1));
    kill_proc_ = static_cast<int*>(shmat(shmid_, NULL, 0));
    if (kill_proc_ == reinterpret_cast<void*>(-1)) {
        printf("MhProtoClient::createShareMemory - shmat failed\n");
        exit(1);
    }
}

void MhProtoClient::DestroySharedMemory(void) {
    if (shmdt(kill_proc_) == -1) {
        printf("DestroySharedMemory - shmdt failed.\n");
    }
    if (shmctl(shmid_, IPC_RMID, 0) == -1) {
        printf("DestroySharedMemory - shmctl(IPC_RMID) failed.\n");
    }
}

// Public Methods
MhProtoClient::MhProtoClient(int *sd , int numClient, bool verbose) {
    // Constructor
    sem_ = new Semaphore(1);
    CreateSharedMemory();
    client_sd_ = sd;
    numClient_ = numClient;
    *kill_proc_ = 0;
    main_proc_ = true;
    verbose_   = verbose;
    if (is_debug_enabled()) verbose_ = false;
}

void MhProtoClient::SndCmd(int cd_sd, unsigned char cmd) {
    write(cd_sd, &cmd, 1);
}

const char *MhProtoClient::getFileName(void) {
    return file_name_;
}

//  Public
void MhProtoClient::DownloadFileFromServer(
        const char *metaFile,
        const char *localFile) {
    meta_file_ = metaFile;

    metadata_.LoadMetadata(metaFile);
    metadata_.setInUse(1);
    metadata_.saveMetadata(metaFile);

    printf("Number of clients : %d\n" , numClient_);
    if (numClient_ < 0 || numClient_ > NPROC) exit(1);
    for (int i = 0 ; i < numClient_ ; i++) {
        pid_[i] = fork();
        if (pid_[i] == -1) {
            cout << "Could not create process." << endl;
            exit(1);
        }
        if (pid_[i] == 0) {
            signal(SIGINT, SIG_IGN);
            DEBUG("Create download fork()\n");
            main_proc_ = false;
            DownloadFileFromServer(client_sd_[i], metaFile, localFile);
            close(client_sd_[i]);
            exit(0);
        }
    }

    if (verbose_) {
        DEBUG("Verbose\n");
        monitor_pid_ = fork();
        if (monitor_pid_== 0) {
            signal(SIGINT, SIG_IGN);
            char in_use = 1;
            while (in_use == 1 && *kill_proc_ == 0) {
                in_use = metadata_.getInUse();
                printf("%s", ANSI_SC);
                metadata_.DisplayControlData();
                metadata_.LoadMetadata(metaFile);
                printf("\n");
                if (in_use == 1 && *kill_proc_ == 0) printf("%s", ANSI_RC);
                sleep(1);
            }
            printf("\n");
            exit(0);
        }
    }

    for (int i = 0 ; i < numClient_ ; i++) {
        DEBUG("Wait for pid : %d\n", pid_[i]);
        waitpid(pid_[i], NULL, 0);
    }
    DEBUG("After Wait for pid......\n");

    metadata_.LoadMetadata(metaFile);
    metadata_.setInUse(0);
    metadata_.saveMetadata(metaFile);

    if (verbose_) waitpid(monitor_pid_, NULL, 0);

    MD5Utils md5download;
    // createError(localFile);
    md5download.md5File(localFile);

    cout << "End of Transfer:" << endl;
    cout << "File MD5 : ";
    cout << md5utils_.digestToStr(metadata_.getDigest()) << endl;

    if (memcmp(metadata_.getDigest(), md5download.getDigest(), 16) != 0) {
        cout << "Downloaded file is corrupted." << endl;
    }
}

//  public
void MhProtoClient::DownloadMetadataFromServer(int smd,
        const char *remote_file) {
    int16_t response;
    char abs_file[2048];

    DEBUG("MhProtoClient::DownloadMetadataFromServer()\n");
    SndCmd(smd, DOWNLOAD_METADATA);

    int16_t path_size = strlen(remote_file);
    write(smd, &path_size, sizeof(int16_t));
    write(smd, remote_file, path_size);

    so_read(smd, &response, sizeof(int16_t));
    DEBUG("DownloadMetadataFromServer->response : %d\n", response);
    if (response == MH_FILE_NOT_FOUND) {
        printf("Remote file not found.\n");
        if (sem_ != NULL) delete sem_;
        exit(1);
    }

    CopyString(abs_file, AbsoluteFile(remote_file));
    StrCat(abs_file, ".mdata");

    // Metadata::saveMetadata only updates, so create "touch" metadata
    // file first.
    int fd = open(abs_file , O_WRONLY | O_CREAT, 0000644);
    close(fd);

    Metadata metadata;
    metadata.readMetadataHeader(smd);
    metadata.initControlData();
    metadata.saveMetadata(abs_file);
}

void MhProtoClient::KillClient(void) {
    if (main_proc_) {
        (*kill_proc_)++;

        if (*kill_proc_ == 1) {
            DEBUG("Wait for %d processes to finish.\n", numClient_);
            for (int i = 0 ; i < numClient_ ; i++) {
                DEBUG("Wait for pid(k) : %d\n", pid_[i]);
                waitpid(pid_[i], NULL, 0);
            }
            if (verbose_) {
                DEBUG("Wait pid monitor\n");
                waitpid(monitor_pid_, NULL, 0);
            }
        }

        if (*kill_proc_ > 1) {
            if (verbose_) kill(monitor_pid_, SIGKILL);
            const char *metaStatus = meta_file_ == NULL ? "null" : "Not NULL";
            DEBUG("My pid : %d - meta_file_ : <%s>" , getpid(), metaStatus);
            DEBUG("  Main : %d\n", main_proc_);

            for (int i = 0 ; i < numClient_ ; i++) {
                DEBUG("Kill Pid : %d\n" , pid_[i]);
                kill(pid_[i], SIGKILL);
            }
        }

        if (meta_file_ == NULL) return;
        metadata_.resetDownloadingChunks(meta_file_, 1);
    }
}

int MhProtoClient::getKillProc(void) {
    return *kill_proc_;
}

MhProtoClient::~MhProtoClient(void) {
    DEBUG("MhProtoClient::Destructor\n");
    delete sem_;
    DestroySharedMemory();
}
