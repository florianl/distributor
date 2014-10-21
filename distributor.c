/*
 * Copyright (c) 2014, Florian Lehner <dev@der-flo.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies of the Software and its documentation and acknowledgment
 * shall be given in the documentation and software packages that this
 * Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

#define     NUM_WORKER      sysconf(_SC_NPROCESSORS_ONLN)

struct kit {
    pthread_t       tID;
    int             wID;
};

static void *processing(void *arg){

    struct kit      *tool = arg;

    fprintf(stdout, "%d %d\n", (int) tool->tID, tool->wID);

    return NULL;
}

int main(int argc, char *argv[]){

    int             i = 0;
    int             status = 0;
    struct kit      *tool;

    tool = calloc(NUM_WORKER, sizeof(struct kit));
    if(tool == NULL){
        return EXIT_FAILURE;
    }

    for(i=0; i<NUM_WORKER; i++){
        tool[i].wID = i;

        status = pthread_create(&tool[i].tID, NULL, &processing, &tool[i]);

        if(status){
            fprintf(stderr, "%s\n", strerror(status));
            return EXIT_FAILURE;
        }
    }

    free(tool);

    return EXIT_SUCCESS;
}
