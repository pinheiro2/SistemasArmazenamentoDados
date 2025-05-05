#include <glib.h>
#include <stdint.h>
#include <pthread.h>

//struct defining the values of the Hashtable 
typedef struct filemeta {
	//TODO
	char path[200];
} Indexmeta;

//structure containing the hashtable structure, global mutex and condition variable
typedef struct index {
	GHashTable *htable;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
} Index;


//Initializes the index structure
//Returns NULL in case of failure and a pointer to the struct otherwise
Index* index_init();

//Adds a new key-value entry into the Hashtable
//Returns -1 in case of failure (or if the key already exists) and 0 otherwise
int index_add(Index *index, char* key, Indexmeta meta);

//Get the value (meta) for a specific key (key)
//Returns -1 in case of failure (or if the key does not exists) and 0 otherwise
int index_get(Index *index, char* key, Indexmeta *meta);

//Remove a key-value entry from the Hashtable 
//Returns -1 in case of failure (or if the key does not exists) and 0 otherwise
int index_remove(Index* index, char* key);

//Destroys the index structure
void index_destroy(Index* index);





