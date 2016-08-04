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
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include "./con_tcp.h"
#include "protocol/mhproto_server.h"

#define DEFAULT_PORT 5000

void core(int sd);

int main(int argc , char *argv[]) {
    int sd, cd, one, len, ret, server_port;
    socklen_t client_len;
    struct sockaddr_in address,
                   client_addr;

    printf("MhFtp %s\n\n", VER);

    DEBUG("Debug Enabled.\n");

    server_port = DEFAULT_PORT;
    if (argc == 2) {
        if (strcmp(argv[1], "-v") == 0) exit(0);
    }

    if (argc == 3) {
        if (strcmp(argv[1], "-p") == 0) {
            server_port = atoi(argv[2]);
        }
    }
    printf("Port %d\n", server_port);

    sd = socket(AF_INET, SOCK_STREAM , 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(server_port);

    one = 1;
    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) == -1) {
        printf("SO_REUSEADDR fail (%d)\n" , 1);
        perror("SO_REUSEADDR ");
        exit(1);
    }
    len = sizeof(address);
    ret = bind(sd, (struct sockaddr *)&address, len);
    if (ret == -1) {
        printf("Bind(%d) error on port %d - ", 1, server_port);
        fflush(NULL);
        perror("Description ");
    }
    listen(sd , 50);

    printf("Socket Descriptor : %d\n" , sd);
    signal(SIGCHLD , SIG_IGN);

    client_len = sizeof(client_addr);
    while (true) {
        cd = accept(sd, (struct sockaddr *)&client_addr, &client_len);
        if (cd != -1) {
            int pid = fork();
            if (pid == 0) {
                core(cd);
            }
        }
    }
    return 0;
}

// Handle Protocol
void core(int sd) {
    printf("Core()\n");
    MhProtoServer *mhProto = new MhProtoServer(sd, 0);
    for (;;)
        mhProto->RcvCmd();
}
