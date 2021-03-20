#include <utils/utils_cli.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool utils_cli_decode_uint16( const char * s, unsigned short * dest ) {
   char * err = NULL;
   long lv = strtol( s, &err, 10 );
   if( err &&( *err == '\0' )) {
      *dest = (unsigned short)lv;
      return true;
   }
   return false;
}

static bool utils_cli_decode( const char * s, utils_cli_type type, void * dest ) {
   switch( type ) {
   case utils_cli_STRING: *((const char **)dest) = s; return true;
   case utils_cli_USHORT: return utils_cli_decode_uint16( s, (unsigned short *)dest );
   default: return false;
   }
   return false;
}

bool utils_cli_new( utils_cli_arg * options, int argc, char * argv[], ... ) {
   if(( options == NULL )||( argv == NULL )) {
      return false;
   }
   va_list args;
   va_start( args, argv );
   *options = NULL;
   const char * name = NULL;
   utils_cli_arg head = NULL;
   while(( name = va_arg( args, const char *)) != NULL ) {
      utils_cli_type type          = va_arg( args, utils_cli_type );
      bool           mandatory     = va_arg( args, int );
      const char *   default_value = va_arg( args, const char * );
      void *         value         = va_arg( args, void * );
      utils_cli_arg  option        = malloc( sizeof( struct utils_cli_arg_s ));
      option->name      = name;
      option->type      = type;
      option->mandatory = mandatory;
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
      const char *  arg    = argv[i];
      bool          search = true;
      utils_cli_arg option = head;
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
   utils_cli_arg option = head;
   while( option ) {
      if(option->mandatory &&( option->value == NULL )) {
         fprintf( stderr, "Argument '%s' is mandatory\n", name );
         return false;
      }
      option = option->next;
   }
   *options = head;
   return true;
}

bool utils_cli_delete( utils_cli_arg * options ) {
   if( options == NULL ) {
      return false;
   }
   utils_cli_arg option = *options;
   while( option ) {
      utils_cli_arg next = option->next;
      free( option );
      option = next;
   }
   *options = NULL;
   return true;
}
