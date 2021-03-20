#include <utils/utils_prefs.h>
#include <utils/utils_map.h>

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
   char      rcpath[PATH_MAX];
   utils_map map;
} utils_prefs_private;


static int property_name_compare( const void * l, const void * r ) {
   const char * const * pl = (const char * const *)l;
   const char * const * pr = (const char * const *)r;
   return strcmp( *pl, *pr );
}

bool utils_prefs_load( utils_prefs * prefs, const char * program_name ) {
   if(( prefs == NULL )||( program_name == NULL )) {
      return false;
   }
   utils_prefs_private * This = malloc( sizeof( utils_prefs_private ));
   if( This == NULL ) {
      perror( "malloc" );
      return false;
   }
   memset( This, 0, sizeof( utils_prefs_private ));
   *prefs = (utils_prefs)This;
   snprintf( This->rcpath, PATH_MAX, ".%src", program_name );
   FILE * chatrc = fopen( This->rcpath, "rt" );
   if( chatrc == NULL ) {
      const char * user_home = getenv( "HOME" );
      if( user_home ) {
         snprintf( This->rcpath, PATH_MAX, "%s/%s", user_home, program_name );
         chatrc = fopen( This->rcpath, "rt" );
         if( chatrc == NULL ) {
            snprintf( This->rcpath, PATH_MAX, "/etc/%s/%src", program_name, program_name );
            chatrc = fopen( This->rcpath, "rt" );
         }
      }
      else {
         snprintf( This->rcpath, PATH_MAX, "/etc/%s/%src", program_name, program_name );
         chatrc = fopen( This->rcpath, "rt" );
      }
   }
   if( chatrc == NULL ) {
      return false;
   }
   if( ! utils_map_new( &This->map, property_name_compare )) {
      return false;
   }
   char buffer[2000];
   unsigned line = 0;
   while( fgets( buffer, sizeof( buffer ), chatrc )) {
      ++line;
      char * prop_name = buffer;
      while( isspace( *prop_name )) {
         ++prop_name;
      }
      if( *prop_name == '#' ) {
         continue;
      }
      char *  equal  = strchr( prop_name, '=' );
      char *  colon  = strchr( prop_name, ':' );
      char *  sep    = NULL;
      if( equal == NULL ) {
         sep = colon;
      }
      else if( colon == NULL ) {
         sep = equal;
      }
      else {
         sep = ( equal < colon ) ? equal : colon;
      }
      if( sep - prop_name < 0 ) {
         fprintf( stderr, "%s: utils_prefs file syntax error at line %d\n", This->rcpath, line );
         return false;
      }
      *sep = '\0';
      prop_name = strtok( prop_name, " \t" );
      while( isspace( *++sep ));
      char * end = strrchr( sep, '\n' );
      if( end ) {
         while( isspace( *--end ));
         *++end = '\0';
      }
      utils_map_put( This->map, strdup( prop_name ), strdup( sep ));
   }
   fclose( chatrc );
   return true;
}

bool utils_prefs_get_path( utils_prefs prefs, const char ** dest ) {
   if(( prefs == NULL )||( dest == NULL )) {
      return false;
   }
   utils_prefs_private * This = (utils_prefs_private *)prefs;
   *dest = This->rcpath;
   return true;
}

bool utils_prefs_get_boolean( utils_prefs prefs, const char * name, bool * value ) {
   const char * s = NULL;
   if( utils_prefs_get_string( prefs, name, &s )) {
      if( 0 == strcmp( "true", s )) {
         *value = true;
         return true;
      }
      if( 0 == strcmp( "false", s )) {
         *value = false;
         return true;
      }
   }
   return false;
}

bool utils_prefs_get_ushort( utils_prefs prefs, const char * name, unsigned short * value ) {
   long lg = 0;
   if( utils_prefs_get_long( prefs, name, &lg )) {
      *value = (unsigned short)lg;
      return true;
   }
   return false;
}

bool utils_prefs_get_int( utils_prefs prefs, const char * name, int * value ) {
   long lg = 0;
   if( utils_prefs_get_long( prefs, name, &lg )) {
      *value = (int)lg;
      return true;
   }
   return false;
}

bool utils_prefs_get_long( utils_prefs prefs, const char * name, long * value ) {
   const char * s = NULL;
   if( ! utils_prefs_get_string( prefs, name, &s )) {
      return false;
   }
   int base  = 10;
   if(( s[0] == '0' )&&(( s[1] == 'x' )||( s[1] == 'X' ))) {
      base = 16;
      s += 2;
   }
   char * err = NULL;
   *value = strtol( s, &err, base );
   return err &&( *err == '\0' );
}

bool utils_prefs_get_string( utils_prefs prefs, const char * name, const char ** value ) {
   if(( prefs == NULL )||( name == NULL )||( value == NULL )) {
      return false;
   }
   utils_prefs_private * This = (utils_prefs_private *)prefs;
   if( This->map == NULL ) {
      return false;
   }
   void * result = NULL;
   if( ! utils_map_get( This->map, name, &result )) {
      return false;
   }
   *value = result;
   return true;
}

bool utils_prefs_delete( utils_prefs * prefs ) {
   if( prefs == NULL ) {
      return false;
   }
   utils_prefs_private * This = (utils_prefs_private *)*prefs;
   if( This->map ) {
      utils_map_delete( &This->map, true );
   }
   free( This );
   *prefs = NULL;
   return true;
}
