#pragma once

#include <stdbool.h>
#include <stddef.h>

#include <utils/utils_visibility.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { unsigned unused; } * utils_prefs;

DLL_PUBLIC bool utils_prefs_load( utils_prefs * prefs, const char * program_name );
DLL_PUBLIC bool utils_prefs_get_path( utils_prefs prefs, const char ** dest );
DLL_PUBLIC bool utils_prefs_get_boolean( utils_prefs prefs, const char * name, bool * value );
DLL_PUBLIC bool utils_prefs_get_ushort( utils_prefs prefs, const char * name, unsigned short * value );
DLL_PUBLIC bool utils_prefs_get_int( utils_prefs prefs, const char * name, int * value );
DLL_PUBLIC bool utils_prefs_get_long( utils_prefs prefs, const char * name, long * value );
DLL_PUBLIC bool utils_prefs_get_string( utils_prefs prefs, const char * name, const char ** value );
DLL_PUBLIC bool utils_prefs_delete( utils_prefs * prefs );

#ifdef __cplusplus
}
#endif
