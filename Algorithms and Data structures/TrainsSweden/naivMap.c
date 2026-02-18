#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "structs.c"

const int MOD = 127;
const int BUFFER = 256;


void *create_map(){
    map *new = (map*)malloc(sizeof(map));
    new->nOfStations = 0;
    return new;
}

void *create_city(char *name, int id){
    city *new = (city*)malloc(sizeof(city));
    new->name = name;
    new->id = id;
    new->connections = NULL;
    return new;
}

int hash(char *name, int mod){
    int h = 0;
    int i = 0;
    unsigned char c = 0;
    while((c = name[i]) != 0) {
        h = (h*31 + c) % mod;
        i++;
    }
    return h;
}



void connect(city *src, city *dst, int time){

    connection *ab = (connection*)malloc(sizeof(connection));
    connection *ba = (connection*)malloc(sizeof(connection));
    
    //Skapar första conn
    ab->destination = dst;
    ab->distance = time;
    ab->next = NULL;

    //Skapar andra conn
    ba->destination = src;
    ba->distance = time;
    ba->next = NULL;

    //Går in i link listan för src
    connection *n = src->connections; 
    //Om detta är första noden i conn
    if(n == NULL){
        src->connections = ab;
    }else{
        while(n->next != NULL){
            n = n->next;
        }
        n->next = ab;
    }

    //Går in i link listan för dst
    n = dst->connections;

    if(n == NULL){
        dst->connections = ba;
    }else{
        while(n->next != NULL){
            n = n->next;
        }
        n->next = ba;
    }
    
    
}


city *lookup(map *trains, char *name){
    //Extraherar arrayen av pointers från train
    city **stations = trains->stations;
    int index = hash(name, MOD);
    int original_index = index;

    //current blir nuvarande pos i arrayen
    city *current = stations[index];

    while (current != NULL && strcmp(current->name, name) != 0) {
        // Move to the next index using linear probing
        index = (index + 1) % MOD;  // Wrap around using modulo
        if (index == original_index) {
            // We've looped around to the original index, so the table is full
            return NULL;
        }
        current = stations[index];
    }

    if(current == NULL){
        trains->nOfStations++;
        current = create_city(name, trains->nOfStations);
        stations[index] = current;
    }

    return current;
}

int shortest(city *from, city *to, int left) {
    if (from == to) {
        return 0;
    }
    int sofar = -1;

    connection *nxt = from->connections;
    while(nxt != NULL) {
        if (nxt->distance <= left) {
            int d = shortest(nxt->destination, to, left - nxt->distance);
            if (d >= 0 && ((sofar == -1 ) || (d + nxt->distance) < sofar)){
                sofar = (d + nxt->distance);
            }
        }
        nxt = nxt->next;
    }
    return sofar;
}


int search(city *from, city *to, int limit){
    int d = shortest(from, to, limit);
    return d;
}



map *graph(char *file) {
    // buckets or open adressing
    city **cities = malloc(sizeof(city) * MOD);
    for(int i = 0; i < MOD; i++){
        cities[i] = NULL;
    }
    map *trains = create_map();
    trains->stations = cities;
    trains->nOfStations = 0;
    // Open a file in read mode
    FILE *fptr = fopen(file, "r");
    if (fptr == NULL)
        return NULL;
    char *lineptr = malloc(sizeof(char)*BUFFER);
    size_t n = BUFFER;
    while(getline(&lineptr, &n, fptr) > 0) {
        char *copy = (char*)malloc(sizeof(char)*(strlen(lineptr)+1));
        strcpy(copy, lineptr);
        // divide the copy into the three parts
        city *src = lookup(trains,strtok(copy, ","));
        city *dst = lookup(trains, strtok(NULL, ","));
        unsigned int dist = atoi(strtok(NULL, ","));
        // add the connection
        connect(src, dst, dist);
    }
    fclose(fptr);
    return trains;
}
