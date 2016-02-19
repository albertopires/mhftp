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

#ifndef TOOLS_H_
#define TOOLS_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

#include <iostream>

#include "./global.h"

#define C_RESET   "\033[0m"
//  Dark Colors
#define DC_BLACK   "\033[0;30m"
#define DC_RED     "\033[0;31m"
#define DC_GREEN   "\033[0;32m"
#define DC_YELLOW  "\033[0;33m"
#define DC_BLUE    "\033[0;34m"
#define DC_MAGENTA "\033[0;35m"
#define DC_CYAN    "\033[0;36m"
#define DC_WHITE   "\033[0;37m"

//  Light Colors
#define LC_BLACK   "\033[1;30m"
#define LC_RED     "\033[1;31m"
#define LC_GREEN   "\033[1;32m"
#define LC_YELLOW  "\033[1;33m"
#define LC_BLUE    "\033[1;34m"
#define LC_MAGENTA "\033[1;35m"
#define LC_CYAN    "\033[1;36m"
#define LC_WHITE   "\033[1;37m"

using std::cout;
using std::endl;

int is_debug_enabled(void);
void hex_dump(const void *v_buffer , int size);
void CopyString(char *dst_str, const char *src_str);
void CopyString(char *dst_str, const char *src_str, int len);
void StrCat(char *dst_str, const char *src_str);
void PrintColor(const char *str, const char *color, int reol);
void PrintColor(const char *str, const char *color);
const char *AbsoluteFile(const char *full_path);
bool file_exists(const char *file_name);
long i32toLong(int32_t i);  // NOLINT(runtime/int)
long i64toLong(int64_t i);  // NOLINT(runtime/int)
bool fileExists(const char *fname);

#endif  // TOOLS_H_
