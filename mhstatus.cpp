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
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>

#include "protocol/metadata.h"

using std::cout;
using std::endl;

void displayData(const char* controlFile, int verbose);
void recoverMetaData(const char* controlFile, int force);
void resetMetaData(const char* controlFile, int force);
void checkChunkData(const char* controlFile, const char *fileName);

int main(int argc, char *argv[]) {
    printf("mhstatus %s\n\n", VER);

    if (argc < 2) {
        cout << "mhstatus <file>\n" << endl;
        cout << "or\n" << endl;
        cout << "mhstatus --verbose/-v <file.mdata>" << endl;
        cout << "mhstatus --recover/-r <file.mdata>" << endl;
        cout << "mhstatus --reset/-e <file.mdata>" << endl;
        cout << "mhstatus --check/-c <file.mdata> <file>\n" << endl;
        exit(1);
    }

    if (argc == 2) {
        displayData(argv[1], 0);
    }

    if (argc == 3 &&
            (strcmp("--verbose", argv[1]) == 0 || strcmp("-v", argv[1]) == 0)) {
        displayData(argv[2], 1);
    }

    if (argc == 3 &&
            (strcmp("--recover", argv[1]) == 0 || strcmp("-r", argv[1]) == 0)) {
        recoverMetaData(argv[2], 0);
    }

    if (argc == 3 &&
            (strcmp("--reset", argv[1]) == 0 || strcmp("-e", argv[1]) == 0)) {
        resetMetaData(argv[2], 0);
    }

    if (argc == 4 &&
            (strcmp("--recover", argv[1]) == 0 || strcmp("-r", argv[1]) == 0) &&
            (strcmp("--force", argv[2])   == 0 || strcmp("-f", argv[2]) == 0)) {
        recoverMetaData(argv[3], 1);
    }

    if (argc == 4 &&
            (strcmp("--reset", argv[1]) == 0   || strcmp("-e", argv[1]) == 0) &&
            (strcmp("--force", argv[2])   == 0 || strcmp("-f", argv[2]) == 0)) {
        resetMetaData(argv[3], 1);
    }

    if (argc == 4 &&
            (strcmp("--check", argv[1]) == 0   || strcmp("-c", argv[1]) == 0)) {
        checkChunkData(argv[2], argv[3]);
    }

    exit(0);
}

void displayData(const char* controlFile, int verbose) {
    Metadata metadata;

    printf("Control File: <%s>\n\n" , controlFile);

    if (verbose) {
        metadata.DisplayControlDataMD5(controlFile);
        cout << endl;
    } else {
        metadata.DisplayControlData(controlFile);
        cout << endl;
    }

    exit(0);
}


void recoverMetaData(const char* controlFile, int force) {
    Metadata metadata;

    metadata.resetDownloadingChunks(controlFile, force);

    exit(0);
}

void resetMetaData(const char* controlFile, int force) {
    Metadata metadata;

    if (metadata.resetAllChunks(controlFile, force) == -1) {
        cout << "Metadata file is in use. It's possible that the " << endl;
        cout << "download is still in progress or the mhclient  " << endl;
        cout << "process was terminated." << endl << endl;
        cout << "If the process is not running use:" <<  endl << endl;
        cout << "mhstatus --reset/-e --force/-f <file.mdata>" <<  endl;
    }

    exit(0);
}

void checkChunkData(const char* controlFile, const char *fileName) {
    Metadata metadata;

    metadata.checkDataChunks(controlFile, fileName);

    exit(0);
}
