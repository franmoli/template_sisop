#include "shared_utils.h"

char* mi_funcion_compartida(){
    return "Hice uso de la shared!";
}

/* Utilizar esta función para levantar el servidor */
int iniciar_servidor(char *ip, char *puerto, t_log *logger) {
	int socket_servidor;
    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip, puerto, &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next) {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            continue;
		}

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            continue;
        }
        break;
    }

	int error = listen(socket_servidor, SOMAXCONN);
    freeaddrinfo(servinfo);
	if(error != -1){
    	log_info(logger, "Componente preparado para recibir conexiones con socket %d", socket_servidor);
    	return socket_servidor;
	}else{
		log_error(logger, "Se produjo un error en la creacion del servidor\n");
		sleep(1);
		return iniciar_servidor(ip, puerto, logger);
	}
}

/* Utilizar esta función para conectarse a otro servidor */
int crear_conexion(char *ip, char *puerto) {
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1) {
		freeaddrinfo(server_info);	
		return -1;
	}
	freeaddrinfo(server_info);
	return socket_cliente;
}

/* Utilizar esta función cuando es necesario esperar la conexión de un cliente para continuar la ejecución */
int esperar_cliente(int socket_servidor, t_log* logger) {
	struct sockaddr_in dir_cliente;
	socklen_t tam_direccion = sizeof(struct sockaddr_in);
	
	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);
	if(socket_cliente != -1) {
		log_info(logger, "Se conecto un cliente con socket %d", socket_cliente);
	}
	return socket_cliente;
}

/* Utilizar esta función para serializar el paquete antes de enviarlo */
void *serializar_paquete(t_paquete *paquete, int *bytes) {
	void *magic = malloc(*bytes);
	int desplazamiento = 0;
	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(op_code));
	desplazamiento+= sizeof(op_code);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(u_int32_t));
	desplazamiento+= sizeof(u_int32_t);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

/* Utilizar esta función para enviar un paquete */
void enviar_paquete(t_paquete *paquete, int socket_cliente) {
	int bytes = paquete->buffer->size + 2 * sizeof(int);
	printf("SOCKET: %d", socket_cliente);
	//printf("Paquete a enviar: opcode %d | buffer size %d | dest %d\n", paquete->codigo_operacion, paquete->buffer->size, socket_cliente);
	void *a_enviar = serializar_paquete(paquete, &bytes);
	int error = send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	if(paquete->buffer->size){
		free(paquete->buffer->stream);
	}
	free(paquete->buffer);
	free(paquete);
	if(error == -1){
		printf("Hubo un error en el envio\n");
	}
}

/* Utilizar esta función para recibir un paquete */
t_paquete* recibir_paquete(int socket_cliente) {

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));

	recv(socket_cliente, &(paquete->codigo_operacion), sizeof(uint32_t), MSG_WAITALL);
	
	if(paquete->codigo_operacion <= 0 || paquete->codigo_operacion > 100){
		printf("El cliente se desconectó forzosamente...\n");
		paquete->codigo_operacion = CLIENTE_DESCONECTADO;
		paquete->buffer->size = 0;
        paquete->buffer->stream = NULL;
		return paquete;
	}
	recv(socket_cliente, &(paquete->buffer->size), sizeof(uint32_t), 0);
	if(paquete->buffer->size){
		paquete->buffer->stream = malloc(paquete->buffer->size);
		recv(socket_cliente, paquete->buffer->stream, paquete->buffer->size, 0);
	}

	return paquete;
}

// SERIALIZACIONES

/* Funcion general de serializar acá incluir nuevos structs*/
t_paquete * serializar (int arg_count, ...){

    //Declaraciones de variables
    void *stream = NULL;
    int stream_size = 0;

    
    t_type list_type;
    t_list *lista;

    //Inicializacion de lista de argumentos
    va_list valist;
    va_start(valist, arg_count);

    for (int i = 0; i < arg_count; i += 2) {
        //Primer parametro es el tipo de dato que se quiere serializar
        t_type tipo = va_arg(valist, t_type);
        switch(tipo){
            case INT:
                serializar_int(va_arg(valist, int), &stream, &stream_size);
            break;
            case CHAR:
                serializar_char(va_arg(valist, int), &stream, &stream_size);
            break;
            case CHAR_PTR:       
                serializar_char_ptr(va_arg(valist, char*), &stream, &stream_size);
            break;
            case UINT32:
                serializar_uint32(va_arg(valist, uint32_t), &stream, &stream_size);
            break;
            case BOOL:
                serializar_bool(va_arg(valist, int), &stream, &stream_size);
            break;
            case LIST:
                list_type = va_arg(valist, t_type);
                printf("List type %d\n", list_type);
                lista = va_arg(valist, t_list*);
                serializar_lista(list_type, lista, &stream, &stream_size);
                //  se incrementa i por el va_arg extra
                i++;
                // tipo_de_lista = va_arg(valist, t_type); 
                // param_l = va_arg(valist, t_list*);
                // lista_size = list_size(param_l);
                // serializar_single(&stream,&lista_size,&size ,sizeof(int),  &offset);
                
                // for(int j = 0; j < list_size(param_l); j++){
                    
                //     //Traigo un elemento de la lista y lo serializo recursivamente
                //     list_elem = list_get(param_l, j);
                //     paquete_aux = serializar(2, tipo_de_lista, list_elem);
                //     added_size = paquete_aux->buffer->size;

                //     serializar_single(&stream, paquete_aux->buffer->stream, &size, added_size, &offset);
                
                // }
            break;
            case DEMO_STRUCT:
                serializar_demo_struct(va_arg(valist, demo_struct *), &stream, &stream_size);
            break;
            
        }   
    }
    
    //Se libera la lista de argumentos
    va_end(valist);

    //Se arma el paquete
    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));

    paquete->buffer->size = stream_size;
    paquete->buffer->stream = stream;
    paquete->codigo_operacion = CODIGO_INDEFINIDO;

    return paquete;
}

void serializar_int(int dato, void **stream, int *stream_size){

    //Expandir espacio en buffer
    int added_size = sizeof(int);
    *stream = realloc(*stream, *stream_size + added_size);

    //Se copia a memoria a partir del ultimo tamaño que tenía antes de ser reallocado
    memcpy(*stream + *stream_size, &dato, added_size);

    //Actualizar offset
    *stream_size += added_size;

    printf("Serializd %d\n", dato);
    return;
}
void serializar_char(int dato, void **stream, int *stream_size){
    //Expandir espacio en buffer
    int added_size = sizeof(char);
    *stream = realloc(*stream, *stream_size + added_size);

    //Se copia a memoria a partir del ultimo tamaño que tenía antes de ser reallocado
    memcpy(*stream + *stream_size, &dato, added_size);

    //Actualizar offset
    *stream_size += added_size;
    printf("Serialized char  %c\n", dato);

    return;
}
void serializar_char_ptr(char *dato, void **stream, int *stream_size){

    int string_length = strlen(dato) + 1;

    //Expandir espacio en buffer
    int strlen_added_size = sizeof(int);
    int string_added_size = sizeof(char) * string_length;
    *stream = realloc(*stream, *stream_size + string_added_size + strlen_added_size);

    //Se copia a memoria a partir del ultimo tamaño que tenía antes de ser reallocado | primero un int del tamaño del string | despues el string en si
    memcpy(*stream + *stream_size, &string_length, strlen_added_size);
    memcpy(*stream + *stream_size + strlen_added_size, dato, string_added_size);
    
    //Actualizar offset
    *stream_size += string_added_size + strlen_added_size;

    printf("Serializd %s\n", dato);

    return;
}
void serializar_uint32(uint32_t dato, void **stream, int *stream_size){

    //Expandir espacio en buffer
    int added_size = sizeof(uint32_t);
    *stream = realloc(*stream, *stream_size + added_size);

    //Se copia a memoria a partir del ultimo tamaño que tenía antes de ser reallocado
    memcpy(*stream + *stream_size, &dato, added_size);

    //Actualizar offset
    *stream_size += added_size;
    printf("Serialized uint32  %d\n", dato);

    return;
}
void serializar_bool(int dato, void **stream, int *stream_size){

    //Expandir espacio en buffer
    int added_size = sizeof(bool);
    *stream = realloc(*stream, *stream_size + added_size);

    //Se copia a memoria a partir del ultimo tamaño que tenía antes de ser reallocado
    memcpy(*stream + *stream_size, &dato, added_size);

    //Actualizar offset
    *stream_size += added_size;
    return;
}
void serializar_void_ptr(void *dato, void **stream, int *stream_size, int dato_size){
    //Expandir espacio en buffer
    *stream = realloc(*stream, *stream_size + dato_size);

    //Se copia a memoria a partir del ultimo tamaño que tenía antes de ser reallocado
    memcpy(*stream + *stream_size, dato, dato_size);

    //Actualizar offset
    *stream_size += dato_size;
    return;
}
void serializar_lista(t_type tipo_de_lista, t_list *lista, void **stream, int *stream_size){
    //Paquetes auxiliares para serializar listas
    t_paquete *paquete_aux = malloc(sizeof(t_paquete));
    paquete_aux->buffer = malloc(sizeof(t_buffer));
    void *list_elem = NULL;

    printf("Type %d | PTR %p \n", tipo_de_lista, lista);

    //Pongo primero el tamaño de la lista
    serializar_int(list_size(lista), stream, stream_size);
    //Despues el tipo de la lista
    serializar_int(tipo_de_lista, stream, stream_size);

    //Despues el contenido de la lista
    for(int i = 0; i < list_size(lista); i++){
                    
        //Traigo un elemento de la lista y lo serializo a un paquete auxiliar
        list_elem = list_get(lista, i);
        paquete_aux = serializar(2, tipo_de_lista, list_elem);

        //El stream con el dato serializado lo paso como un void * y lo agrego al stream
        serializar_void_ptr(paquete_aux->buffer->stream, stream, stream_size, paquete_aux->buffer->size);
    }
}
void serializar_demo_struct(demo_struct *dato, void **stream, int *stream_size){
    printf("Serializando struct %p\n", dato);
    serializar_int(dato->size, stream, stream_size);
    serializar_char_ptr(dato->string, stream, stream_size);
    serializar_int(dato->otro_int, stream, stream_size);
    return;
}

// DESERIALIZACIONES 
/* Funcion general de dessserializar, aca incluir los nuevos structs*/
int deserializar(t_paquete *paquete, int arg_count, ...){

    //Datos recibidos
    void *stream = paquete->buffer->stream;
    int offset = 0;

    //Tipos de datos posibles
    int *param_int = NULL;
    char *param_char = NULL;
    char **param_char_ptr = NULL;
    uint32_t *param_uint32 = NULL;
    bool *param_bool = NULL;
    demo_struct **param_demo_struct = NULL;
    t_list **param_list = NULL;


    //Inicializacion de la lista de argumentos
    va_list valist;
    va_start(valist, arg_count);
    
    
    for (int i = 0; i < arg_count; i += 2){
        //Primer parametro es el tipo de dato que se quiere deserializar
        t_type tipo = va_arg(valist, t_type);
        switch(tipo){
            case INT:
                param_int = va_arg(valist, int*);
                *param_int = deserializar_int(stream, &offset);
            break;  
            case CHAR:
                param_char = va_arg(valist, char*);
                *param_char = deserializar_char(stream, &offset);
            break;
            case CHAR_PTR:  
                param_char_ptr = va_arg(valist, char**);
                *param_char_ptr = deserializar_char_ptr(stream, &offset);
            break;
            case UINT32:
                param_uint32 = va_arg(valist, uint32_t*);
                *param_uint32 = deserializar_uint32(stream, &offset);
            break;
            case BOOL:
                param_bool = va_arg(valist, bool*);
                *param_bool = deserializar_bool(stream, &offset);
            break;
            case LIST:
                param_list = va_arg(valist, t_list**);
                *param_list = deserializar_lista(stream, &offset);
            break;
            case DEMO_STRUCT:
                param_demo_struct = va_arg(valist, demo_struct **);
                *param_demo_struct = deserializar_demo_struct(stream, &offset);
            break;
        }
    }

    return offset;
}
int deserializar_int(void *stream, int *offset){
    
    int dato = 0;

    memcpy(&dato, stream + *offset, sizeof(int));

    *offset += sizeof(int);

    return dato;
}
char deserializar_char(void *stream, int *offset){
    
    char dato = 0;

    memcpy(&dato, stream + *offset, sizeof(char));

    *offset += sizeof(char);

    return dato;
}
char *deserializar_char_ptr(void *stream, int *offset){
    
    char *dato = NULL;
    int stringlen = 0;

    //Copiar tamaño del string
    memcpy(&stringlen, stream + *offset, sizeof(int));

    //allocar memoria y copiar string
    dato = malloc(sizeof(char) * stringlen);
    memcpy(dato, stream + *offset + sizeof(int), sizeof(char) * stringlen);

    *offset += sizeof(char) * stringlen + sizeof(int);

    return dato;
}
uint32_t deserializar_uint32(void *stream, int *offset){
    
    uint32_t dato = 0;

    memcpy(&dato, stream + *offset, sizeof(uint32_t));

    *offset += sizeof(uint32_t);

    return dato;
}
bool deserializar_bool(void *stream, int *offset){
    
    int dato = 0;

    memcpy(&dato, stream + *offset, sizeof(bool));

    *offset += sizeof(bool);

    return dato;
}
t_list *deserializar_lista(void *stream, int *offset){
    
    int list_size = deserializar_int(stream, offset);
    t_type list_type = deserializar_int(stream, offset);

    t_list *lista = list_create();
    void *dato = NULL;
    t_paquete *paquete_aux = malloc(sizeof(t_paquete));
    paquete_aux->buffer = malloc(sizeof(t_buffer));
    
    for(int i = 0; i < list_size; i++){
        paquete_aux->buffer->stream = stream + *offset;
        //Traigo un elemento de la lista y lo serializo recursivamente
        *offset += deserializar(paquete_aux,2,list_type,&dato);
        list_add(lista,dato);
        dato = NULL;
    }
    
    return lista;
}
demo_struct *deserializar_demo_struct(void *stream, int *offset){
    
    demo_struct *struct_deserializado = malloc(sizeof(demo_struct));
    struct_deserializado->size = deserializar_int(stream, offset);
    struct_deserializado->string = deserializar_char_ptr(stream, offset);
    struct_deserializado->otro_int = deserializar_int(stream, offset);

    return struct_deserializado; 
}