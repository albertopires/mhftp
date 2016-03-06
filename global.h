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

#ifndef GLOBAL_H_
#define GLOBAL_H_

#define MHFTP_DEBUG    "MHFTP_DEBUG"
#define MHFTP_NOCOLOR  "MHFTP_NOCOLOR"
#define METADATA_EXT   ".mdata"

#define DIGEST_SIZE    16
#define FNAME_SIZE     2048
#define MAX_CHUNK_SIZE 20971520  // 20 MB

#define DEBUG(...) if (debug_enabled) { printf(__VA_ARGS__); }

extern int debug_enabled;  // Declared at tools.cpp

#endif  // GLOBAL_H_
