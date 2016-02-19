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
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <termios.h>

#include "./con_tcp.h"
#include "./tools.h"

#define MAX_RCV_BUFFER 2048

int tcp_connect(const char *host_name, int port) {
    struct sockaddr_in address;
    int  sd,
         result,
         len;
    char ip_str[32];

    memset(ip_str, 0 , sizeof(ip_str));
    CopyString(ip_str, resolve_name(host_name));

    sd = socket(AF_INET, SOCK_STREAM, 0);
    DEBUG("Socket Descriptor : %d\n", sd);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip_str);
    address.sin_port = htons(port);
    len = sizeof(address);
    result = connect(sd, (struct sockaddr *)&address, len);

    if (result == -1) {
        perror("con_tcp ");
        close(sd);
        sd = -1;
    }
    return(sd);
}

const char *resolve_name(const char *name) {
    char **addr;
    struct hostent *host_info;

    host_info = gethostbyname(name);
    if (host_info == NULL) {
        printf("cannot resolve name : %s\n" , name);
        return NULL;
    }
    addr = host_info->h_addr_list;
    return(inet_ntoa(*(struct in_addr*)*addr));
}

int so_read(int sd, void *buff, int len) {
    int bytes_read;
    int res;
    int ptr;
    unsigned char *buffer = static_cast<unsigned char*>(buff);

    bytes_read = 0;

    bytes_read = read(sd, buffer, len);

    if ((errno == EAGAIN) && (bytes_read == -1)) {
        perror("con_tcp.so_read (EAGAIN) ");
        bytes_read = 0;
    }

    if ((errno != EAGAIN) && (bytes_read == -1)) {
        perror("con_tcp.so_read ");
        return(-1);
    }
    res = len - bytes_read;
    ptr = bytes_read;
    while (res != 0) {
        bytes_read = read(sd , &buffer[ptr] , res);
        if ((errno == EAGAIN) && (bytes_read == -1)) bytes_read = 0;
        if (bytes_read <= 0) break;
        res = res - bytes_read;
        ptr = ptr + bytes_read;
    }

    return(ptr);
}

