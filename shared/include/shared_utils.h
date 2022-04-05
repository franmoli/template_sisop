#ifndef SHARED_UTILS_H
#define SHARED_UTILS_H

#include <stdio.h>
#include <commons/log.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/collections/list.h>
#include <stdarg.h>
#include <string.h>

//Estructuras
typedef enum {
    CODIGO_INDEFINIDO,
	NUEVO_CLIENTE,
    CLIENTE_DESCONECTADO
} op_code;

typedef struct {
    uint32_t size;
    void *stream;
} t_buffer;

typedef struct {
    op_code codigo_operacion;
    t_buffer *buffer;
} t_paquete;

typedef enum {
    INT,
    CHAR_PTR,
    UINT32,
    BOOL,
    LIST,
	U_INT,
    STRUCT
}t_type;

typedef struct{
    int size;
    t_type tipos[];
}t_custom_struct;


//FUNCIONES

char* mi_funcion_compartida();
int iniciar_servidor(char *ip, char *puerto, t_log *logger);
int crear_conexion(char *ip, char* puerto);
int esperar_cliente(int socket_servidor, t_log *logger);
void* serializar_paquete(t_paquete *paquete, int *bytes);
void enviar_paquete(t_paquete *paquete, int socket_cliente);
t_paquete *recibir_paquete(int socket_cliente);
//Utilizacion: serializar(Codigo de operacion , cant de argumentos (2 * cantidad de datos a serializar) , datos a serializar)
//             datos a serializar = (TIPO DE DATO (segun enum t_type) , DATO .....)
//             para enviar una lista será = (LIST, tipo de datos que contiene la lista , PUNTERO A LA LISTA) / las listas ponerlas siempre al final para usar el deserializar
t_paquete * serializar (int arg_count, ...);
void serializar_single (void **stream, void *elem, int *stream_size, int elem_size, int *offset);
//Utilizacion: deserializar(paquete, TIPO DE DATO(segun el enum t_type), &DATO BUSCADO) para strings se pasa con & ej: char *string_objetivo; deserializar(paquete, CHAR_PTR, &string_objetivo)
int deserializar(t_paquete *paquete, int arg_count, ...);
void deserializar_single (void *stream, void *elem, int size, int *offset);



#endif