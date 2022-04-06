#include "proceso1.h"

typedef struct {
    uint32_t first;
    char second;
    char *third;
} test_t; 

typedef enum {
	INT2 = sizeof(int),
    CHAR2 = sizeof(char) 
} test2;

union test_union_types{
    int int_t;
    char *char_ptr;
};

void probandaso(int size, int arr[]){
    for (int i = 0; i < size; i++)
       printf(" %d", arr[i]);
    putchar('\n');
}

int testing_other_shit(){
    return 2;
}

int main(int argc, char ** argv){

    if(argc > 1 && strcmp(argv[1],"-test")==0)
        return run_tests();

    
    // test_t testeando = {1, 'c', "algo"};
    // printf("Testeando %d | %c | %s\n", testeando.first, testeando.second, testeando.third);
    // t_custom_struct custom ;
    // custom.size = 3;
    // custom.tipos = calloc(3, sizeof(t_type));
    // custom.tipos[0] = INT;
    // custom.tipos[1] = INT;
    // custom.tipos[2] = CHAR_PTR;
    // t_paquete *test_package = serializar(3, STRUCT, &testeando, custom);
    
    // int test =0;
    // char *string_desrlzd = malloc(sizeof(char)*5);

    // memcpy(&test, test_package->buffer->stream + sizeof(int), sizeof(char));
    // memcpy(string_desrlzd, test_package->buffer->stream + sizeof(int) + sizeof(char), sizeof(char) * 5);
    // printf("deserializd %c\n", test);
    // printf("deserializd %s\n", string_desrlzd);




    return 0;
}

