#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "structs.c"

const int Mod = 127;

int hash(char *name, int mod);
city *lookup(map *trains, char *name);
map *graph(char *file);

int main(int argc, char*argv[]) {
    map *trains = graph("trains.csv");
    if(argc < 3) {
    printf("usage: %s <from> <to>\n", argv[0]);
    exit(1);
    }
    city *from = lookup(trains, argv[1]);
    city *to = lookup(trains, argv[2]);

    printf("\nDet finns %d stationer i spårsystemet:\n", trains->nOfStations);
    
    //Implementera en function som printar alla stationer, aka när en pointer inte är null
    int k = 0;
    int i = 1;
    int h = 0;
    while(i <= 52 && k < Mod){
        if(trains->stations[k] != NULL){
            if(h > hash(trains->stations[k]->name, Mod)){
                printf("too tight\n");
            }
            h = hash(trains->stations[k]->name, Mod);
            printf("h:%d %s, ", h, trains->stations[k]->name);
            i++;
        }
        k++;
    }
    printf("\n");

    printf("\n");
    printf("%s har tågspår till:\n", from->name);
    connection *n = from->connections;
    while(n != NULL){
        printf("%s och det tar %d minuter att åka dit\n", n->destination->name, n->distance);
        n = n->next;
    }
    printf("\n");
    printf("%s har tågspår till:\n", to->name);
    n = to->connections;
    while(n != NULL){
        printf("%s och det tar %d minuter att åka dit\n", n->destination->name, n->distance);
        n = n->next;
    }
    printf("\n");
}
    