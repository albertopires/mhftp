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

#include "../crypt/md5Utils.h"

/** Compute md5 of a buffer.
  * @param data unsigned chat pointer to data buffer.
  * @param len size of buffer.
  */
void MD5Utils::md5Buffer(unsigned char *data , int len) {
    MD5_CTX context;

    memset(digest , 0 , sizeof(digest));
    MD5_Init(&context);
    MD5_Update(&context , data , len);
    MD5_Final(digest , &context);

    memset(digestStr , 0 , sizeof(digestStr));
    char digit[6];
    for (int i = 0 ; i < 16 ; i++) {
        snprintf(digit, sizeof(digit), "%.2x" , digest[i]);
        digit[2] = 0;
        StrCat(digestStr , digit);
    }
}

void MD5Utils::md5File(const char *fileName) {
    int fd;

    fd = open(fileName , O_RDONLY);
    if (fd == -1) {
        printf("File Name : <%s>\n" , fileName);
        perror("md5File ");
        return;
    }

    md5File(fd);

    close(fd);
}

void MD5Utils::md5File(int fd) {
    unsigned char buffer[2048];
    MD5_CTX context;
    int br;

    MD5_Init(&context);
    do {
        br = read(fd , buffer , sizeof(buffer));
        if (br > 0) {
            MD5_Update(&context , buffer , br);
        }
    } while (br != 0 && br != -1);
    MD5_Final(digest , &context);
}

const char *MD5Utils::getDigestStr(void) {
    memset(digestStr , 0 , sizeof(digestStr));
    char digit[6];
    for (int i = 0 ; i < 16 ; i++) {
        snprintf(digit, sizeof(digit), "%.2x" , digest[i]);
        digit[2] = 0;
        StrCat(digestStr , digit);
    }

    return digestStr;
}

const unsigned char *MD5Utils::getDigest(void) {
    return digest;
}


const char *MD5Utils::digestToStr(const unsigned char *digestArray) {
    static char str_digest[33];

    memset(str_digest , 0 , sizeof(str_digest));
    char digit[6];
    for (int i = 0 ; i < 16 ; i++) {
        snprintf(digit, sizeof(digit), "%.2x" , digestArray[i]);
        digit[2] = 0;
        StrCat(str_digest , digit);
    }

    return str_digest;
}
