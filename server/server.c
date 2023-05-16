/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   server.c
 * Author: juan-
 *
 * Created on 9 de mayo de 2023, 9:35
 *
 * Características implementadas hasta ahora 9 de mayo 2023 10:26:
 * -Interceptación de señales de terminación para una terminación ordenada, que incluye el vaciado del caché en el disco.
 * -Temporizador para vaciar el caché periódicamente.
 * -Capacidad para hacer fork() y desvincularse de la línea de comandos, permitiendo que el daemon continúe en segundo plano como un servicio real.
 * -Estas son las características que faltan por implementar:

 * Quedan:
 * Agregar señales personalizadas para realizar acciones útiles dentro del daemon, como imprimir estadísticas en la consola.
 * Redirigir los mensajes a un archivo de registro para que puedas cerrar la terminal sin perder los mensajes generados durante su vida.
 * Agregar argumentos de línea de comandos para controlar el comportamiento al iniciarse.
 * El código actual proporcionado incluye las mejoras solicitadas en la gestión de señales, el manejo del caché y la ejecución en segundo plano.
 * 
 * Errores pendientes a corregir de la versión 1.2:
 * Al pasar al nuevo proyecto, problemas con las librerías. (Por ahora)
 * 

 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "mystore_srv.h"
#include "messages.h"
#include "myrecord.h"
#include <errno.h>
#include "cache.h"
//#include "cache.c"

#define DEBUG_LEVEL 0

// Estructura de caché básica
/*
typedef struct {
  int is_modified; // indica si el contenido de la caché ha sido modificado
  off_t offset; // desplazamiento en el archivo donde se encuentra el contenido de la caché
  size_t size; // tamaño del contenido de la caché
  char data[256]; // contenido de la caché (ajustar el tamaño según sea necesario)
} cache_entry_t;
*/
// Declaración de funciones para trabajar con la caché
void server_cache_init(cache_entry_t *cache, int cache_size);
void cache_flush(cache_entry_t *cache, int cache_size);
int write_to_disk(const void *data, size_t size, off_t offset);

// Variables globales
int message_queue = -1; // identificador de la cola de mensajes
static cache_entry_t cache[10]; // caché de tamaño 10 (ajustar el tamaño según sea necesario)
static int cache_size = sizeof(cache) / sizeof(cache_entry_t); // tamaño de la caché

// Funciones para el manejo de señales
static void handle_termination_signal(int sig);
static void handle_cache_flush(int sig);
static void setup_signals();
static void setup_cache_flush_timer(int interval_seconds);

// Implementación de funciones para trabajar con la caché
void server_cache_init(cache_entry_t *cache, int cache_size) {
  // Inicializar la caché
    int i;
   /* for (i = 0; i < cache_size; i++) {
    cache[i].is_modified = 0;
    cache[i].offset = 0;
    cache[i].size = 0;
  }*/
    for (i = 0; i < cache_size; i++){
        cache[i].valid = false;
    }
}

int write_to_disk(const void *data, size_t size, off_t offset) {
  int fd;
  ssize_t bytes_written;

  fd = open("your_output_file.bin", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR); // abrir el archivo para escritura
  if (fd == -1) {
    perror("Error opening file"); // error al abrir el archivo
    return errno;
  }

  if (lseek(fd, offset, SEEK_SET) == -1) {
    perror("Error seeking to file offset"); // error al buscar la posición en el archivo
    close(fd);
    return errno;
  }

  bytes_written = write(fd,(const char *)data, size); // escribir los datos en el archivo
  if (bytes_written == -1) {
    perror("Error writing to file"); // error al escribir en el archivo
    close(fd);
    return errno;
  }

  close(fd); // cerrar el archivo
  return 0;
}

void cache_flush(cache_entry_t *cache, int cache_size) {
// Vacía la caché escribiendo los datos en el archivo si han sido modificados
    int i;
    for (i = 0; i < cache_size; i++) {/*
        if (cache[i].is_modified) {
        // No estoy seguro si esta función se utiliza para escribir datos en el disco
        write_to_disk(cache[i].data, cache[i].size, cache[i].offset);
        cache[i].is_modified = 0; */ 
        if (cache[i].valid){
            write_to_disk(&cache[i].record, sizeof(MYRECORD_RECORD_t), cache[i].index * sizeof(MYRECORD_RECORD_t));
            cache[i].valid = false;
        }
    }
}

int main(int argc, char *argv[]) {
 int daemon_flag = 0;
 int option;
 pid_t pid, sid;
 printf("El servidor está operativo.\n");

    while ((option = getopt(argc, argv, "d")) != -1) {
     switch (option) {
     case 'd':
     daemon_flag = 1;
     break;
     default:
     fprintf(stderr, "Usage: %s [-d]\n", argv[0]);
     exit(EXIT_FAILURE);
     }
    }

 if (daemon_flag) {
 // Si se especifica la flag '-d', crear un proceso hijo y terminar el proceso padre
 pid = fork();
    if (pid < 0) {
    perror("Fork failed");
    exit(EXIT_FAILURE);
    }

    if (pid > 0) {
    exit(EXIT_SUCCESS);
    }  
 
    // Crea una nueva sesión para el proceso hijo
    sid = setsid();
    if (sid < 0) {
    perror("setsid failed");
    exit(EXIT_FAILURE);
    }

    // Cambia el directorio de trabajo al directorio raíz
    if ((chdir("/")) < 0) {
    perror("chdir failed");
    exit(EXIT_FAILURE);
    }

    // Cierra los descriptores de archivo estándar
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    }

    // Inicializa la cola de mensajes
    if (STORS_init() != 0) {
    perror("STORS_init failed");
    exit(EXIT_FAILURE);
    }

    // Inicializa la caché
    //server_cache_init(cache, cache_size);
    server_cache_init(cache_entries, MAX_CACHE_SIZE);
 
    // Configura el manejo de señales y el temporizador para el vaciado periódico de la caché
    setup_signals();
    setup_cache_flush_timer(30); // vaciar la caché cada 30 segundos

        while (1) {
         request_message_t request;
         answer_message_t answer;
         // Lee la solicitud de la cola de mensajes
            if (STORS_readrequest(&request) != 0) {
              perror("STORS_readrequest failed");
              continue;
}

// Procesar la solicitud y generar la respuesta
// Hay que implementar aquí el procesamiento de las solicitudes
    // Procesar la solicitud y generar la respuesta
switch (request.requested_op) {
    case MYSTORE_CLI_OP_READ:
        // Código para manejar las solicitudes de lectura
        if (request.index < 0 || request.index >= cache_size) {
            // Si el índice está fuera de rango, envía un error
            answer.status = MYSTORE_SRV_OP_ERROR;
            strcpy(answer.message, "Error: Index out of range.");
        } else {
            // Leer el registro de la caché
            //MYRECORD_RECORD_t *record = cache_read(&cache_entries[request.index]);
            MYRECORD_RECORD_t *record = malloc(sizeof(MYRECORD_RECORD_t));
            // Copiar el registro leído a la respuesta
            answer.data = *record;
            //answer.size = sizeof(MYRECORD_RECORD_t);
            answer.status = MYSTORE_SRV_OP_OK;
        }
        break;

    case MYSTORE_CLI_OP_WRITE:
        // Código para manejar las solicitudes de escritura
        if (request.index < 0 || request.index >= cache_size) {
            // Si el índice está fuera de rango, envía un error
            answer.status = MYSTORE_SRV_OP_ERROR;
            strcpy(answer.message, "Error: Index out of range.");
        } else {
            // Escribe el registro en la caché
            //cache_write(cache, request.index, request.data);
            cache_write(&(cache_entries[request.index].record), request.index);
            answer.status = MYSTORE_SRV_OP_OK;
        }
        break;

    case MYSTORE_CLI_OP_FLUSH:
        // Código para manejar las solicitudes de flush
        cache_flush(cache, cache_size);
        answer.status = MYSTORE_SRV_OP_OK;
        break;

    default:
        // Código para manejar solicitudes no reconocidas
        answer.status = MYSTORE_SRV_OP_ERROR;
        strcpy(answer.message, "Error: Unrecognized operation.");
        break;
}
    // Enviar la respuesta a través de la cola de mensajes
if (STORS_sendanswer(&answer) != 0) {
  perror("STORS_sendanswer failed");
  continue;
}
}

return 0;
}

static void handle_termination_signal(int sig) {
 // Vaciar la caché antes de salir
 //cache_flush(cache, cache_size);
    cache_flush(cache_entries, MAX_CACHE_SIZE);
 // Cerrar la cola de mensajes y salir del programa
 STORS_close();
 exit(EXIT_SUCCESS);
}

static void handle_cache_flush(int sig) {
    if (sig == SIGALRM) {
    // Vaciar la caché y volver a configurar el temporizador para el próximo vaciado periódico
    //cache_flush(cache, cache_size);
        cache_flush(cache_entries, MAX_CACHE_SIZE);
    setup_cache_flush_timer(30);
    }
}

static void setup_signals() {
 struct sigaction sa;

 // Configurar el manejo de señales de terminación
 sigemptyset(&sa.sa_mask);
 sa.sa_flags = 0;
 sa.sa_handler = handle_termination_signal;
 sigaction(SIGINT, &sa, NULL);
 sigaction(SIGTERM, &sa, NULL);

 // Configurar el manejo de la señal del temporizador para el vaciado periódico de la caché
 sigemptyset(&sa.sa_mask);
 sa.sa_flags = 0;
 sa.sa_handler = handle_cache_flush;
 sigaction(SIGALRM, &sa, NULL);
}

static void setup_cache_flush_timer(int interval_seconds) {
 struct itimerval timer;

 // Configurar el temporizador para que envíe una señal SIGALRM cada interval_seconds segundos
 timer.it_interval.tv_sec = interval_seconds;
 timer.it_interval.tv_usec = 0;
 timer.it_value.tv_sec = interval_seconds;
 timer.it_value.tv_usec = 0;

 setitimer(ITIMER_REAL, &timer, NULL);
}