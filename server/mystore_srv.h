/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   mystore_srv.h
 * Author: juan-
 *
 * Created on 9 de mayo de 2023, 9:39
 */

#ifndef MYSTORE_CLI_H
#define MYSTORE_CLI_H

#include <stdint.h>
#include <sys/types.h>
#include "myrecord.h"
#include "messages.h"
#include <errno.h>

#define MSG_TYPE_REQUEST 1

extern int message_queue;

#ifdef __cplusplus
extern "C"
{
#endif

#define MESSAGE_QUEUE_KEY 1234 // Un valor arbitrario
  /* This function initializes the server side of the API. */
int STORS_init () {
    // Crea o abre una cola de mensajes
    message_queue = msgget(MESSAGE_QUEUE_KEY, IPC_CREAT | 0666);
    if (message_queue == -1) {
        perror("Error al crear/abrir la cola de mensajes");
        return -1;
    }
    return 0;
}
  /* This function closes the server side of the API. */
int STORS_close () {
    // Elimina la cola de mensajes
    if (msgctl(message_queue, IPC_RMID, NULL) == -1) {
        perror("Error al eliminar la cola de mensajes");
        return -1;
    }
    return 0;
}
  /* This function sends back an answer to the message queue. */
int STORS_readrequest (request_message_t *request) {
    // Lee un mensaje de tipo MSG_TYPE_REQUEST de la cola de mensajes
    while (1) {
        ssize_t result = msgrcv(message_queue, request, sizeof(request_message_t) - sizeof(long), MSG_TYPE_REQUEST, 0);
        if (result == -1) {
            if (errno == EINTR) {
                continue;  // Si la llamada a la función fue interrumpida por una señal, vuelve a intentarlo.
            } else {
                perror("Error al leer la solicitud de la cola de mensajes");
                return -1;
            }
        }
        break;  // Si la llamada a la función fue exitosa, sal del ciclo.
    }
    return 0;
}

  /* This function reads a request from the message queue. */
int STORS_sendanswer (answer_message_t *answer) {
    // Envía un mensaje a la cola de mensajes
    while (1) {
        ssize_t result = msgsnd(message_queue, answer, sizeof(answer_message_t) - sizeof(long), 0);
        if (result == -1) {
            if (errno == EINTR) {
                continue;  // Si la llamada a la función fue interrumpida por una señal, vuelve a intentarlo.
            } else {
                perror("Error al enviar la respuesta a través de la cola de mensajes");
                return -1;
            }
        }
        break;  // Si la llamada a la función fue exitosa, sal del ciclo.
    }
    return 0;
}


#ifdef __cplusplus
}
#endif

#endif /* MYSTORE_CLI_H */

