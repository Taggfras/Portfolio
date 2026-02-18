#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "structs.c"


const int BUFFER = 256;

path *dequeue(priorityQ* pq);
void enqueue(priorityQ* pq, path *new_path);
priorityQ* create_pq(int capacity);


map *create_map(){
    map *new = (map*)malloc(sizeof(map));
    new->nOfStations = 0;
    return new;
}

city *create_city(char *name, int id){
    city *new = (city*)malloc(sizeof(city));
    new->name = name;
    new->id = id;
    new->connections = NULL;
    return new;
}

path *create_path(city *dest, city *prev, int time){
    path *new = (path*)malloc(sizeof(path));
    new->destination = dest;
    new->prev = prev;
    new->time = time;
    return new;
}

void print_path(path *path){
    if(path != NULL){
        if(path->prev == NULL){
            printf("NULL-%s(%d), ", path->destination->name, path->time);
        }else{
            printf("%s-%s(%d), ", path->prev->name, path->destination->name, path->time);
        }
    }else{
        printf("Path var NULL ");
    }
    return;
}


void print_donePath(city *to, path **done){
    path *current = done[to->id];
    while(current->prev != NULL && current != NULL){
        printf("%s (%d)\n", current->destination->name, current->time);
        current = done[current->prev->id];
    }
    printf("%s (%d)\n", current->destination->name, current->time);
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

int dijkstra(city *from, city *to, path **done, priorityQ *pathQ) {
    bool found = false;
    while (!found){    
        bool newDone = false;
        path *current;
        while(!newDone){
            current = dequeue(pathQ);
            if(current == NULL){
                return -1;
            }
            int index = current->destination->id;
            /*
            print_path(current); //debug
            printf("vs "); //debug
            print_path(done[index]); //debug
            printf("\n"); //debug
            */
            if(index == to->id){
                done[index] = current;
                found = true;
                return 1;
            }
            //Om det inte finns någon path för staden eller om denna väg är kortare
            if(done[index] == NULL){
                done[index] = current;
                newDone = true;
            }else{
                if(current->time < done[index]->time){
                    done[index] = current;
                    newDone = true;
                }
            }
        }

        //Går igenom nuvarande stadens connections
        connection *n = current->destination->connections;
        while (n != NULL){
            //Skapar en ny path för alla connections och lägger de i kön
            int new_time = current->time + n->distance;
            path *new_path = create_path(n->destination, current->destination, new_time);
            enqueue(pathQ, new_path);
            n = n->next;
        }   
    }
}



int search(city *from, city *to, path **done, int nOfStations){
    priorityQ *pathQ = create_pq(nOfStations);
    path *first = create_path(from, NULL, 0);
    enqueue(pathQ, first);
    int s = dijkstra(from, to, done, pathQ);
    free(pathQ->array);
    free(pathQ);
    return s;
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
