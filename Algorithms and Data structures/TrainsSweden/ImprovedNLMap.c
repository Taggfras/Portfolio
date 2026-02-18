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


bool loop(city **path, int index, city *destination){
    for(int i = 0; i < index; i++){
        if(path[i]->id == destination->id){
            return true;
        }
    }
    return false;
}


int shortest_path(city *from, city *to, city **path, int k, int *maxD, int current_distance) {
    if (from == to) {
        // När vi når målet, returnera 0 för att indikera att vi har hittat en väg
        return 0;
    }
    if(current_distance == -1){
        return -1;
    } else if(current_distance > *maxD){
        return -1;
    }

    int new_distance = -1;
    connection *nxt = from->connections;
    while (nxt != NULL) {
        if (!loop(path, k, nxt->destination)) {  // Kontrollera att vi inte har besökt staden tidigare
            path[k] = nxt->destination;  // Lägg till destinationen i path

            // Rekursivt anrop för att hitta vägen från nästa stad, och skicka med nuvarande avstånd
            int d = shortest_path(nxt->destination, to, path, k + 1, maxD, current_distance + nxt->distance);

            

            // När vi hittar en väg till målet
            if (d == 0) {
                new_distance = current_distance + nxt->distance;  // Totalt avstånd hittills
                if (new_distance < *maxD) {
                    *maxD = new_distance;  // Uppdatera maxD om den nya vägen är kortare
                } 
                return new_distance;
            }
            else if (d > 0 && (current_distance + nxt->distance) < *maxD) {
                new_distance = current_distance + nxt->distance;
            }

        }
        nxt = nxt->next;  // Gå till nästa anslutning
    }
    return new_distance;
}



int search(city *from, city *to, int nOfStations){
    city **path = malloc(sizeof(city) * nOfStations);
    path[0] = from;
    int maxD = INT_MAX;
    int d = shortest_path(from, to, path, 1, &maxD, 0);
    return maxD;
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
