/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdbool.h>
#include "myrecord.h"
#include "cache.h"

// Definición de las entradas de la caché
cache_entry_t cache_entries[MAX_CACHE_SIZE];


//Inicializar la cache
void cache_init(){
    int i;
    for (i = 0; i < MAX_CACHE_SIZE; i++){
        cache_entries[i].valid = false;
    }
}
// Implementación de las funciones de caché
int cache_read(MYRECORD_RECORD_t *record, int index) {
    if (index < 0 || index >= MAX_CACHE_SIZE) {
        return -1;
    }

    if (cache_entries[index].valid) {
        *record = cache_entries[index].record;
        return 0;
    } else {
        return -1;
    }
}

int cache_write(MYRECORD_RECORD_t *record, int index) {
    if (index < 0 || index >= MAX_CACHE_SIZE) {
        return -1;
    }

    cache_entries[index].record = *record;
    cache_entries[index].valid = true;
    return 0;
}
