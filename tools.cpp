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

#include "./tools.h"

int debug_enabled = is_debug_enabled();

int is_debug_enabled(void) {
    char *debug_str = getenv(MHFTP_DEBUG);
    if (debug_str != NULL)
        return 1;
    else
        return 0;
}

/* Prints an Hex dump to stdout. */
void hex_dump(const void *v_buffer , int size) {
    unsigned char buffer[17];
    unsigned const char *x_buffer = static_cast<unsigned const char*>(v_buffer);
    int  i, j, c;

    c = 0;
    for (i = 0 ; i < size ; i++) {
        printf("%.2X " , (x_buffer[i] & 0xff));
        c++;
        if (c == 16) {
            memset(buffer, 0, sizeof(buffer));
            c--;
            memcpy(buffer, &x_buffer[i-c], 16);
            for (j = 0 ; j < 16 ; j++)
                if (buffer[j]<0x20 || buffer[j]>0x7f) buffer[j] = 0x2e;
            printf("- %s\n" , buffer);
            c = 0;
        }
    }

    if (c != 0) {
        memset(buffer , 0 , sizeof(buffer));
        memcpy(buffer , &x_buffer[i-c] , c);
        for (j = 0 ; j < c ; j++)
            if (buffer[j]<0x20 || buffer[j]>0x7f) buffer[j] = 0x2e;
        printf("%*s" , ((16-c)*3) , "");
        printf("- %s\n" , buffer);
        c = 0;
    }
    printf("\n");
}

void CopyString(char *dst_str, const char *src_str) {
    if (dst_str == NULL) {
        cout << "Error: dst_str == NULL" << endl;
        return;
    }
    if (src_str == NULL) {
        cout << "Error: str_str == NULL" << endl;
        return;
    }
    snprintf(dst_str, strlen(src_str)+1, "%s", src_str);
}

void CopyString(char *dst_str, const char *src_str, int len) {
    if (dst_str == NULL) {
        cout << "Error: dst_str == NULL" << endl;
        return;
    }
    if (src_str == NULL) {
        cout << "Error: str_str == NULL" << endl;
        return;
    }
    snprintf(dst_str, len+1, "%s", src_str);
}

void StrCat(char *dst_str, const char *src_str) {
    int dst_len = strlen(dst_str);
    int len = dst_len + strlen(src_str);
    len++;  // NULL
    snprintf(&dst_str[dst_len], len, "%s", src_str);
}

/** reol : 1 == Reset End of Line.
 */
void PrintColor(const char *str, const char *color, int reol) {
    char *mhcolor = getenv(MHFTP_NOCOLOR);
    if (mhcolor == NULL) {
        printf("%s%s", color, str);
        if (reol == 1) cout << C_RESET;
    } else {
        printf("%s", str);
    }
}

void PrintColor(const char *str, const char *color) {
    PrintColor(str, color, 1);
}

const char *AbsoluteFile(const char *full_path) {
    const char *abs_file;

    for (int i = strlen(full_path) ; i > 0 ; i--) {
        if (full_path[i] == '/') {
            abs_file = strdup(&full_path[i+1]);
            break;
        }
    }

    return abs_file;
}

bool file_exists(const char *file_name) {
    struct stat buffer;
    return stat(file_name, &buffer) == 0;
}

long i32toLong(int32_t i) {  // NOLINT(runtime/int)
    return i;
}

long i64toLong(int64_t i) {  // NOLINT(runtime/int)
    return i;
}

bool fileExists(const char *fname) {
    if (access(fname, F_OK) != -1) {
        // file exists
        return true;
    }

    return false;
}
