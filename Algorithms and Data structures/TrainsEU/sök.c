#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "structs.c"

city *lookup(map *trains, char *name);
map *graph(char *file);
int search(city *from, city *to, path **done, int nOfStations);
void print_donePath(city *to, path **done);

struct timespec t_start, t_stop;

double nano_seconds(struct timespec *t_start, struct timespec *t_stop) {
    return (t_stop->tv_nsec - t_start->tv_nsec) +
    (t_stop->tv_sec - t_start->tv_sec)*1000000000;
}


int main(int argc, char*argv[]) {
    map *trains = graph("europe.csv");
    if(argc < 3) {
    printf("usage: %s <from> <to> <limit>\n", argv[0]);
    exit(1);
    }
    city *from = lookup(trains, argv[1]);
    city *to = lookup(trains, argv[2]);

    path **done = (path**)malloc(sizeof(path*) * trains->nOfStations+1);
    for(int i = 1; i <= trains->nOfStations; i++){
        done[i] = NULL;
    }
    clock_gettime(CLOCK_MONOTONIC, &t_start);
        int s = search(from, to, done, trains->nOfStations);
    clock_gettime(CLOCK_MONOTONIC, &t_stop);
    long wall = nano_seconds(&t_start, &t_stop);
    
    int nOfDone = 0;
    for(int i = 1; i<= trains->nOfStations; i++){
        if(done[i] != NULL){
            nOfDone++;
        }
    }
    

    char *file = "tc.dat";
    //Open file in append mode
    FILE *fptr = fopen(file, "a");
    
    if(s>0){
        int d = done[to->id]->time;
        //fprintf(fptr, "\n%s & %s & %d & Null & %.2f \\\\", from->name, to->name, d, ((double)wall)/1000000);
        fprintf(fptr, "\n%d %0.2f", nOfDone, ((double)wall)/1000000);

        print_donePath(to, done);
        printf("shortest path %d found in %.2fms\n", d, ((double)wall)/1000000);
    }else{
        fprintf(fptr, "\n%s & %s & NULL & %.2f \\\\", from->name, to->name, ((double)wall)/1000000);

        printf("no path found in %.2fms\n", ((double)wall)/1000000);
    }
    fclose(fptr);
}

