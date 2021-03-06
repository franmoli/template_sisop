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
    CHAR,
    CHAR_PTR,
    UINT32,
    BOOL,
    LIST,
    DEMO_STRUCT
}t_type;

typedef struct{
    int size;
    char *string;
    int otro_int;
}demo_struct; 

union simple_data_types{
    int int_t;
    char char_ptr;
    uint32_t uint_32_t;
    bool bool_t;
    unsigned int uint_t;
};




//FUNCIONES

char* mi_funcion_compartida();
int iniciar_servidor(char *ip, char *puerto, t_log *logger);
int crear_conexion(char *ip, char* puerto);
int esperar_cliente(int socket_servidor, t_log *logger);
void* serializar_paquete(t_paquete *paquete, int *bytes);
void enviar_paquete(t_paquete *paquete, int socket_cliente);
t_paquete *recibir_paquete(int socket_cliente);


//SERIALIZACIONES
//Utilizacion: serializar(cant de argumentos , datos a serializar)
//             datos a serializar = (TIPO DE DATO, DATO .....)
//             Para mandar lista(LIST, Tipo de la lista , lista) 
//             Para struct hacer las funciones de serializar, deserializar y agregar al enum, como el struct demo 
t_paquete * serializar (int arg_count, ...);

//Serializaciones de tipos basicos
void serializar_int(int dato, void **stream, int *stream_size);
void serializar_char(int dato, void **stream, int *stream_size);
void serializar_char_ptr(char *dato, void **stream, int *stream_size);
void serializar_bool(int dato, void **stream, int *stream_size);
void serializar_uint32(uint32_t dato, void **stream, int *stream_size);
void serializar_void_ptr(void *dato, void **stream, int *stream_size, int dato_size);

void serializar_demo_struct(demo_struct *dato, void **stream, int *stream_size);
void serializar_lista(t_type tipo_de_lista, t_list *lista, void **stream, int *stream_size);


//DESERIALIZACIONES
//Utilizacion: deserializar(paquete, TIPO DE DATO, &DATO)
int deserializar(t_paquete *paquete, int arg_count, ...);
//Deserializaciones de tipos basicos
int deserializar_int(void *stream, int *offset);
char deserializar_char(void *stream, int *offset);
char *deserializar_char_ptr(void *stream, int *offset);
uint32_t deserializar_uint32(void *stream, int *offset);
bool deserializar_bool(void *stream, int *offset);

demo_struct *deserializar_demo_struct(void *stream, int *offset);
t_list *deserializar_lista(void *stream, int *offset);


#endif