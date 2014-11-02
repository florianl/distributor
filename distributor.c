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
#define     PIPE_BUF        2048

struct kit {
    pthread_t       tID;
    int             wID;
    int             fd[2];
};

static void *processing(void *arg){

    struct kit      *tool = arg;
    char            buf[PIPE_BUF];

    fprintf(stdout, "%d %d\n", (int) tool->tID, tool->wID);

    close(tool->fd[1]);     /*  Close the writing end of the pipe   */

    while(read(tool->fd[0], &buf, PIPE_BUF) > 0){
        fprintf(stdout, "[%d] %s\n", tool->wID, buf);
    }
    close(tool->fd[0]);

    return NULL;
}

static void *distribute(void *arg){

    struct kit      *tool = arg;
    int             i = 0;

    for(i=0; i<NUM_WORKER; i++){
        close(tool[i].fd[0]);   /*  Close the reading end of the pipe   */
    }

    while(1){
        write(tool[i%NUM_WORKER].fd[1], (void *) ((48+(i%NUM_WORKER))%58), sizeof(char));
        i++;
    }

    for(i=0; i<NUM_WORKER; i++){
        close(tool[i].fd[1]);
    }

    return NULL;
}

int main(int argc, char *argv[]){

    int             i = 0;
    int             status = 0;
    struct kit      *tool;
    pthread_t       distributor;

    tool = calloc(NUM_WORKER, sizeof(struct kit));
    if(tool == NULL){
        return EXIT_FAILURE;
    }

    fprintf(stdout, "%ld worker will start\n", NUM_WORKER);

    for(i=0; i<NUM_WORKER; i++){
        tool[i].wID = i;
        status = pipe(tool[i].fd);
        if(status){
            fprintf(stderr, "%s\n", strerror(errno));
            return EXIT_FAILURE;
        }


        status = pthread_create(&tool[i].tID, NULL, &processing, &tool[i]);

        if(status){
            fprintf(stderr, "%s\n", strerror(status));
            return EXIT_FAILURE;
        }
    }

    status = pthread_create(&distributor, NULL, &distribute, &tool);

    if(status){
        fprintf(stderr, "%s\n", strerror(status));
        return EXIT_FAILURE;
    }

    status = pthread_join(distributor, NULL);

    if(status){
        fprintf(stderr, "%s\n", strerror(status));
        return EXIT_FAILURE;
    }

    for(i=0; i<NUM_WORKER; i++){

        status = pthread_join(tool[i].tID, NULL);

        if(status){
            fprintf(stderr, "%s\n", strerror(status));
            return EXIT_FAILURE;
        }
    }

    free(tool);

    return EXIT_SUCCESS;
}
