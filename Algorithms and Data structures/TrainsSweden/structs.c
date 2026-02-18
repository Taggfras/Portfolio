
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
    city *origin;
    city *destination;
    int time;
} path;
