/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   messages.h
 * Author: juan-
 *
 * Created on 9 de mayo de 2023, 10:09
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
#include "mystore_cli.h"

#define MESSAGE_CLI_OP_FLUSH 0x10
  /**
   * We can define an enum instead of an integer to give unique numbers to
   * protocol operations.
   
  
   */
    
    /*
  typedef enum
  {
    MYSCOP_READ = 0,
    MYSCOP_WRITE,
    /* Any other operation will have its own number here. */
    /*MESSAGE_CLI_OP_FLUSH
 } MYSTORE_CLI_OP;
   
  
  */
          
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
    /* Did you forget some other field? Add it to the message. */
  } answer_message_t;

#ifdef __cplusplus
}
#endif

#endif /* MESSAGES_H */

