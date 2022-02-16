#pragma once

#include <stdbool.h>

void         utils_error_stack_reset( void );
bool         utils_error_stack_push( const char * file, unsigned line, const char * function, const char * format, ... );
const char * utils_error_stack_get( void );
