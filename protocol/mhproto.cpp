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

#include "../protocol/mhproto.h"

int64_t *MhProtoCommons::getPayLoadArray(
        int64_t fileSize,
        int64_t chunkSize,
        int64_t nChunks) {
    int64_t *payLoadArray;
    payLoadArray = static_cast<int64_t*>(malloc(nChunks*sizeof(int64_t)));

    for (int i = 0 ; i < nChunks ; i++) {
        int64_t remain = fileSize - (chunkSize * i);
        if (remain >= chunkSize) {
            payLoadArray[i] = chunkSize;
        } else {
            payLoadArray[i] = remain;
        }
    }

    return payLoadArray;
}
