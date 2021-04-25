#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "utils_comparator.h"
#include "utils_visibility.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef const void * map_key;
typedef const void * map_value;

typedef struct {
   map_key   key;
   map_value value;
} map_pair;

typedef struct { unsigned unused; } * utils_map;

typedef bool (* utils_map_iterator)( size_t index, map_pair pair, void * user_context );

#define UTILS_MAP_TRACE_ALLOC  0x01
#define UTILS_MAP_TRACE_FREE   0x02
#define UTILS_MAP_TRACE_PUT    0x04
#define UTILS_MAP_TRACE_REMOVE 0x04
#define UTILS_MAP_TRACE_CLEAR  0x10

DLL_PUBLIC bool utils_map_new        ( utils_map * map, utils_comparator comparator, bool must_free_keys, bool must_free_values );
DLL_PUBLIC bool utils_map_set_trace  ( utils_map   map, int flags );
DLL_PUBLIC bool utils_map_put        ( utils_map   map, map_key key, map_value value );
DLL_PUBLIC bool utils_map_merge      ( utils_map   map, utils_map src );
DLL_PUBLIC bool utils_map_remove     ( utils_map   map, map_key key );
DLL_PUBLIC bool utils_map_get        ( utils_map   map, map_key key, map_value * value );
DLL_PUBLIC bool utils_map_contains   ( utils_map   map, map_value data, bool * result );
DLL_PUBLIC bool utils_map_get_size   ( utils_map   map, size_t * cardinality );
DLL_PUBLIC bool utils_map_get_keys   ( utils_map   map, map_key keys[], size_t * keys_count );
DLL_PUBLIC bool utils_map_get_values ( utils_map   map, map_value values[], size_t * values_count );
DLL_PUBLIC bool utils_map_get_entries( utils_map   map, map_pair pairs[], size_t * pairs_count );
DLL_PUBLIC bool utils_map_foreach    ( utils_map   map, utils_map_iterator iter, void * user_context );
DLL_PUBLIC bool utils_map_clear      ( utils_map   map );
DLL_PUBLIC bool utils_map_delete     ( utils_map * map );

#ifdef __cplusplus
}
#endif
