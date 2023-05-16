/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   mystore_cli.h
 * Author: juan-
 *
 * Created on 9 de mayo de 2023, 10:07
 */

#ifndef MYSTORE_CLI_H
#define MYSTORE_CLI_H

#define MYSTORE_QUEUE_KEY 1234
#define MAX_FILES 1000
#define RECORD_SIZE 2048 // changed to 2048 from 1024
#define MESSAGE_TYPE_REQUEST 0x10
#define MESSAGE_REQUEST_TYPE_READ 0x02
#define MAX_RESPONSE_SIZE 512 // changed to 512 from 256
#define MESSAGE_RESPONSE_OK 0
#define STOR_REQUEST_WRITE 0x10
//#define MESSAGE_REQUEST_TYPE_WRITE MYSTORE_OP_WRITE
#define MESSAGE_REQUEST_TYPE_CHECK 1
//#define MAX_FILES 100
#define MYSTORE_OP_WRITE  3


#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "myrecord.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum {
    STOR_MSG_REQUEST_READ,
    STOR_MSG_REQUEST_WRITE,
    STOR_MSG_REQUEST_REMOVE,
    STOR_MSG_REQUEST_FLUSH,
    STOR_MSG_REQUEST_FLUSH_ALL,
    STOR_MSG_REQUEST_CREATE,
} STOR_MSG_REQUEST_t;
/*
  typedef struct {
    int type;
    int file_index;
    MYRECORD_RECORD_t record;
} STOR_MSG_REQUEST_t;
*/
typedef enum {
    STOR_MSG_REQUEST_WRITE_TYPE,
} STOR_MSG_REQUEST_WRITE_t;

typedef struct {
    int message_type;
    STOR_MSG_REQUEST_t request_type;
    STOR_MSG_REQUEST_WRITE_t request_write_type;
    int file_index;
    int result;
    int type;
    MYRECORD_RECORD_t record;
    //int record;
    int client_id;
    int response;
    char* response_data[MAX_RESPONSE_SIZE];
} MESSAGE_REQUEST_t;

struct file_entry {
    int inUse; // 1 if the entry is being used, 0 if it's available
    int fd; // The file descriptor of the file
    char buffer[RECORD_SIZE]; // A buffer to store a record
    int recordIndex; // The index of the next record to be written
    char *fileName;
    char *fileContents;
};

struct file_entry fileTable[MAX_FILES];

typedef struct {
    int message_type;
    int result;
    MYRECORD_RECORD_t content;
} STOR_MSG_RESPONSE_t;

typedef struct {
    int message_type;
    STOR_MSG_REQUEST_t request_type;
    int result;
    int client_id;
    int type;
    int file_index;
    int response;
    int record;
} MESSAGE_RESPONSE_t;

typedef struct {
    int id;
    int type;
    int length;
    int size;
    char *data;
} STOR_RECORD_t;

/*
typedef enum {
    MYSTORE_OP_READ = 0,
    MYSTORE_OP_WRITE = 1,
    MYSTORE_OP_DELETE = 2
} MYSTORE_OPERATION_TYPE;
*/
typedef enum {
    MYSTORE_RESPONSE_OK = 0,
    MYSTORE_RESPONSE_ERROR = 1
} MYSTORE_RESPONSE_TYPE;

        
typedef struct {
    int type;
    int data_length;
    char *data;
    int file_index;
    int message_type;
    int operation;
    int result;
    enum {
        STOR_MSG_REQUEST,
        STOR_MSG_RESPONSE,
        STOR_MSG_NOTIFICATION
    } message_category;
    union {
        STOR_MSG_REQUEST_WRITE_t request;
        STOR_MSG_RESPONSE_t response;
    } message_content;
    MYRECORD_RECORD_t record;
    //STOR_MSG_RESPONSE_t response;
    int client_id;
} STOR_MESSAGE_t;

typedef enum {
    // Tus otros tipos de operación aquí...
    MYSTORE_CLI_OP_READ,
    MYSTORE_CLI_OP_WRITE,
    MYSTORE_CLI_OP_FLUSH,
} MYSTORE_CLI_OP;

/* This function initializes the client API. */
int STORC_init();

/* This function closes only the client API. Not the storage server. */
int STORC_close();

/* This function reads a record from the storage server. */
int STORC_read(int fileIndex, MYRECORD_RECORD_t *record);

/* This function writes a record into the storage server. */
//int STORC_write(int fileIndex, MYRECORD_RECORD_t *record);
int STORC_write (MYRECORD_RECORD_t *record);

/* This function flushes this record index inside the storage server. */
int STORC_flush(int fileIndex);

/* This function flushes all the entries in the storage server. */
int STORC_flushAll();

int STORC_getSocket();

int STORC_check (int fileIndex);

#ifdef __cplusplus
}
#endif

#endif /* MYSTORE_CLI_H */
