#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "structs.c"

city *lookup(map *trains, char *name);
map *graph(char *file);
int search(city *from, city *to, int nOfstations);

struct timespec t_start, t_stop;

double nano_seconds(struct timespec *t_start, struct timespec *t_stop) {
    return (t_stop->tv_nsec - t_start->tv_nsec) +
    (t_stop->tv_sec - t_start->tv_sec)*1000000000;
}


int main(int argc, char*argv[]) {
    map *trains = graph("trains.csv");
    if(argc < 3) {
    printf("usage: %s <from> <to> <limit>\n", argv[0]);
    exit(1);
    }
    city *from = lookup(trains, argv[1]);
    city *to = lookup(trains, argv[2]);
    clock_gettime(CLOCK_MONOTONIC, &t_start);
        int s = search(from, to, trains->nOfStations);
    clock_gettime(CLOCK_MONOTONIC, &t_stop);
    long wall = nano_seconds(&t_start, &t_stop);
    
    char *file = "improvedBench.txt";
    //Open file in append mode
    FILE *fptr = fopen(file, "a");
    fprintf(fptr, "\n%s & %s & %d & %.2f \\\\", from->name, to->name, s, ((double)wall)/1000000);
    fclose(fptr);

    if (s > 0)
    printf("shortest path %d found in %.2fms\n", s, ((double)wall)/1000000);
    else
    printf("no path found\n");
}

