#include <utils/utils_map.h>

#include <stdint.h>  // perror
#include <stdio.h>  // perror
#include <stdlib.h> // malloc, realloc, qsort, bsearch
#include <string.h> // memmove

// Astuce pour libérer un pointeur const void * en espérant qu'il ne soit pas "vraiment" const,
// sinon c'est SIGSEGV !
// Ne fonctionne que si les pointeurs sont stockables sur 64 bits
#define CONST_CAST(p)      ((void *)(uint64_t)(p))

typedef struct {
   utils_comparator comparator;
   bool             must_free_keys;
   bool             must_free_values;
   int              trace_flags;
   size_t           count;
   map_pair *       data;
} utils_map_private;

DLL_PUBLIC bool utils_map_new( utils_map * map, utils_comparator comparator, bool must_free_keys, bool must_free_values ) {
   if( map == NULL ) {
      return false;
   }
   utils_map_private * This = malloc( sizeof( utils_map_private ));
   if( This == NULL ) {
      perror( "malloc" );
      return false;
   }
   *map = (utils_map)This;
   This->comparator       = comparator;
   This->must_free_keys   = must_free_keys;
   This->must_free_values = must_free_values;
   This->trace_flags      = 0;
   This->count            = 0;
   This->data             = NULL;
   return true;
}

DLL_PUBLIC bool utils_map_set_trace( utils_map map, int flags ) {
   if( map == NULL ) {
      return false;
   }
   utils_map_private * This = (utils_map_private *)map;
   This->trace_flags = flags;
   return true;
}

DLL_PUBLIC bool utils_map_put( utils_map map, map_key key, map_value value ) {
   if(( map == NULL )||( key == NULL )) {
      return false;
   }
   utils_map_private * This = (utils_map_private *)map;
   map_pair   kvp  = { key, NULL };
   map_pair * res  = bsearch( &kvp, This->data, This->count, sizeof( map_pair ), This->comparator );
   if( res == NULL ) {
      map_pair * data = realloc( This->data, ( This->count + 1 ) * sizeof( map_pair ));
      if( data == NULL ) {
         perror( "realloc" );
         return false;
      }
      if( This->trace_flags & UTILS_MAP_TRACE_ALLOC ) {
         fprintf( stderr, "%s: This->data = %p realloc to %p\n", __func__, (const void *)This->data, (const void *)data );
      }
      This->data = data;
      This->data[This->count].key   = key;
      This->data[This->count].value = value;
      This->count++;
      qsort( This->data, This->count, sizeof( map_pair ), This->comparator );
   }
   else {
      if( This->must_free_keys ) {
		   if( This->trace_flags & UTILS_MAP_TRACE_FREE ) {
		      fprintf( stderr, "%s: free( key = %p )\n", __func__, (const void *)res->key );
		   }
         free( CONST_CAST( res->key ));
      }
      if( This->must_free_values ) {
		   if( This->trace_flags & UTILS_MAP_TRACE_FREE ) {
		      fprintf( stderr, "%s: free( value = %p )\n", __func__, (const void *)res->value );
		   }
         free( CONST_CAST( res->value ));
      }
      res->key   = key;
      res->value = value;
   }
   if( This->trace_flags & UTILS_MAP_TRACE_PUT ) {
      fprintf( stderr, "%s: count = %ld, map_pair {%p, %p} added\n", __func__, This->count, (const void *)key, (const void *)value );
   }
   return true;
}

DLL_PUBLIC bool utils_map_merge( utils_map map, utils_map src ) {
   if(( map == NULL )||( src == NULL )) {
      return false;
   }
   utils_map_private * Src = (utils_map_private *)src;
   for( size_t i = 0; i < Src->count; ++i ) {
      map_pair * p = Src->data + i;
      if( ! utils_map_put( map, p->key, p->value )) {
         return false;
      }
      p->key   = NULL;
      p->value = NULL;
   }
   utils_map_clear( src );
   return true;
}

DLL_PUBLIC bool utils_map_remove( utils_map map, map_key key ) {
   if(( map == NULL )||( key == NULL )) {
      return false;
   }
   utils_map_private * This = (utils_map_private *)map;
   if( This->data == NULL ) {
      return false;
   }
   map_pair   kvp = { key, NULL };
   map_pair * res = bsearch( &kvp, This->data, This->count, sizeof( map_pair ), This->comparator );
   if( res ) {
      if( This->must_free_keys ) {
		   if( This->trace_flags & UTILS_MAP_TRACE_FREE ) {
		      fprintf( stderr, "%s: free( key = %p )\n", __func__, (const void *)res->key );
		   }
         free( CONST_CAST( res->key ));
      }
      if( This->must_free_values ) {
		   if( This->trace_flags & UTILS_MAP_TRACE_FREE ) {
		      fprintf( stderr, "%s: free( value = %p )\n", __func__, (const void *)res->value );
		   }
         free( CONST_CAST( res->value ));
      }
      ssize_t index = res - This->data;
      --This->count;
      if( This->trace_flags & UTILS_MAP_TRACE_REMOVE ) {
         fprintf( stderr, "%s: count = %ld, map_pair {%p, %p} removed\n", __func__, This->count, res->key, res->value );
      }
      memmove( res, res + 1, ( This->count - (size_t)index ) * sizeof( map_pair ));
      return true;
   }
   return false;
}

DLL_PUBLIC bool utils_map_get( utils_map map, map_key key, map_value * value ) {
   if(( map == NULL )||( key == NULL )||( value == NULL )) {
      return false;
   }
   utils_map_private * This = (utils_map_private *)map;
   if( This->data == NULL ) {
      return false;
   }
   map_pair   kvp = { key, NULL };
   map_pair * p   = bsearch( &kvp, This->data, This->count, sizeof( map_pair ), This->comparator );
   if( p == NULL ) {
      return false;
   }
   *value = p->value;
   return true;
}

DLL_PUBLIC bool utils_map_contains( utils_map map, map_key key, bool * result ) {
   if(( map == NULL )||( key == NULL )||( result == NULL )) {
      return false;
   }
   *result = false;
   utils_map_private * This = (utils_map_private *)map;
   if( This->data ) {
      map_pair   kvp = { key, NULL };
      map_pair * res = bsearch( &kvp, This->data, This->count, sizeof( map_pair ), This->comparator );
      *result = ( res != NULL );
   }
   return true;
}

DLL_PUBLIC bool utils_map_get_size( utils_map map, size_t * cardinality ) {
   if(( map == NULL )||( cardinality == NULL )) {
      return false;
   }
   utils_map_private * This = (utils_map_private *)map;
   *cardinality = This->count;
   return true;
}

DLL_PUBLIC bool utils_map_get_keys( utils_map map, map_key keys[], size_t * keys_count ) {
   if(( map == NULL )||( keys_count == NULL )) {
      return false;
   }
   utils_map_private * This = (utils_map_private *)map;
   if(( keys == NULL )||( *keys_count < This->count )) {
      *keys_count = This->count;
      return true;
   }
   for( size_t i = 0; i < This->count; ++i ) {
      keys[i] = This->data[i].key;
   }
   return true;
}

DLL_PUBLIC bool utils_map_get_values( utils_map map, map_value values[], size_t * values_count ) {
   if(( map == NULL )||( values_count == NULL )) {
      return false;
   }
   utils_map_private * This = (utils_map_private *)map;
   if(( values == NULL )||( *values_count < This->count )) {
      *values_count = This->count;
      return true;
   }
   for( size_t i = 0; i < This->count; ++i ) {
      values[i] = This->data[i].value;
   }
   return true;
}

DLL_PUBLIC bool utils_map_get_entries( utils_map map, map_pair pairs[], size_t * pairs_count ) {
   if(( map == NULL )||( pairs_count == NULL )) {
      return false;
   }
   utils_map_private * This = (utils_map_private *)map;
   if(( pairs == NULL )||( *pairs_count < This->count )) {
      *pairs_count = This->count;
      return true;
   }
   for( size_t i = 0; i < This->count; ++i ) {
      pairs[i] = This->data[i];
   }
   return true;
}

DLL_PUBLIC bool utils_map_foreach( utils_map map, utils_map_iterator iter, void * user_context ) {
   if(( map == NULL )||( iter == NULL )) {
      return false;
   }
   utils_map_private * This = (utils_map_private *)map;
   for( size_t i = 0; i < This->count; ++i ) {
      if( ! (*iter)( i, This->data[i], user_context )) {
         break;
      }
   }
   return true;
}

DLL_PUBLIC bool utils_map_clear( utils_map map ) {
   if( map == NULL ) {
      return false;
   }
   utils_map_private * This = (utils_map_private *)map;
   for( size_t i = 0; i < This->count; ++i ) {
      map_pair * res = This->data + i;
      if( This->must_free_keys ) {
		   if( This->trace_flags & UTILS_MAP_TRACE_FREE ) {
		      fprintf( stderr, "%s: free( key = %p )\n", __func__, (const void *)res->key );
		   }
         free( CONST_CAST( res->key ));
      }
      if( This->must_free_values ) {
		   if( This->trace_flags & UTILS_MAP_TRACE_FREE ) {
		      fprintf( stderr, "%s: free( value = %p )\n", __func__, (const void *)res->value );
		   }
         free( CONST_CAST( res->value ));
      }
   }
   if( This->trace_flags & UTILS_MAP_TRACE_CLEAR ) {
      fprintf( stderr, "%s: %ld entr%s removed\n", __func__, This->count, ( This->count > 1 ) ? "ies" : "y" );
   }
   free( This->data );
   This->data  = NULL;
   This->count = 0;
   return true;
}

DLL_PUBLIC bool utils_map_delete( utils_map * map ) {
   if( map && utils_map_clear( *map )) {
      free( *map );
      *map = NULL;
      return true;
   }
   return false;
}
