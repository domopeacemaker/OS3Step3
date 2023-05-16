/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   mybucket.h
 * Author: juan-
 *
 * Created on 9 de mayo de 2023, 9:37
 */

#ifndef MYBUCKET_H
#define MYBUCKET_H

#include <string.h>
#include "myrecord.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /* This is the number of records in this bucket */
#define MYBUCKET_NRECORDS 64

  /* BUCKET: This structure contains one single record and additional control fields. */
  typedef struct
  {

    /* Position of this bucket in file (in buckets) */
    unsigned int id;

    /* State of each record inside this bucket. -1: exists, 0: empty */
    int exists[MYBUCKET_NRECORDS];

    /* This is the space needed to store N records inside this bucket. */
    MYRECORD_RECORD_t record[MYBUCKET_NRECORDS];
    
  } MYBUCKET_BUCKET_t;

#ifdef __cplusplus
}
#endif

#endif /* MYBUCKET_H */

