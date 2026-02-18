#include <stdio.h>
#include <stdlib.h>
#include "structs.c"

void print_path(path *path);

// Function to create a new pq with the given capacity
priorityQ* create_pq(int capacity) {
    priorityQ* pq = (priorityQ*)malloc(sizeof(priorityQ));
    pq->capacity = capacity;
    pq->size = 0;
    pq->array = (path**)malloc(capacity * sizeof(path*));
    return pq;
}

void print_pq(priorityQ* pq) {
    for (int i = 0; i < pq->size; i++) {
        if(pq->array[i]->prev == NULL){
            printf("NULL-%s(%d), ", pq->array[i]->destination->name, pq->array[i]->time);
        }else{
            printf("%s-%s(%d), ", pq->array[i]->prev->name, pq->array[i]->destination->name, pq->array[i]->time);
            
        }
    }
    printf("\n");
}

// Function to pqify down (sink) from index i
void sink(priorityQ* pq, int i) {
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    int smallest = i;

    // Find the smallest among root, left, and right
    if (left < pq->size && pq->array[left]->time < pq->array[smallest]->time) {
        smallest = left;
    }
    if (right < pq->size && pq->array[right]->time < pq->array[smallest]->time) {
        smallest = right;
    }

    // If the smallest is not root, swap and continue sinking
    if (smallest != i) {
        path *temp = pq->array[i];
        pq->array[i] = pq->array[smallest];
        pq->array[smallest] = temp;
        sink(pq, smallest);
    }
}

// Function to pqify up (enqueue) from index i
void enqueue(priorityQ* pq, path *new_path) {
    if (pq->size == pq->capacity) {
        int size = 2*pq->size;
        pq->capacity = size;
        path** copy = (path**)malloc(size*sizeof(path*));

        for (int i = 0; i <= pq->size; i++) {
            copy[i] = pq->array[i];
        }
        free(pq->array);
        pq->array = copy;
    }

    // Insert the new value at the end of the pq
    pq->array[pq->size] = new_path;
    pq->size++;

    // pqify up (enqueue) from the last element to maintain pq property
    int i = pq->size - 1;
    while (i > 0 && pq->array[(i - 1) / 2]->time > pq->array[i]->time) {
        path *temp = pq->array[i];
        pq->array[i] = pq->array[(i - 1) / 2];
        pq->array[(i - 1) / 2] = temp;

        i = (i - 1) / 2;
    }
}

// Function to remove the root (minimum element) from the pq and perform the "sink" operation
path *dequeue(priorityQ* pq) {
    if (pq->size == 0) {
        return NULL;  // Return -1 to indicate error (empty pq)
    }

    path *root = pq->array[0];  // Store the root value (minimum value)

    // Move the last element to the root
    pq->array[0] = pq->array[pq->size - 1];
    pq->size--;

    // pqify down (sink) from the root to restore the pq property
    sink(pq, 0);

    return root;
}

// Function to print the pq array (for debugging purposes)



// Main function to test the pq operations
/*
int main() {
    priorityQ* pq = create_pq(10);  // Create a pq with capacity 10


    // Adding elements using enqueue (pqify up)
    enqueue(pq, 5);
    enqueue(pq, 3);
    enqueue(pq, 8);
    enqueue(pq, 1);
    enqueue(pq, 6);

    print_pq(pq);
    printf("pq cap: %d\n", pq->capacity);

    for (int i = 0; i < 20; i++){
        enqueue(pq, 2*i+20);
    }

    printf("pq cap: %d\n", pq->capacity);

    printf("pq after insertions (enqueue): ");
    print_pq(pq);

    // Remove the root element and pqify down
    printf("Removed root: %d\n", dequeue(pq));
    printf("pq after removing root: ");
    print_pq(pq);

    // Remove the root element again and pqify down
    printf("Removed root: %d\n", dequeue(pq));
    printf("pq after removing root: ");
    print_pq(pq);

    // Clean up the pq memory
    free(pq->array);
    free(pq);

    return 0;
}
*/