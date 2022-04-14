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


    char *char_test = "Testing";
    t_paquete *paquete_test = serializar(10, INT, 3, CHAR, 'c', CHAR_PTR, char_test, UINT32, 15, BOOL, 1);

    
    int return_int = 0;
    char return_char = 'a';
    char *return_char_ptr = NULL;
    uint32_t return_uint32 = 0;
    bool return_bool = 0;
    deserializar(paquete_test, 10, INT, &return_int, CHAR, &return_char, CHAR_PTR, &return_char_ptr, UINT32, &return_uint32, BOOL, &return_bool);

    printf("Returned | %d | %c | %s | %d | %d \n", return_int, return_char, return_char_ptr, return_uint32, return_bool);

    // -----------------
    demo_struct *demo_struct_first = malloc(sizeof(demo_struct));
    demo_struct_first->size = 20;
    demo_struct_first->string = "A ver";
    demo_struct_first->otro_int = 25;

    paquete_test = serializar(2, DEMO_STRUCT, demo_struct_first);
    
    demo_struct *demo_struct_second = malloc(sizeof(demo_struct));
    deserializar(paquete_test, 2, DEMO_STRUCT, &demo_struct_second);

    printf("Struct deserializd %d | %s | %d\n", demo_struct_second->size, demo_struct_second->string, demo_struct_second->otro_int);

    t_list *lista_test = list_create();
    demo_struct_first = malloc(sizeof(demo_struct));
    demo_struct_first->size = 20;
    demo_struct_first->string = "A ver 1";
    demo_struct_first->otro_int = 25;

    demo_struct_second = malloc(sizeof(demo_struct));
    demo_struct_second->size = 21;
    demo_struct_second->string = "A ver 2";
    demo_struct_second->otro_int = 26;

    demo_struct *demo_struct_third = malloc(sizeof(demo_struct));
    demo_struct_third->size = 22;
    demo_struct_third->string = "A ver 3";
    demo_struct_third->otro_int = 27;

    list_add(lista_test, demo_struct_first);
    printf("Struct 1 %p\n", demo_struct_first);
    list_add(lista_test, demo_struct_second);
    printf("Struct 2 %p\n", demo_struct_second);
    list_add(lista_test, demo_struct_third);
    printf("Struct 3 %p\n", demo_struct_third);

    printf("List_size %d | %p\n", list_size(lista_test), lista_test);
    

    paquete_test = serializar(3, LIST, DEMO_STRUCT, lista_test);

    t_list *lista2 = NULL;
    deserializar(paquete_test, 2, LIST, &lista2);

    printf("List size %d\n", list_size(lista2));
    demo_struct *demo_structaso;
    for (size_t i = 0; i < list_size(lista2); i++)
    {
        demo_structaso = list_get(lista2, i);
        printf("Deserializado struct %d: Size %d | string %s | otro int %d \n\n",i, demo_structaso->size, demo_structaso->string, demo_structaso->otro_int);
    }
    

    return 0;
}

