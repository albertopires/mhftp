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

#ifndef CRYPT_MD5UTILS_H_
#define CRYPT_MD5UTILS_H_

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <openssl/md5.h>

#include "../tools.h"

/** Helper to use md5 algorithm.
  */
class MD5Utils {
 private:
    unsigned char digest[16];
    MD5_CTX context;
    char digestStr[33];

 public:
    void md5Buffer(unsigned char *data , int len);
    void md5File(const char *fileName);
    void md5File(int fd);
    const char *getDigestStr(void);
    const unsigned char *getDigest(void);
    const char *digestToStr(const unsigned char *digestArray);
};

#endif  // CRYPT_MD5UTILS_H_
