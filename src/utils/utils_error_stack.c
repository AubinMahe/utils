#include <utils/utils_error_stack.h>

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static char error_stack[4000] = "";

void utils_error_stack_reset( void ) {
   error_stack[0] = '\0';
}

bool utils_error_stack_push( const char * file, unsigned line, const char * function, const char * format, ... ) {
   va_list args;
   va_start( args, format );
   char buffer1[1000];
   vsnprintf( buffer1, sizeof( buffer1 ), format, args );
   va_end( args );
   char buffer2[2000];
   snprintf( buffer2, sizeof( buffer2 ), "%s:%d:%s:%s", file, line, function, buffer1 );
   if( strlen( error_stack ) + 1 + strlen( buffer2 ) < sizeof( error_stack )) {
      strcat( error_stack, "\n" );
      strcat( error_stack, buffer2 );
   }
   return false;
}

const char * utils_error_stack_get( void ) {
   return error_stack;
}
