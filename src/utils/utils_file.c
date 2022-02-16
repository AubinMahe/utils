#include <utils/utils_file.h>

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __linux
#  include <sys/sendfile.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#ifdef __linux
#  include <unistd.h>
#else
#  include <windows.h>
#endif

typedef struct {
   size_t  capacity;
   size_t  count;
   char ** storage;
} utils_file_list_private;

static int filename_compare( const void * left, const void * right ) {
   const char * const * l = left;
   const char * const * r = right;
   if( l && r ) {
      return strcmp( *l, *r );
   }
   if( l ) {
      return +1;
   }
   if( r ) {
      return -1;
   }
   // Les deux sont nuls
   return 0;
}

static bool utils_file_list_add( utils_file_list_private * This, const char * item ) {
   if( This->count == This->capacity ) {
      This->capacity *= 2;
      char ** new_storage = realloc( This->storage, This->capacity * sizeof( char * ));
      if( new_storage == NULL ) {
         fprintf( stderr, "%s:%d:%s:realloc:%s", __FILE__, __LINE__, __func__, strerror( errno ));
         return false;
      }
      This->storage = new_storage;
   }
   This->storage[This->count++] = strdup( item );
   return true;
}

DLL_PUBLIC bool utils_file_list_new( utils_file_list * files, const char * path ) {
   if(( files == NULL )||( path == NULL )) {
      fprintf( stderr, "%s:%d:%s: null argument", __FILE__, __LINE__, __func__ );
      return false;
   }
   utils_file_list_private * This = malloc( sizeof( utils_file_list_private ));
   *files = (utils_file_list)This;
   This->capacity = 20;
   This->count    = 0;
   This->storage  = malloc( This->capacity * sizeof( char * ));
   if( This->storage == NULL ) {
      fprintf( stderr, "%s:%d:%s:malloc:%s", __FILE__, __LINE__, __func__, strerror( errno ));
      return false;
   }
   DIR * dir = opendir( path );
   if( dir == NULL ) {
      fprintf( stderr, "%s:%d:%s:opendir(%s):%s", __FILE__, __LINE__, __func__, path, strerror( errno ));
      return false;
   }
   struct dirent * d = NULL;
   errno = 0;
   while(( d = readdir( dir )) != NULL ) {
      if( ! utils_file_list_add( This, d->d_name )) {
         closedir( dir );
         return false;
      }
      errno = 0;
   }
   if( errno ) {
      fprintf( stderr, "%s:%d:%s:readdir:%s", __FILE__, __LINE__, __func__, strerror( errno ));
   }
   int ret = closedir( dir );
   if( ret ) {
      fprintf( stderr, "%s:%d:%s:closedir(%s):%s", __FILE__, __LINE__, __func__, path, strerror( errno ));
      return false;
   }
   qsort( This->storage, This->count, sizeof( char * ), filename_compare );
   return true;
}

DLL_PUBLIC bool utils_file_list_contains( utils_file_list files, const char * filename ) {
   if( files == NULL ) {
      fprintf( stderr, "%s:%d:%s: null argument", __FILE__, __LINE__, __func__ );
      return false;
   }
   utils_file_list_private * This = (utils_file_list_private *)files;
   const char ** entry = bsearch( &filename, This->storage, This->count, sizeof( char * ), filename_compare );
   return entry != NULL;
}

DLL_PUBLIC bool utils_file_list_delete( utils_file_list * files ) {
   if( files == NULL ) {
      fprintf( stderr, "%s:%d:%s: null argument", __FILE__, __LINE__, __func__ );
      return false;
   }
   utils_file_list_private * This = *(utils_file_list_private **)files;
   for( size_t i = 0; i < This->count; ++i ) {
      free( This->storage[i] );
   }
   free( This->storage );
   This->count    = 0;
   This->capacity = 0;
   This->storage  = NULL;
   *files = NULL;
   return true;
}

#ifdef _WIN32
static char * realpath( const char * path, void * unused ) {
   (void)unused;
   char too_small[1];
   DWORD size = GetFullPathNameA( path, sizeof( too_small ), too_small, NULL );
   if( size == 0 ) {
      return NULL;
   }
   char * buffer = malloc( size );
   if( buffer == NULL ) {
      return NULL;
   }
   if( GetFullPathNameA( path, size, buffer, NULL ) == ( size - 1 )) {
      return buffer;
   }
   free( buffer );
   return NULL;
}
#endif

DLL_PUBLIC bool utils_file_get_absolute_path( const char * path, char * target, size_t target_size ) {
   bool   ok  = false;
   char * tmp = realpath( path, NULL );
   if( tmp ) {
      if( strlen( tmp ) < target_size ) {
         snprintf( target, target_size, "%s", tmp );
         ok = true;
      }
      free( tmp );
   }
   else if( errno == ENOENT ) {
      size_t tmp_size = ( target_size < PATH_MAX ) ? PATH_MAX : target_size;
      char * dir  = malloc( tmp_size );
      if( dir == NULL ) {
         fprintf( stderr, "%s:%d:%s:malloc:%s", __FILE__, __LINE__, __func__, strerror( errno ));
      }
      else {
         if( utils_file_dir_name( path, dir, tmp_size )) {
            char * file = malloc( tmp_size );
            if( file == NULL ) {
               fprintf( stderr, "%s:%d:%s:malloc:%s", __FILE__, __LINE__, __func__, strerror( errno ));
            }
            else {
               if( utils_file_file_name( path, file, tmp_size )) {
                  tmp = realpath( dir, NULL );
                  if( tmp ) {
                     ok = utils_file_build_path( tmp, file, target, target_size );
                     free( tmp );
                  }
               }
               free( file );
            }
         }
         free( dir );
      }
   }
   return ok;
}

DLL_PUBLIC bool utils_file_build_path( const char * path, const char * filename, char * target, size_t target_size ) {
   size_t path_len     = strlen( path );
   size_t filename_len = strlen( filename );
   if( path_len + 1 + filename_len >= target_size ) {
      return false;
   }
   int ret = snprintf( target, target_size, "%s/%s", path, filename );
   return ret > 0;
}

DLL_PUBLIC bool utils_file_dir_name( const char * path, char * target, size_t target_size ) {
   const char * last_sep_linux = strrchr( path, '/' );
   const char * last_sep_dos   = strrchr( path, '\\' );
   const char * last_sep = ( last_sep_dos < last_sep_linux ) ? last_sep_linux : last_sep_dos;
   if( last_sep == NULL ) {
      return false;
   }
   size_t len = strlen( path ) - strlen( last_sep );
   if( len >= target_size ) {
      return false;
   }
   memcpy( target, path, len );
   target[len] = '\0';
   return true;
}

DLL_PUBLIC bool utils_file_file_name( const char * path, char * target, size_t target_size ) {
   const char * last_sep_linux = strrchr( path, '/' );
   const char * last_sep_dos   = strrchr( path, '\\' );
   const char * last_sep = ( last_sep_dos < last_sep_linux ) ? last_sep_linux : last_sep_dos;
   if( last_sep == NULL ) {
      return false;
   }
   size_t len = strlen( last_sep + 1 );
   if( len >= target_size ) {
      return false;
   }
   memcpy( target, last_sep + 1, len );
   target[len] = '\0';
   return true;
}

DLL_PUBLIC bool utils_file_set_extension( const char * path, const char * extension, char * target, size_t target_size ) {
   bool ok = false;
   const char * dot = strrchr( path, '.' );
   if( *extension == '.' ) {
      ++extension;
   }
   size_t extension_len = strlen( extension );
   if( dot == NULL ) {
      // Il n'y a pas d'extension
      int ret = snprintf( target, target_size, "%s.%s", path, extension );
      ok = ( ret > 0 )&&((size_t)ret < target_size );
   }
   else {
      // Il y a une extension
      if( 0 == strcmp( dot+1, extension )) {
         // C'est la bonne, on effectue juste une copie
         int ret = snprintf( target, target_size, "%s", path );
         ok = ( ret > 0 )&&((size_t)ret < target_size );
      }
      else {
         // Ce n'est pas la bonne, on concatene le chemin, le point et l'extension
         size_t path_len = strlen( path ) - strlen( dot );
         if( path_len + 1 + extension_len < target_size ) {
            memcpy( target, path, path_len );
            target[path_len] = '.';
            memcpy( target + path_len + 1, extension, extension_len );
            target[path_len + 1 + extension_len] = '\0';
            ok = true;
         }
      }
   }
   return ok;
}

DLL_PUBLIC bool utils_file_is_a_directory( const char * path ) {
   struct stat statbuf;
   if( stat( path, &statbuf ) == -1 ) {
      return false;
   }
   return S_ISDIR( statbuf.st_mode );
}

DLL_PUBLIC bool utils_file_copy( const char * dest, const char * src ) {
#ifdef _WIN32
   bool ret = CopyFile( src, dest, FALSE );
#else
   chmod( dest, 0666 );
   int out_fd = open( dest, O_CREAT|O_WRONLY|O_TRUNC, 0666 );
   if( out_fd < 0 ) {
      fprintf( stderr, "%s:%d:%s:open(%s):%s", __FILE__, __LINE__, __func__, dest, strerror( errno ));
      return false;
   }
   int in_fd  = open( src, O_RDONLY );
   if( in_fd < 0 ) {
      fprintf( stderr, "%s:%d:%s:open(%s):%s", __FILE__, __LINE__, __func__, src, strerror( errno ));
      return false;
   }
   struct stat statbuf;
   if( stat( src, &statbuf ) == -1 ) {
      fprintf( stderr, "%s:%d:%s:stat(%s):%s", __FILE__, __LINE__, __func__, src, strerror( errno ));
      close( out_fd );
      close( in_fd );
      return false;
   }
   ssize_t bytes = sendfile( out_fd, in_fd, 0, (size_t)statbuf.st_size );
   if( bytes < 0 ) {
      fprintf( stderr, "%s:%d:%s:sendfile:%s", __FILE__, __LINE__, __func__, strerror( errno ));
      close( out_fd );
      close( in_fd );
      return false;
   }
   bool ret = true;
   if( close( out_fd )) {
      fprintf( stderr, "%s:%d:%s:close(%s):%s", __FILE__, __LINE__, __func__, dest, strerror( errno ));
      ret = false;
   }
   if( close( in_fd )) {
      fprintf( stderr, "%s:%d:%s:close(%s):%s", __FILE__, __LINE__, __func__, src, strerror( errno ));
      ret = false;
   }
#endif
   return ret;
}
