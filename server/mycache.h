/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   mycache.h
 * Author: juan-
 *
 * Created on 9 de mayo de 2023, 9:38
 */

#ifndef MYCACHE_H
#define MYCACHE_H

#include <stdint.h>
#include <sys/types.h>

#include "mybucket.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /* This is the size of our cache in buckets.  */
#define MYC_NUMENTRIES 64

  /* This is the default name of the DB file. */
#define MYC_FILENAME "myDBtable.dat"

  /* This function initializes the cache. */
  int MYC_initCache ();
  /* This function closes the cache. It flushes all the information inside the
     cache that is not written to the file yet. */
  int MYC_closeCache ();

  /* This function reads a record from the file (at given index)
   * inside the record passed as argument. */
  int MYC_readEntry (int fileIndex, MYRECORD_RECORD_t *record);

  /* This function writes a record into the cache from the record passed as argument.
   * The record will be written at the given index of the file later.
   * This funtions does not write the cache entry to the file inmediately. */
  int MYC_writeEntry (int fileIndex, MYRECORD_RECORD_t *record);
  /* This function flushes one cache entry containing one record to be written
   * at the given index. */
  int MYC_flushEntry (int fileIndex);
  /* This function flushes all the entries of the cache to the file. */
  int MYC_flushAll ();

#ifdef __cplusplus
}
#endif

#endif /* MYCACHE_H */


