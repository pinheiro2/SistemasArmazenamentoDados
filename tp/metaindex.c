#include "metaindex.h"
#include <stdio.h>

Index* index_init(){

	//allocate memory for the index
	Index *index = malloc(sizeof(Index));

	//GHashTable initialization
	index->htable = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

	//mutex variable initialization
	//int pthread_mutex_init(pthread_mutex_t *mutex,const pthread_mutexattr_t *attr);
	//attr can be used to define non-default attributes (e.g., recursive lock)
	pthread_mutex_init(&index->mutex, NULL);

	return index;

}

//Note: remember that memory allocation and copying must be done here
int index_add(Index *index, char* key, Indexmeta meta){

	pthread_mutex_lock(&index->mutex);

	if(g_hash_table_contains(index->htable, key)){
		pthread_mutex_unlock(&index->mutex);
		return -1;
	}

	char *nkey = strdup(key);
	Indexmeta *value = malloc(sizeof(Indexmeta));
	
	strcpy(value->path, meta.path);

	g_hash_table_insert(index->htable,nkey,value);

	pthread_mutex_unlock(&index->mutex);

	return 0;
}

int index_get(Index *index, char* key, Indexmeta *meta){

	pthread_mutex_lock(&index->mutex);

	Indexmeta *res = g_hash_table_lookup(index->htable, key);
	if(res==NULL){
		pthread_mutex_unlock(&index->mutex);
		return -1;
	}

	strcpy(meta->path, res->path);

	pthread_mutex_unlock(&index->mutex);
	
	return 0;

}


int index_remove(Index* index, char* key){

	pthread_mutex_lock(&index->mutex);

	Indexmeta *res = g_hash_table_lookup(index->htable, key);
	if(res==NULL){
		pthread_mutex_unlock(&index->mutex);
		return -1;
	}

	g_hash_table_remove (index->htable,key);
	
	pthread_mutex_unlock(&index->mutex);

	return 0;
}

void index_destroy(Index* index){

	//destroy hashtable
	g_hash_table_destroy(index->htable);

	//Useful for exercise 2.2
	//destroy mutex and cond variables
	pthread_mutex_destroy(&index->mutex);
    pthread_cond_destroy(&index->cond);

	free(index);
}