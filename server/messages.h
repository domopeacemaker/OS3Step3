/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   messaged.h
 * Author: juan-
 *
 * Created on 9 de mayo de 2023, 9:36
 */

#ifndef MESSAGES_H
#define MESSAGES_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "myrecord.h"

  /**
   * We can define an enum instead of an integer to give unique numbers to
   * protocol operations.
   */
  typedef enum
  {
    MYSTORE_CLI_OP_READ = 0,
    MYSTORE_CLI_OP_WRITE,
    MYSTORE_CLI_OP_FLUSH
    /* Any other operation will have its own number here. */
  } MYSTORE_CLI_OP;
  
  typedef enum
  {
      MYSTORE_SRV_OP_OK = 0,
      MYSTORE_SRV_OP_ERROR
  } MYSTORE_SRV_OP;
  

  /**
   * Message for a request from the client.
   */
  typedef struct
  {
    long mtype; /* This type distinguishes messages to server from messages to clients. */
    MYSTORE_CLI_OP requested_op; /* This is the selected operation to perform. */
    long return_to; /* The client sends a type to address the reply to because we may have several clients. */
    MYRECORD_RECORD_t data; /* This field contains a record only when writing. */
    int index; /* Record index to read or write */

    /* Did you forget some other field? Add it to the message. */
  } request_message_t;

  /**
   * Message for an answer from the server.
   */
  typedef struct
  {
    long mtype; /* This type distinguishes messages to server from messages to clients. */
    int status; /* This status passes back the result of each operation. */
    MYRECORD_RECORD_t data; /* This field contains a record only when reading. */
    char message[100];
    /* Did you forget some other field? Add it to the message. */
  } answer_message_t;

#ifdef __cplusplus
}
#endif

#endif /* MESSAGES_H */

