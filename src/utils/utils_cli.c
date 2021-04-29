#include <utils/utils_cli.h>

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct utils_cli_arg_s {
   const char *             name;
   utils_cli_type           type;
   bool                     mandatory;
   void *                   value;
   struct utils_cli_arg_s * next;
} utils_cli_arg_private;

static bool utils_cli_decode_boolean( const char * s, bool * dest ) {
   if( strcmp( s, "true" ) == 0 ) {
      *dest = true;
      return true;
   }
   if( strcmp( s, "false" ) == 0 ) {
      *dest = false;
      return true;
   }
   return false;
}

static bool utils_cli_decode_int16( const char * s, int16_t * dest ) {
   char * err = NULL;
   long lv = strtol( s, &err, 10 );
   if( err &&( *err == '\0' )) {
      *dest = (int16_t)lv;
      return true;
   }
   return false;
}

static bool utils_cli_decode_uint16( const char * s, uint16_t * dest ) {
   char * err = NULL;
   unsigned long lv = strtoul( s, &err, 10 );
   if( err &&( *err == '\0' )) {
      *dest = (uint16_t)lv;
      return true;
   }
   return false;
}

static bool utils_cli_decode_int32( const char * s, int32_t * dest ) {
   char * err = NULL;
   long lv = strtol( s, &err, 10 );
   if( err &&( *err == '\0' )) {
      *dest = (int32_t)lv;
      return true;
   }
   return false;
}

static bool utils_cli_decode_uint32( const char * s, uint32_t * dest ) {
   char * err = NULL;
   unsigned long lv = strtoul( s, &err, 10 );
   if( err &&( *err == '\0' )) {
      *dest = (uint32_t)lv;
      return true;
   }
   return false;
}

static bool utils_cli_decode_int64( const char * s, int64_t * dest ) {
   char * err = NULL;
   long lv = strtol( s, &err, 10 );
   if( err &&( *err == '\0' )) {
      *dest = (int64_t)lv;
      return true;
   }
   return false;
}

static bool utils_cli_decode_uint64( const char * s, uint64_t * dest ) {
   char * err = NULL;
   unsigned long long lv = strtoull( s, &err, 10 );
   if( err &&( *err == '\0' )) {
      *dest = (uint64_t)lv;
      return true;
   }
   return false;
}

static bool utils_cli_decode_float( const char * s, float * dest ) {
   char * err = NULL;
   float v = strtof( s, &err );
   if( err &&( *err == '\0' )) {
      *dest = v;
      return true;
   }
   return false;
}

static bool utils_cli_decode_double( const char * s, double * dest ) {
   char * err = NULL;
   double v = strtod( s, &err );
   if( err &&( *err == '\0' )) {
      *dest = v;
      return true;
   }
   return false;
}

static bool utils_cli_decode( const char * s, utils_cli_type type, void * dest ) {
   switch( type ) {
   case utils_cli_boolean: return utils_cli_decode_boolean( s, (bool *    )dest );
   case utils_cli_int16  : return utils_cli_decode_int16  ( s, (int16_t * )dest );
   case utils_cli_uint16 : return utils_cli_decode_uint16 ( s, (uint16_t *)dest );
   case utils_cli_int32  : return utils_cli_decode_int32  ( s, (int32_t * )dest );
   case utils_cli_uint32 : return utils_cli_decode_uint32 ( s, (uint32_t *)dest );
   case utils_cli_int64  : return utils_cli_decode_int64  ( s, (int64_t * )dest );
   case utils_cli_uint64 : return utils_cli_decode_uint64 ( s, (uint64_t *)dest );
   case utils_cli_float  : return utils_cli_decode_float  ( s, (float *   )dest );
   case utils_cli_double : return utils_cli_decode_double ( s, (double *  )dest );
   case utils_cli_string : *((const char **)dest) = s; return true;
   default: return false;
   }
   return false;
}

DLL_PUBLIC bool utils_cli_new( utils_cli_arg * options, int argc, char * argv[], ... ) {
   if(( options == NULL )||( argv == NULL )) {
      return false;
   }
   va_list args;
   va_start( args, argv );
   *options = NULL;
   const char * name = NULL;
   utils_cli_arg_private * head = NULL;
   while(( name = va_arg( args, const char *)) != NULL ) {
      utils_cli_type type = va_arg( args, utils_cli_type );
      const char * default_value = va_arg( args, const char * );
      void * value = va_arg( args, void * );
      utils_cli_arg_private * option = malloc( sizeof( utils_cli_arg_private ));
      option->name      = name;
      option->type      = type;
      option->mandatory = ( value == NULL );
      option->value     = value;
      option->next      = head;
      if( default_value ) {
         if( ! utils_cli_decode( default_value, type, option->value )) {
            fprintf( stderr, "Bad default value: '%s' for argument '%s'\n", default_value, name );
            return false;
         }
      }
      head = option;
   }
   va_end( args );
   for( int i = 1; i < argc; ++i ) {
      const char * arg = argv[i];
      bool search = true;
      utils_cli_arg_private * option = head;
      while( option && search ) {
         size_t len = strlen( option->name );
         const char * s = strstr( arg, option->name );
         if( s &&( s == ( arg + 2 ))&&( arg[2+len] == '=' )) {
            search = false;
            s = arg + 2 + len + 1;
            if( ! utils_cli_decode( s, option->type, option->value )) {
               fprintf( stderr, "Unable to decode argument '%s' value: '%s'\n", name, s );
               return false;
            }
            break;
         }
         option = option->next;
      }
      if( search ) {
         fprintf( stderr, "Unexpected argument: %s\n", arg );
         return false;
      }
   }
   utils_cli_arg_private * option = head;
   while( option ) {
      if(option->mandatory &&( option->value == NULL )) {
         fprintf( stderr, "Argument '%s' is mandatory\n", name );
         return false;
      }
      option = option->next;
   }
   *options = (utils_cli_arg)head;
   return true;
}

DLL_PUBLIC bool utils_cli_delete( utils_cli_arg * options ) {
   if( options == NULL ) {
      return false;
   }
   utils_cli_arg_private * option = (utils_cli_arg_private *)*options;
   while( option ) {
      utils_cli_arg_private * next = option->next;
      free( option );
      option = next;
   }
   *options = NULL;
   return true;
}
