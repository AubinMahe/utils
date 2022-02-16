#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "utils_visibility.h"

typedef struct { int unused; } * utils_file_list;

DLL_PUBLIC bool utils_file_list_new     ( utils_file_list * files, const char * path );
DLL_PUBLIC bool utils_file_list_contains( utils_file_list files, const char * filename );
DLL_PUBLIC bool utils_file_list_delete  ( utils_file_list * files );

DLL_PUBLIC bool utils_file_get_absolute_path( const char * path, char * target, size_t target_size );
DLL_PUBLIC bool utils_file_build_path( const char * path, const char * filename, char * target, size_t target_size );
DLL_PUBLIC bool utils_file_dir_name( const char * path, char * target, size_t target_size );
DLL_PUBLIC bool utils_file_file_name( const char * path, char * target, size_t target_size );
DLL_PUBLIC bool utils_file_set_extension( const char * path, const char * suffix, char * target, size_t target_size );
DLL_PUBLIC bool utils_file_is_a_directory( const char * path );
DLL_PUBLIC bool utils_file_copy( const char * dest, const char * src );
