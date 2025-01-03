#include "khg_dbm/dbm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define COMMAND_MAX_STRLEN 256

#define prompt_string(text, outbuff)            \
do {                                            \
    printf((text));                             \
    fflush(stdout);                             \
    fgets((outbuff), sizeof((outbuff)), stdin); \
    (outbuff)[strlen((outbuff)) - 1] = '\0';    \
} while (0)

#define prompt_int(text, intval)             \
do {                                         \
    char buff[20];                           \
    prompt_string(text, buff);               \
    (intval) = (int) strtol(buff, NULL, 10); \
} while (0)

#define prompt_float(text, floatval) \
do {                                 \
    char buff[20];                   \
    prompt_string(text, buff);       \
    (floatval) = strtof(buff, NULL); \
} while (0)

typedef struct Alumno
{
    char nombre[52]; // 51 + \0
    int ncontrol;
    float promedio;
} Alumno;

static void print_pretty_table(const Alumno *alumno, bool print_header)
{
    if (print_header) {
        puts("+-----------------------------------------------------+------------+----------+");
        puts("|                       Nombre                        | N. Control | Promedio |");
        puts("+-----------------------------------------------------+------------+----------+");
    }

    if (alumno != NULL) {
        printf("| %-51s | % 10d | % 8.1f |\n", alumno->nombre, alumno->ncontrol, alumno->promedio);
        puts("+-----------------------------------------------------+------------+----------+");
    }
}

static void select_print_callback(int64_t key, void *data)
{
    print_pretty_table((Alumno *) data, false);
}

static void print_help_text()
{
    puts(
            "Ayuda de MiniDB:\n\n"
            " Comando        Descripción                                     \n"
            "-------------- -------------------------------------------------\n"
            " exit, salir    Cierra la base de datos y termina el programa.  \n"
            " new, nueva     Crea una nueva base de datos vacía.             \n"
            " open, abrir    Abre una base de datos existente.               \n"
            " dbinfo         Muestra información sobre la base de datos.     \n"
            " select         Buscar un registro y mostrarlo en pantalla.     \n"
            " select *       Mostrar todos los registros en pantalla.        \n"
            " insert         Insertar un registro.                           \n"
            " update         Actualizar un registro existente.               \n"
            " delete         Borrar un registro.                             \n"
    );
}

int main_dbm(void)
{
    dbm_db *db;
    dbm_db_state error;

    char command[COMMAND_MAX_STRLEN];
    char filepath[COMMAND_MAX_STRLEN];

    // Preamble
    printf("MiniDb x%zu bits.\n\n", sizeof(int *) * 8);

    while (1) {
        prompt_string("MiniDb> ", command);

        if (strcmp(command, "exit") == 0) {
            return 0;
        } else if (strcmp(command, "help") == 0) {
            print_help_text();
        } else if (strcmp(command, "new") == 0 || strcmp(command, "nueva") == 0) {
            prompt_string("Path: ", filepath);
            printf("Creando base de datos... ");
            fflush(stdout);

            error = dbm_create(&db, filepath, sizeof(Alumno));
            if (error != DBM_OK) {
                printf("Fatal error: %s\n", dbm_error_get_str(error));
                exit(1);
            }

            puts("Ok!\n");
            fflush(stdout);
        } else if (strcmp(command, "open") == 0 || strcmp(command, "abrir") == 0) {
            prompt_string("Path: ", filepath);
            error = dbm_open(&db, filepath);

            if (error != DBM_OK) {
                printf("Fatal error: %s\n", dbm_error_get_str(error));
                exit(1);
            }

            puts("La base de datos se ha abierto correctamente.\n");
        } else {
            printf(
                    "Error: commando \"%s\" no es valido.\n"
                    "Debe crear una nueva base de datos utilizando el comando 'nueva'\n"
                    "o abrir una base de datos existente con el comando 'abrir'.\n"
                    "Para terminar el programa, escriba el comando 'salir'.\n\n",
                    command
            );

            continue;
        }

        break;
    }

    const char *db_name = filepath;
    for (int64_t i = (int64_t) strlen(filepath) - 1; i >= 0; i--) {
        if (filepath[i] == '\\' || filepath[i] == '/') {
            db_name = filepath + i + 1;
            break;
        }
    }

    Alumno alumno;
    while (1) {
        printf("MiniDb\\%s> ", db_name);
        fflush(stdout);
        fgets(command, sizeof(command), stdin);
        command[strlen(command) - 1] = '\0';

        if (strcmp(command, "salir") == 0 || strcmp(command, "exit") == 0) {
            break;
        } else if (strcmp(command, "help") == 0) {
            print_help_text();
        } else if (strcmp(command, "dbinfo") == 0) {
            dbm_db_info info;
            dbm_get_info(db, &info);

            puts("== DATABASE INFO ==");
            printf("Physical name  : %s\n", db_name);
            printf("Data Size      : %zu\n", info.data_size);
            printf("Row Count      : %zu\n", info.row_count);
            printf("Free Count     : %zu\n", info.free_count);
            printf("Db Data Size   : %zu\n", info.data_size * info.row_count);
            puts("");
        } else if (strcmp(command, "select") == 0) {
            int ncontrol;
            prompt_int("N. control: ", ncontrol);
            puts("");

            error = dbm_select(db, ncontrol, &alumno);
            if (error != DBM_OK) {
                printf("Error: %s\n\n", dbm_error_get_str(error));
                continue;
            }

            print_pretty_table(&alumno, true);
            printf("\n");
        } else if (strcmp(command, "select *") == 0) {
            print_pretty_table(NULL, true);
            dbm_select_all(db, select_print_callback);
        } else if (strcmp(command, "insert") == 0) {
            prompt_string("Nombre[51] : ", alumno.nombre);
            prompt_int("N. control : ", alumno.ncontrol);
            prompt_float("Promedio   : ", alumno.promedio);

            error = dbm_insert(db, alumno.ncontrol, &alumno);
            if (error != DBM_OK) {
                printf("Error: %s\n\n", dbm_error_get_str(error));
                continue;
            }

            puts("Tupla insertada correctamente\n");
        } else if (strcmp(command, "update") == 0) {
            prompt_string("Nombre[51] : ", alumno.nombre);
            prompt_int("N. control : ", alumno.ncontrol);
            prompt_float("Promedio   : ", alumno.promedio);

            error = dbm_update(db, alumno.ncontrol, &alumno);
            if (error != DBM_OK) {
                printf("Error: %s\n\n", dbm_error_get_str(error));
                continue;
            }

            puts("Tupla actualizada correctamente\n");
        } else if (strcmp(command, "delete") == 0) {
            int ncontrol;
            prompt_int("N. control: ", ncontrol);

            error = dbm_delete(db, ncontrol);
            if (error != DBM_OK) {
                printf("Error: %s\n\n", dbm_error_get_str(error));
                continue;
            }

            puts("Tupla eliminada correctamente\n");
        } else {
            if (command[0] != '\0') {
                puts("Error: comando no reconocido.\n");
            }
        }
    }

    dbm_close(&db);
    printf("Programa finalizado\n");
    return 0;
}
