#pragma once

#include <stdbool.h>

#include <utils/utils_visibility.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
   utils_cli_no_argument,
   utils_cli_boolean,
   utils_cli_int16,
   utils_cli_uint16,
   utils_cli_int32,
   utils_cli_uint32,
   utils_cli_int64,
   utils_cli_uint64,
   utils_cli_float,
   utils_cli_double,
   utils_cli_string,
} utils_cli_type;

typedef struct { int unused; } * utils_cli_arg;

/**
 * @brief parse la ligne de commande et renseigne des variables dont les adresses sont fournies.
 * @param options ADT permettant de libérer la mémoire par utils_cli_delete()
 * @param argc    le nombre d'argument de la ligne de commande
 * @param argv    les arguments de la ligne de commande
 * Les autres paramètres sont par groupes de 3 :<dl>
 * <dt>utils_cli_type</dt><dd>le type de la valeur de l'argument, utils_cli_no_argument quand il n'y a pas d'argument</dd>
 * <dt>const char * default_value</dt><dd>indique une valeur par défaut, rendant ainsi l'argument optionnel,
 * si elle est nulle l'argument est obligatoire</dd>
 * <dt>void * value</dt><dd>un pointeur vers la variable qui prendra la valeur de l'argument ou recevra 'true'
 * pour les argument sans valeur</dd>
 * </dl>
 * @return true quand l'analyse de la ligne de commande est conforme à la spécification fournie, false sinon.
 */
DLL_PUBLIC bool utils_cli_new   ( utils_cli_arg * options, int argc, char * argv[], ... );
DLL_PUBLIC bool utils_cli_delete( utils_cli_arg * options );

#ifdef __cplusplus
}
#endif
