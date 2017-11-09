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
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <string>
#include <iostream>

#include "protocol/mhproto_client.h"
#include "crypt/md5Utils.h"
#include "./con_tcp.h"
#include "./tools.h"

using std::string;
using std::cout;
using std::cin;
using std::endl;

void HostPort(const char *hostPortStr, char *host, int *port);
void sig_handler(int signo);
void DownloadFile(int *sds, int n_hosts,
        const char *file, int64_t chunk_size, bool verbose);
void UploadFile(int *sds, int n_hosts,
        const char *file, int64_t chunk_size, bool verbose);

MhProtoClient *mhProto;

int main(int argc, char *argv[]) {
    struct sigaction sa;
    int  *sds, sd;
    int  n_hosts;
    char host[256];
    int  port;
    bool verbose     = false;
    bool cmd_invalid = true;

    printf("MhClient %s\n\n", VER);

    DEBUG("Debug Enabled.\n");

    if (argc < 4) {
        printf("Error: mhclient <cmd> [-v] [-c chunk_size] <file> ");
        printf("<host1:port> <host2:port> .. <hostn:port>\n\n");
        exit(0);
    }

    sa.sa_handler = sig_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_NODEFER;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
          printf("\ncan't catch SIGINT\n");
          exit(0);
    }
    signal(SIGCHLD, SIG_IGN);

    int hosts_index = 3;
    int chunk_size  = 0;

    // Cmd parameters
    int index_param = 2;
    while (argv[index_param][0] == '-') {
        if (strcmp(argv[index_param], "-v") == 0) {
            hosts_index++;
            index_param++;
            verbose = true;
        }
        if (strcmp(argv[index_param], "-c") == 0) {
            index_param++;
            chunk_size = SuffixToInt(argv[index_param]);
            index_param++;
            hosts_index += 2;
        }
    }
    DEBUG("Chunk size selected: %d\n", chunk_size);

    n_hosts = argc - hosts_index;
    sds = static_cast<int*>(malloc(n_hosts*sizeof(int)));

    DEBUG("Number of server connections : %d\n" , n_hosts);
    for (int i = hosts_index ; i < argc ; i++) {
        HostPort(argv[i], host, &port);
        DEBUG("Host : <%s>\n", host);
        DEBUG("Port : %d\n", port);

        sd = tcp_connect(host, port);
        if (sd == -1) {
            perror("client ");
            exit(1);
        }
        sds[i-hosts_index] = sd;
    }

    if (strcmp(argv[1], "download") == 0) {
        cmd_invalid = false;
        DownloadFile(sds, n_hosts, argv[hosts_index-1], chunk_size, verbose);
    }
    if (strcmp(argv[1], "upload") == 0) {
        cmd_invalid = false;
        UploadFile(sds, n_hosts, argv[hosts_index-1], chunk_size, verbose);
    }
    if (cmd_invalid) {
        printf("Invalid command.\n");
        printf("Use download or upload\n");
    }

    exit(0);
}

void UploadFile(int *sds, int n_hosts,
        const char *file, int64_t chunk_size, bool verbose) {
    mhProto = new MhProtoClient(sds, n_hosts, chunk_size, verbose);

    mhProto->UploadFileToServer(file);

    delete mhProto;
}

void DownloadFile(int *sds, int n_hosts,
        const char *file, int64_t chunk_size, bool verbose) {
    char file_name[2048];
    char meta_name[2048];

    mhProto = new MhProtoClient(sds, n_hosts, chunk_size, verbose);

    // Generate Metadata file name and Local file name.
    CopyString(file_name, AbsoluteFile(file));
    CopyString(meta_name, file_name);
    StrCat(meta_name, METADATA_EXT);
    DEBUG("Local File : <%s>\n", file_name);
    DEBUG("Meta  Name : <%s>\n", meta_name);

    // If needed, download Metadata file from server.
    if (!file_exists(meta_name))
        mhProto->DownloadMetadataFromServer(sds[0], file, chunk_size);

    mhProto->DownloadFileFromServer(meta_name, file_name);

    printf("Client - End of transfer.\n");
    delete mhProto;
}

void HostPort(const char *hostPortStr, char *host, int *port) {
    string s(hostPortStr);
    int pos = s.find(":");
    string s_host = string(s, 0, pos);
    string s_port = string(s, ++pos);

    snprintf(host, s_host.size()+1, "%s", s_host.c_str());
    *port = atoi(s_port.c_str());
}

void sig_handler(int signo) {
    if (signo == SIGINT)
        printf("Stopping...\n");
    mhProto->KillClient();

    delete mhProto;

    exit(0);
}
