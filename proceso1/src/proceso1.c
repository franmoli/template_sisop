#include "proceso1.h"

typedef struct {
    uint32_t first;
    char *second;
    int third;
} test_t;

typedef enum {
	INT2 = sizeof(int),
    CHAR2 = sizeof(char) 
} test2;

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

    
    t_list *test_list = list_create(); 
    t_list *test_list2 = list_create(); 
    int *test_1 = malloc(sizeof(int));
    int *test_2 = malloc(sizeof(int));
    int *test_3 = malloc(sizeof(int));
    int *test_4 = malloc(sizeof(int));
    *test_1 = 1; 
    *test_2 = 2; 
    *test_3 = 3; 
    *test_4 = 4; 

    list_add(test_list, test_1);
    list_add(test_list, test_2);
    list_add(test_list, test_3);
    list_add(test_list, test_4);

    
    t_paquete *test_paquete = serializar(3, LIST, INT, test_list);
    
    deserializar(test_paquete, 3, LIST, INT, test_list2);
    list_size(test_list2);
    int *deserializado_1 = list_get(test_list2, 0);
    int *deserializado_2 = list_get(test_list2, 1);
    int *deserializado_3 = list_get(test_list2, 2);
    int *deserializado_4 = list_get(test_list2, 3);

    printf("deser : %d | %d | %d | %d", *deserializado_1, *deserializado_2, *deserializado_3, *deserializado_4);

}