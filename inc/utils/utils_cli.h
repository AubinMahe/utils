#pragma once

#include <stdbool.h>

#include <utils/utils_visibility.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
   utils_cli_STRING,
   utils_cli_USHORT,
} utils_cli_type;

typedef struct utils_cli_arg_s {
   const char *             name;
   utils_cli_type           type;
   bool                     mandatory;
   void *                   value;
   struct utils_cli_arg_s * next;
} * utils_cli_arg;

DLL_PUBLIC bool utils_cli_new   ( utils_cli_arg * options, int argc, char * argv[], ... );
DLL_PUBLIC bool utils_cli_delete( utils_cli_arg * options );

#ifdef __cplusplus
}
#endif
