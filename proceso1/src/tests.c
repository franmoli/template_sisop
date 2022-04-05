#include "tests.h" 
int run_tests(){

    CU_initialize_registry();

    CU_pSuite tests = CU_add_suite("PROCESO1 Suite",NULL,NULL);
    CU_add_test(tests,"Probar Suma", suma_proceso1);

    CU_pSuite tests_serializar = CU_add_suite("PROCESO1 Suite Serializar",NULL,NULL);
    CU_add_test(tests_serializar,"Probar serialiar tipos simples", prueba_serializar_tipos_simples);
    CU_add_test(tests_serializar,"Probar serialiar listas", prueba_serializar_listas);
    CU_add_test(tests_serializar,"Probar serialiar structs", prueba_serializar_structs);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
void suma_proceso1(){
    CU_ASSERT_EQUAL(2+2, 4);
}

void prueba_serializar_tipos_simples(){

    t_paquete *paquete = NULL;
    paquete = serializar(10, INT, 2, BOOL, 0, CHAR_PTR, "Test", UINT32, 2, U_INT, 2);

    char *test_string = NULL;
    bool test_bool = 1;
    int test_int = 40;
    unsigned int test_u_int = 3;
    uint32_t test_u_int_32 = 4;

    deserializar(paquete, 10,  INT, &test_int, BOOL, &test_bool, CHAR_PTR, &test_string, UINT32, &test_u_int_32, U_INT, &test_u_int);

    CU_ASSERT_EQUAL(test_int, 2);
    CU_ASSERT_EQUAL(test_bool, 0);
    CU_ASSERT_EQUAL(test_u_int, 2);
    CU_ASSERT_EQUAL(test_u_int_32, 2);
    CU_ASSERT_STRING_EQUAL(test_string, "Test");

}

void prueba_serializar_listas(){
    t_list *test_list = list_create(); 
    t_list *test_list2 = list_create(); 
    int *test_1 = malloc(sizeof(int));
    int *test_2 = malloc(sizeof(int));
    int *test_3 = malloc(sizeof(int));
    int *test_4 = malloc(sizeof(int));
    *test_1 = 1; 

    list_add(test_list, test_1);
    list_add(test_list, test_2);
    list_add(test_list, test_3);
    list_add(test_list, test_4);

    t_paquete *test_paquete = serializar(3, LIST, INT, test_list);

    deserializar(test_paquete, 2, test_list2);

    int *deserializado = list_get(test_list2, 1);
    CU_ASSERT_EQUAL(*deserializado, 1);
}

void prueba_serializar_structs(){

}