/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   cache.h
 * Author: juan-
 *
 * Created on 15 de mayo de 2023, 0:32
 */

#ifndef CACHE_H
#define CACHE_H

#include <stdbool.h>
#include "myrecord.h"

#define MAX_CACHE_SIZE 100

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    bool valid; // Indica si la entrada de la caché es válida
    MYRECORD_RECORD_t record; // es el registro en sí
    int index;
} cache_entry_t;

extern cache_entry_t cache_entries[MAX_CACHE_SIZE];

    
// Declaración de funciones para trabajar con la caché
void cache_init();
int cache_read(MYRECORD_RECORD_t* record, int index);
int cache_write(MYRECORD_RECORD_t* record, int index);

#ifdef __cplusplus
}
#endif

#endif /* CACHE_H */

