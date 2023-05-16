/*
 * File:   client.c
 * Author: juan-
 *
 * Created on 9 de mayo de 2023, 10:09
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "mystore_cli.h"
#include "messages.h"
#include <string.h>

#define DEBUG_LEVEL 0

/************************************************************
 PRIVATE VARIABLES
 ************************************************************/

static int message_queue = -1;
STOR_MESSAGE_t request_message;

/************************************************************
 PRIVATE FUNCTIONS
 ************************************************************/

// No private functions currently.

/************************************************************
 PUBLIC FUNCTIONS
 ************************************************************/

// Inicializa el cliente STORC.
int STORC_init()
{
  // Obtiene la cola de mensajes.
  message_queue = msgget(MYSTORE_QUEUE_KEY, 0666);
  if (message_queue < 0) {
    perror("msgget");
    return -1;
  }

  return 0;
}

// Cierra el cliente STORC.
int STORC_close()
{
  if (message_queue != -1) {
    // Restablece la variable de la cola de mensajes.
    message_queue = -1;
  }
  return 0;
}

// Lee un registro desde el servidor.
int STORC_read(int fileIndex, MYRECORD_RECORD_t *record)
{
  // Crea el mensaje de solicitud.
  MESSAGE_REQUEST_t request_message;
  request_message.client_id = getpid();
  request_message.message_type = MESSAGE_TYPE_REQUEST;
  request_message.request_type = MESSAGE_REQUEST_TYPE_READ;
  request_message.file_index = fileIndex;

  // Envía el mensaje de solicitud al servidor.
  int send_status = msgsnd(message_queue, &request_message, sizeof(MESSAGE_REQUEST_t) - sizeof(long), 0);
  if (send_status == -1) {
    perror("Error sending request to server");
    return -1;
  }

  // Recibe el mensaje de respuesta del servidor.
  MESSAGE_RESPONSE_t response_message;
  int receive_status = msgrcv(message_queue, &response_message, sizeof(MESSAGE_RESPONSE_t) - sizeof(long), getpid(), 0);
  if (receive_status == -1) {
    perror("Error receiving response message from server");
    return -1;
  }

  if (response_message.response == MESSAGE_RESPONSE_OK) {
    // Copia el registro recibido en el parámetro 'record'.
    memcpy(record, &response_message.record, sizeof(MYRECORD_RECORD_t));
  }

  int status = response_message.response;
  return status;
}

// Escribe un registro en el servidor.
int STORC_write(MYRECORD_RECORD_t *record)
{
  int status;
  STOR_MESSAGE_t request_message;
  STOR_MESSAGE_t response_message;

  // Configura el mensaje de solicitud con el registro a escribir.
  request_message.operation = MYSTORE_OP_WRITE;
  memcpy(&request_message.record, record, sizeof(MYRECORD_RECORD_t));

  // Envía el mensaje de solicitud al servidor.
  status = msgsnd(message_queue, &request_message, sizeof(STOR_MESSAGE_t) - sizeof(long), 0);
  if (status < 0) {
    perror("STORC_write: msgsnd");
    return -1;
  }

  // Recibe el mensaje de respuesta del servidor.
  status = msgrcv(message_queue, &response_message, sizeof(response_message) - sizeof(long), MYSTORE_RESPONSE_OK, 0);
    if (status < 0) {
    perror("STORC_write: msgrcv");
    return -1;
  }

  if (response_message.result < 0) {
    perror("STORC_write: server returned error");
    return -1;
  }

  return 0;
}

// Realiza una operación de flush en el servidor.
int STORC_flush(int fileIndex) {
  // Crea el mensaje de solicitud para la operación de flush.
  MESSAGE_REQUEST_t request_message;
  request_message.client_id = getpid();
  request_message.message_type = MESSAGE_TYPE_REQUEST;
  request_message.request_type = MESSAGE_CLI_OP_FLUSH;
  request_message.file_index = fileIndex;

  // Envía el mensaje de solicitud de flush al servidor.
  int send_status = msgsnd(message_queue, &request_message, sizeof(MESSAGE_REQUEST_t) - sizeof(long), 0);
  if (send_status == -1) {
    perror("Error sending flush request to server");
    return -1;
  }

  // Recibe el mensaje de respuesta del servidor.
  MESSAGE_RESPONSE_t response_message;
  int receive_status = msgrcv(message_queue, &response_message, sizeof(MESSAGE_RESPONSE_t) - sizeof(long), getpid(), 0);
  if (receive_status == -1){
    perror("Error receiving response message from server");
    return -1;
  }

  int status = response_message.response;
  return status;
}

// Verifica el estado de la conexión con el servidor
int checkServerConnection() {
  // Realiza una operación de prueba con el servidor
  int fileIndex = 0;
  MYRECORD_RECORD_t record;
  int status = STORC_read(fileIndex, &record);

  // Verifica el resultado de la operación de prueba
  if (status == 0) {
    // La operación se realizó correctamente, el cliente está conectado al servidor
    printf("Conexión exitosa con el servidor\n");
    return 1;
  } else {
    // Ocurrió un error al realizar la operación, la conexión falló
    fprintf(stderr, "Error al conectar con el servidor\n");
    return 0;
  }
}

int main() {
  // Inicializa el cliente STORC
  if (STORC_init() != 0) {
    fprintf(stderr, "Error al inicializar el cliente STORC\n");
    return 1;
  }

    // Verifica el estado de la conexión con el servidor
  if (!checkServerConnection()) {
    // La conexión falló, sal del programa
    STORC_close();
    return 1;
  }
  
  // Ejemplo de uso: Lee un registro del servidor
  int fileIndex = 0;
  MYRECORD_RECORD_t record;
  int status = STORC_read(fileIndex, &record);
  if (status == 0) {
    // El registro se leyó correctamente
    printf("Registro leído del servidor:\n");
    printf("ID: %u\n", record.registerid);
    printf("Age: %d\n", record.age);
    printf("Gender: %d\n", record.gender);
    printf("Name: %s\n", record.name);
  } else {
    // Ocurrió un error al leer el registro
    fprintf(stderr, "Error al leer el registro del servidor\n");
  }

  // Ejemplo de uso: Escribe un registro en el servidor
  MYRECORD_RECORD_t newRecord;
  newRecord.registerid = 1;
  newRecord.age = 25;
  newRecord.gender = 1;
  strncpy(newRecord.name, "John", MYRECORD_NAMELENGTH);
  status = STORC_write(&newRecord);
  if (status == 0) {
    printf("Registro escrito en el servidor correctamente\n");
  } else {
    fprintf(stderr, "Error al escribir el registro en el servidor\n");
  }

  // Ejemplo de uso: Realiza una operación de flush en el servidor
  status = STORC_flush(fileIndex);
  if (status == 0) {
    printf("Operación de flush realizada en el servidor correctamente\n");
  } else {
    fprintf(stderr, "Error al realizar la operación de flush en el servidor\n");
  }

  // Cierra la conexión con el servidor
  STORC_close();

  return 0;
}
