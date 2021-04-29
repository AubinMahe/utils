#include <utils/utils_set.h>

#include <stdint.h>
#include <stdlib.h> // qsort, bsearch
#include <string.h> // memset

// Astuce pour libérer un pointeur const void * en espérant qu'il ne soit pas "vraiment" const,
// sinon c'est SIGSEGV !
// Ne fonctionne que si les pointeurs sont stockables sur 64 bits
#define CONST_CAST(p)      ((void *)(uint64_t)(p))

typedef struct {
   utils_comparator comparator;
   size_t           count;
   const void **    data;
} utils_set_private;

DLL_PUBLIC bool utils_set_new( utils_set * set, utils_comparator comparator ) {
   if( set == NULL ) {
      return false;
   }
   utils_set_private * This = malloc( sizeof( utils_set_private ));
   if( This == NULL ) {
      return false;
   }
   memset( This, 0, sizeof( utils_set_private ));
   This->comparator = comparator;
   *set = (void *)This;
   return true;
}

DLL_PUBLIC bool utils_set_add( utils_set set, const void * item ) {
   if( set == NULL ) {
      return false;
   }
   utils_set_private * This = (utils_set_private *)set;
   const void ** data = realloc( This->data, ( 1 + This->count ) * sizeof( const void * ));
   if( data == NULL ) {
      return false;
   }
   This->data = data;
   This->data[This->count] = item;
   ++This->count;
   qsort( This->data, This->count, sizeof( void *), This->comparator );
   return true;
}

DLL_PUBLIC bool utils_set_remove( utils_set set, const void * data, bool free_data ) {
   if(( set == NULL )||( data == NULL )) {
      return false;
   }
   utils_set_private * This = (utils_set_private *)set;
   if( This->data == NULL ) {
      return false;
   }
   void * res = bsearch( &data, This->data, This->count, sizeof( const void *), This->comparator );
   if( res ) {
      if( free_data ) {
         free( res );
      }
      --This->count;
      ssize_t index = ((const void **)res) - This->data;
      void ** src   = ((void **)res) + 1;
      size_t  n     = ( This->count - (size_t)index ) * sizeof( void * );
      memmove( res, src, n );
      return true;
   }
   return false;
}

DLL_PUBLIC bool utils_set_replace( utils_set set, const void * old_data, const void * new_data, bool free_old_data ) {
   if(( set == NULL )||( old_data == NULL )) {
      return false;
   }
   utils_set_private * This = (utils_set_private *)set;
   if( This->data == NULL ) {
      return false;
   }
   const void ** res = bsearch( &old_data, This->data, This->count, sizeof( const void *), This->comparator );
   if( res ) {
      if( free_old_data ) {
         free( res );
      }
      res[0] = new_data;
      qsort( This->data, This->count, sizeof( void * ), This->comparator );
      return true;
   }
   return false;
}

DLL_PUBLIC bool utils_set_contains( utils_set set, const void * value, bool * result ) {
   if(( set == NULL )||( result == NULL )) {
      return false;
   }
   *result = false;
   utils_set_private * This = (utils_set_private *)set;
   if( This->data ) {
      void * res = bsearch( &value, This->data, This->count, sizeof( void * ), This->comparator );
      *result = ( res != NULL );
   }
   return true;
}

DLL_PUBLIC bool utils_get_size( utils_set set, size_t * cardinality ) {
   if(( set == NULL )||( cardinality == NULL )) {
      return false;
   }
   utils_set_private * This = (utils_set_private *)set;
   *cardinality = This->count;
   return true;
}

DLL_PUBLIC bool utils_set_foreach( utils_set set, utils_set_iterator iter, void * user_context ) {
   if( set == NULL ) {
      return false;
   }
   utils_set_private * This = (utils_set_private *)set;
   for( size_t i = 0; i < This->count; ++i ) {
      if( ! iter( i, This->data[i], user_context )) {
         return false;
      }
   }
   return true;
}

DLL_PUBLIC bool utils_set_clear( utils_set set, bool free_data ) {
   if( set == NULL ) {
      return false;
   }
   utils_set_private * This = (utils_set_private *)set;
   if( free_data ) {
      for( size_t i = 0; i < This->count; ++i ) {
         // Astuce pour libérer un pointeur const void * en espérant qu'il ne soit pas "vraiment" const,
         // sinon c'est SIGSEGV !
         // Ne fonctionne que si les pointeurs sont stockables sur 64 bits
         free( CONST_CAST( This->data[i] ));
      }
   }
   free( This->data );
   This->data  = NULL;
   This->count = 0U;
   return true;
}

DLL_PUBLIC bool utils_set_delete( utils_set * pset, bool free_data ) {
   if( pset && utils_set_clear( *pset, free_data )) {
      free( *pset );
      *pset = NULL;
      return true;
   }
   return false;
}
