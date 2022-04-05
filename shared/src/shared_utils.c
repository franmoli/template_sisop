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

t_paquete * serializar (int arg_count, ...){

    //Declaraciones de variables
    void *stream = NULL;
    int size = 0;
    int offset = 0;
    int added_size = 0;

    //Declaracion de parametros posibles
    int param_i = 0;
    unsigned int param_un_i = 0; 
    char *param_s = NULL;
    uint32_t param_ui = 0;
    bool param_b = false;
    t_list *param_l = NULL; 
    t_paquete *paquete_aux = malloc(sizeof(t_paquete));
    paquete_aux->buffer = malloc(sizeof(t_buffer));
    t_type tipo_de_lista = INT;
    void *list_elem = NULL;

    va_list valist;
    va_start(valist, arg_count);
    int lista_size = 0;

    for (int i = 0; i < arg_count; i += 2) {
        //Primer parametro es el tipo de dato que se quiere serializar
        t_type tipo = va_arg(valist, t_type);

        switch(tipo){
            case INT:
                param_i = va_arg(valist, int);
                added_size = sizeof(int);

                serializar_single(&stream, &param_i, &size, added_size, &offset);

                break;
            case CHAR_PTR:       
                param_s = va_arg(valist, char*);
                int string_length = strlen(param_s) +1;
                added_size = sizeof(char) * sizeof(char)*string_length;

                serializar_single(&stream, &string_length, &size, sizeof(int), &offset);   

                serializar_single(&stream, param_s, &size, added_size, &offset);                

                break;
            case UINT32:
                param_ui = va_arg(valist, uint32_t);
                added_size = sizeof(uint32_t);

                serializar_single(&stream, &param_ui, &size, added_size, &offset);

                break;
            case BOOL:
                param_b = va_arg(valist, int);
                added_size = sizeof(bool);

                serializar_single(&stream, &param_b, &size, added_size, &offset);

                break;
            case LIST:
                tipo_de_lista = va_arg(valist, t_type); 
                param_l = va_arg(valist, t_list*);
                //Se trae el tipo de lista y se incrementa i por el va_arg extra
                i++;
                lista_size = list_size(param_l);
                serializar_single(&stream,&lista_size,&size ,sizeof(int),  &offset);
                
                for(int j = 0; j < list_size(param_l); j++){
                    
                    //Traigo un elemento de la lista y lo serializo recursivamente
                    list_elem = list_get(param_l, j);
                    paquete_aux = serializar(2, tipo_de_lista, list_elem);
                    added_size = paquete_aux->buffer->size;

                    serializar_single(&stream, paquete_aux->buffer->stream, &size, added_size, &offset);
                
                }
                break;
            case U_INT:
                param_un_i = va_arg(valist, unsigned int);
                added_size = sizeof(unsigned int);

                serializar_single(&stream, &param_un_i, &size, added_size, &offset);

                break;
        }
    }
    
    //Se libera la lista de argumentos
    va_end(valist);

    //Se arma el paquete
    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));

    paquete->buffer->size = size;
    paquete->buffer->stream = stream;
    paquete->codigo_operacion = CODIGO_INDEFINIDO;
    printf("Paquete size %d\n", size);
    return paquete;
}

void serializar_single (void **stream, void *elem, int *stream_size, int added_size, int *offset){

    //Expandir espacio en buffer
    *stream_size = *stream_size + added_size;
    *stream = realloc(*stream, *stream_size);
    printf("Added size %d\n", added_size);
    //copiar contenido a buffer
    memcpy(*stream + *offset, elem, added_size);
    
    //correr offset
    *offset += added_size;

    return;
}

void deserializar_single (void *stream, void *elem, int size, int *offset){    

    //copiar contenido a variable
    memcpy(elem, stream + *offset, size);
    
    //correr offset
    *offset += size;

    return;
}

int deserializar(t_paquete *paquete, int arg_count, ...){

    int size = 0;
    void *param = NULL;
    void *stream = paquete->buffer->stream;
    int offset = 0;
    char **param_char = NULL;
    va_list valist;
    va_start(valist, arg_count);
    
    t_paquete *paquete_aux= NULL;

    t_list *param_l = NULL; 
    t_type tipo_de_lista = INT;
    void *list_elem = NULL;
    
    
    for (int i = 0; i < arg_count; i += 2){
        //Primer parametro es el tipo de dato que se quiere deserializar
        t_type tipo = va_arg(valist, t_type);
        switch(tipo){
            case INT:
                param = va_arg(valist, void*);
                size = sizeof(int);
                deserializar_single(stream, param, size, &offset);

                break;
            case CHAR_PTR:  
                param_char = va_arg(valist, char**);
                //Primero traigo el tamanio del string
                size = sizeof(int);
                deserializar_single(stream, &size, size, &offset);
                //Con el tamanio del string asigno memoria y traigo variable
                *param_char = realloc(*param_char, size);
                deserializar_single(stream, *param_char, size, &offset);              

                break;
            case UINT32:
                param = va_arg(valist, void*);
                size = sizeof(uint32_t);
                deserializar_single(stream, param, size, &offset);

                break;
            case BOOL:
                param = va_arg(valist, void*);
                size = sizeof(bool);
                deserializar_single(stream, param, size, &offset);

                break;
            case U_INT:
                param = va_arg(valist, void*);
                size = sizeof(unsigned int);
                deserializar_single(stream, param, size, &offset);

                break;
            case LIST:
            
                tipo_de_lista = va_arg(valist, t_type); 
                param_l = va_arg(valist, t_list*);
                //Se trae el tipo de lista y se incrementa i por el va_arg extra
                i++;
                int tamanio_lista;
                deserializar_single(stream, &tamanio_lista, sizeof(int), &offset);
                printf("Tamaño de la lista recibida %d", tamanio_lista);
                for(int j = 0; j < tamanio_lista; j++){
                printf("pass\n");
                    paquete_aux = malloc(sizeof(t_paquete));
                    paquete_aux->buffer = malloc(sizeof(t_buffer));
                    paquete_aux->buffer->stream = stream + offset;
                    list_elem = NULL;
                    //Traigo un elemento de la lista y lo serializo recursivamente
                    offset += deserializar(paquete_aux,2,tipo_de_lista,&list_elem);
                    list_add(param_l,list_elem);
                }
                break;
        }
    }
    return offset;
}