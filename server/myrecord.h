/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   myrecord.h
 * Author: juan-
 *
 * Created on 9 de mayo de 2023, 9:38
 */

#ifndef MYRECORD_H
#define MYRECORD_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define MYRECORD_NAMELENGTH 16

  /* This is the definition of one record in my database.
   * SQL equivalent could be:
   * CREATE TABLE (
   *    registerid  INTEGER,
   *    age         INTEGER,
   *    gender      INTEGER,
   *    name        CHAR(15)
   *  );
   */
  typedef struct
  {
    /* This is the ID field of the record. */
    unsigned int registerid;
    /* Age */
    int age;
    /* Gender is an integer with fixed values for genders.*/
    int gender;
    /* Name is a fixed length string. It can only store 15 chars plus a terminating zero char. */
    char name[MYRECORD_NAMELENGTH];
  } MYRECORD_RECORD_t;

  /* We define some useful macros to help writing code. */

  /* This MACRO allocates space for a record. */
#define myb_newrecord() ((MYRECORD_RECORD_t *)calloc(1,sizeof(MYRECORD_RECORD_t)))
  /* You don't need a special function to free but this macro is nicer. */
#define myb_freerecord(r) free(r)


#ifdef __cplusplus
}
#endif

#endif /* MYRECORD_H */

