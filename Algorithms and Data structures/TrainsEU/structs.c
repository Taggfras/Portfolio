#define MOD 347

typedef struct connection{
    struct city *destination;
    unsigned int distance;
    struct connection *next;
} connection;

typedef struct city{
    char *name;
    int id;
    connection *connections; //Firstpekare till LL
} city;

typedef struct map{
    int nOfStations;
    city **stations;
} map;

typedef struct path{
    city *destination;
    city *prev;
    int time;
} path;

// Define the structure for the priorityQ
typedef struct priorityQ{
    path** array;    // Array to store the pq element
    int size;      // Current size of the pq
    int capacity;  // Maximum capacity of the pq
} priorityQ;
