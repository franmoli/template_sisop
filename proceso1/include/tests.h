#ifndef TESTS_H
#define TESTS_H
#include <CUnit/Basic.h>
#include "shared_utils.h"
#include "proceso1.h"

int run_tests();
void suma_proceso1();
void prueba_serializar_tipos_simples();
void prueba_serializar_listas();
void prueba_serializar_structs();

#endif