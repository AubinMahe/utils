#include <net/net_buff.h>
#include <utils/utils_error_stack.h>

#ifdef __linux
#  include <arpa/inet.h>
#else
#  include <winsock2.h>
#endif
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
   byte * buffer;
   size_t capacity;
   size_t limit;
   size_t position;
} net_buff_private;

static bool byte_order_is_little = true;

DLL_PUBLIC bool net_buff_new( net_buff * nb, size_t capacity ) {
   byte_order_is_little = ( htonl(1) != 1 );
   if(( nb == NULL )||( capacity == 0 )) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "null argument" );
   }
   if( *nb ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "*nb must be null" );
   }
   net_buff_private * This = malloc( sizeof( net_buff_private ));
   memset( This, 0, sizeof( net_buff_private ));
   This->buffer   = malloc( capacity );
   This->capacity = capacity;
   This->limit    = capacity;
   *nb = (net_buff)This;
   return true;
}

DLL_PUBLIC bool net_buff_get_capacity( net_buff nb, size_t * capacity ) {
   if(( nb == NULL )||( capacity == 0 )) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "null argument" );
   }
   net_buff_private * This = (net_buff_private *)nb;
   *capacity = This->capacity;
   return true;
}

DLL_PUBLIC bool net_buff_get_limit( net_buff nb, size_t * limit ) {
   if( nb == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "nb: null argument" );
   }
   if( limit == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "limit: null argument" );
   }
   net_buff_private * This = (net_buff_private *)nb;
   *limit = This->limit;
   return true;
}

DLL_PUBLIC bool net_buff_get_position( net_buff nb, size_t * position ) {
   if( nb == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "nb: null argument" );
   }
   if( position == 0 ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "position: null argument" );
   }
   net_buff_private * This = (net_buff_private *)nb;
   *position = This->position;
   return true;
}

DLL_PUBLIC bool net_buff_dump( net_buff nb, char * dest, size_t dest_size ) {
   if( nb == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "nb: null argument" );
   }
   if( dest == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "dest: null argument" );
   }
   if( dest_size == 0 ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "dest_size: zero argument" );
   }
   memset( dest, 0, dest_size );
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "internal buffer is null" );
   }
   // 00005b20  00 00 00 00 00 00 00 00  01 00 00 00 00 00 00 00  |................|
   // 012345678901234567890123456789012345678901234567890123456789012345678901234567
   //          10        20        30        40        50        60        70
   size_t hi = 0, ti = 0;
   for( size_t i = 0; i < This->limit; ++i, hi += 3, ++ti ) {
      if( ti >= dest_size ) {
         return utils_error_stack_push( __FILE__, __LINE__, __func__, "dest too small" );
      }
      if(( i % 16 ) == 0 ) {
         if( i > 0 ) {
            size_t pai = 79 * (i-16)/16;
            dest[pai+59] = ' ';
            dest[pai+77] = '|';
         }
         size_t ai = 79 * i/16;
#ifdef __linux
         sprintf( dest+ai, "%08lX ", i );
#else
         sprintf( dest+ai, "%08I64X ", i );
#endif
         dest[ai+59] = ' ';
         dest[ai+60] = '|';
         dest[ai+77] = '|';
         dest[ai+78] = '\n';
         hi = ai +  9;
         ti = ai + 61;
      }
      else if(( i % 8 ) == 0 ) {
         dest[hi++] = ' ';
      }
      byte c = This->buffer[i];
      sprintf( dest+hi, " %02X", c );
      dest[hi+3] = ' ';
      dest[ti]   = (( c <= 32 )||( c >= 127 )) ? '.' : (char)c;
   }
   for( size_t i = This->limit % 16; i < 16; ++i ) {
      if(( i % 8 ) == 0 ) {
         dest[++hi] = ' ';
      }
      dest[++hi] = ' ';
      dest[++hi] = ' ';
      dest[++hi] = ' ';
      dest[ti++] = ' ';
   }
   return true;
}

DLL_PUBLIC bool net_buff_clear( net_buff nb ) {
   if( nb == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "nb: null argument" );
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "internal buffer is null" );
   }
   This->position = 0;
   This->limit    = This->capacity;
   return true;
}

DLL_PUBLIC bool net_buff_encode_boolean( net_buff nb, bool value ) {
   if( nb == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "nb: null argument" );
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "internal buffer is null" );
   }
   if( This->position + 1 > This->limit ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "overflow" );
   }
   This->buffer[This->position] = value ? 1 : 0;
   This->position += 1;
   return true;
}

DLL_PUBLIC bool net_buff_encode_byte( net_buff nb, byte value ) {
   if( nb == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "nb: null argument" );
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "internal buffer is null" );
   }
   if( This->position + sizeof( value ) > This->limit ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "overflow" );
   }
   This->buffer[This->position] = value;
   This->position += sizeof( value );
   return true;
}

DLL_PUBLIC bool net_buff_encode_int8( net_buff nb, int8_t value ) {
   return net_buff_encode_byte( nb, (byte)value );
}

DLL_PUBLIC bool net_buff_encode_uint16( net_buff nb, uint16_t value ) {
   if( nb == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "nb: null argument" );
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "internal buffer is null" );
   }
   if( This->position + sizeof( value ) > This->limit ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "overflow" );
   }
   uint16_t encoded_value = htons( value );
   memcpy( This->buffer + This->position, &encoded_value, sizeof( encoded_value ));
   This->position += sizeof( encoded_value );
   return true;
}

DLL_PUBLIC bool net_buff_encode_int16( net_buff nb, int16_t value ) {
   return net_buff_encode_uint16( nb, (uint16_t)value );
}

DLL_PUBLIC bool net_buff_encode_uint32( net_buff nb, uint32_t value ) {
   if( nb == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "nb: null argument" );
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "internal buffer is null" );
   }
   if( This->position + sizeof( value ) > This->limit ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "overflow" );
   }
   uint32_t encoded_value = htonl( value );
   memcpy( This->buffer + This->position, &encoded_value, sizeof( encoded_value ));
   This->position += sizeof( encoded_value );
   return true;
}

DLL_PUBLIC bool net_buff_encode_int32( net_buff nb, int32_t value ) {
   return net_buff_encode_uint32( nb, (uint32_t)value );
}

DLL_PUBLIC bool net_buff_encode_uint64( net_buff nb, uint64_t value ) {
   if( nb == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "nb: null argument" );
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "internal buffer is null" );
   }
   if( This->position + sizeof( value ) > This->limit ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "overflow" );
   }
   if( byte_order_is_little ) {
      value = (( value & 0xFF00000000000000LL ) >> 56 )
             |(( value & 0x00FF000000000000LL ) >> 40 )
             |(( value & 0x0000FF0000000000LL ) >> 24 )
             |(( value & 0x000000FF00000000LL ) >>  8 )
             |(( value & 0x00000000FF000000LL ) <<  8 )
             |(( value & 0x0000000000FF0000LL ) << 24 )
             |(( value & 0x000000000000FF00LL ) << 40 )
             |(  value                          << 56 );
   }
   memcpy( This->buffer + This->position, &value, sizeof( value ));
   This->position += sizeof( value );
   return true;
}

DLL_PUBLIC bool net_buff_encode_int64( net_buff nb, int64_t value ) {
   return net_buff_encode_uint64( nb, (uint64_t)value );
}

DLL_PUBLIC bool net_buff_encode_float( net_buff nb, float src ) {
   void * ptr = &src;
   return net_buff_encode_uint32( nb, *(uint32_t*)ptr );
}

DLL_PUBLIC bool net_buff_encode_double( net_buff nb, double src ) {
   void * ptr = &src;
   return net_buff_encode_uint64( nb, *(uint64_t *)ptr );
}

DLL_PUBLIC bool net_buff_encode_string( net_buff nb, const char * string ) {
   if( nb == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "nb: null argument" );
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "internal buffer is null" );
   }
   size_t length = strlen( string );
   uint32_t encoded_length = htonl((uint32_t)length );
   if( This->position + sizeof( encoded_length ) > This->limit ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "overflow" );
   }
   memcpy( This->buffer + This->position, &encoded_length, sizeof( encoded_length ));
   This->position += sizeof( encoded_length );
   if( This->position + length > This->limit ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "overflow" );
   }
   memcpy( This->buffer + This->position, string, length );
   This->position += length;
   return true;
}

DLL_PUBLIC bool net_buff_flip( net_buff nb ) {
   if( nb == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "nb: null argument" );
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "internal buffer is null" );
   }
   This->limit = This->position;
   This->position = 0;
   return true;
}

DLL_PUBLIC bool net_buff_send( net_buff nb, SOCKET sckt, struct sockaddr_in * to ) {
   if( nb == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "nb: null argument" );
   }
   if( to == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "to: null argument" );
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "internal buffer is null" );
   }
#ifdef __linux
   size_t  length = This->limit - This->position;
   ssize_t nbytes = sendto( sckt, This->buffer + This->position, length, 0, (struct sockaddr*)to, sizeof( struct sockaddr_in ));
#else
   int length = (int)( This->limit - This->position );
   ssize_t nbytes = sendto( sckt, (char *)(This->buffer + This->position), length, 0, (struct sockaddr*)to, sizeof( struct sockaddr_in ));
#endif
   if( nbytes >= 0 ) {
      This->position += (size_t)nbytes;
#ifdef __linux
      return ((size_t)nbytes) == length;
#else
      return nbytes == length;
#endif
   }
   return utils_error_stack_push( __FILE__, __LINE__, __func__, "sendto:%s", strerror( errno ));
}

DLL_PUBLIC bool net_buff_wrap( net_buff * nb, byte * bytes, size_t capacity ) {
   byte_order_is_little = ( htonl(1) != 1 );
   if( nb == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "nb: null argument" );
   }
   if( bytes == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "bytes: null argument" );
   }
   if( capacity == 0 ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "capacity: zero argument" );
   }
   if( *nb ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "*nb must be null" );
   }
   net_buff_private * This = malloc( sizeof( net_buff_private ));
   memset( This, 0, sizeof( net_buff_private ));
   This->buffer   = malloc( capacity );
   This->capacity = capacity;
   This->limit    = capacity;
   memmove( This->buffer, bytes, capacity );
   *nb = (net_buff)This;
   return true;
}

DLL_PUBLIC bool net_buff_receive( net_buff nb, SOCKET sckt, struct sockaddr_in * from ) {
   if(( nb == NULL )||( from == NULL )) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "nb: null argument" );
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "internal buffer is null" );
   }
#ifdef __linux
   size_t    length  = This->limit - This->position;
   socklen_t addrlen = sizeof( struct sockaddr_in );
   ssize_t   nbytes  = recvfrom( sckt, This->buffer + This->position, length, 0, (struct sockaddr *)from, &addrlen );
#else
   int     length  = (int)( This->limit - This->position );
   int     addrlen = sizeof( struct sockaddr_in );
   ssize_t nbytes  = recvfrom( sckt, (char *)(This->buffer + This->position), length, 0, (struct sockaddr *)from, &addrlen );
#endif
   if( nbytes >= 0 ) {
      This->position += (size_t)nbytes;
      return true;
   }
   return utils_error_stack_push( __FILE__, __LINE__, __func__, "recvfrom:%s", strerror( errno ));
}

DLL_PUBLIC bool net_buff_decode_boolean( net_buff nb, bool * dest ) {
   if( nb == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "nb: null argument" );
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "internal buffer is null" );
   }
   if( This->position + 1 > This->limit ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "underflow" );
   }
   *dest = This->buffer[This->position] != 0;
   This->position += 1;
   return true;
}

DLL_PUBLIC bool net_buff_decode_byte( net_buff nb, byte * dest ) {
   if( nb == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "nb: null argument" );
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "internal buffer is null" );
   }
   if( This->position + sizeof( byte ) > This->limit ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "underflow" );
   }
   *dest = This->buffer[This->position];
   This->position += 1;
   return true;
}

DLL_PUBLIC bool net_buff_decode_int8( net_buff nb, int8_t * dest ) {
   return net_buff_decode_byte( nb, (byte *)dest );
}

DLL_PUBLIC bool net_buff_decode_uint16( net_buff nb, uint16_t * dest ) {
   if( nb == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "nb: null argument" );
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "internal buffer is null" );
   }
   if( This->position + sizeof( uint16_t ) > This->limit ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "underflow" );
   }
   *dest = ntohs( *(uint16_t *)( This->buffer + This->position ));
   This->position += sizeof( *dest );
   return true;
}

DLL_PUBLIC bool net_buff_decode_int16( net_buff nb, int16_t * dest ) {
   return net_buff_decode_uint16( nb, (uint16_t *)dest );
}

DLL_PUBLIC bool net_buff_decode_uint32( net_buff nb, uint32_t * dest ) {
   if( nb == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "nb: null argument" );
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "internal buffer is null" );
   }
   if( This->position + sizeof( uint32_t ) > This->limit ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "underflow" );
   }
   *dest = ntohl( *(uint32_t *)( This->buffer + This->position ));
   This->position += sizeof( *dest );
   return true;
}

DLL_PUBLIC bool net_buff_decode_int32( net_buff nb, int32_t * dest ) {
   return net_buff_decode_uint32( nb, (uint32_t *)dest );
}

DLL_PUBLIC bool net_buff_decode_uint64( net_buff nb, uint64_t * dest ) {
   if( nb == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "nb: null argument" );
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "internal buffer is null" );
   }
   if( This->position + sizeof( uint64_t ) > This->limit ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "underflow" );
   }
   *dest = *(uint64_t *)( This->buffer + This->position );
   if( byte_order_is_little ) {
      *dest = (( *dest & 0xFF00000000000000LL ) >> 56 )
             |(( *dest & 0x00FF000000000000LL ) >> 40 )
             |(( *dest & 0x0000FF0000000000LL ) >> 24 )
             |(( *dest & 0x000000FF00000000LL ) >>  8 )
             |(( *dest & 0x00000000FF000000LL ) <<  8 )
             |(( *dest & 0x0000000000FF0000LL ) << 24 )
             |(( *dest & 0x000000000000FF00LL ) << 40 )
             |(  *dest                          << 56 );
   }
   This->position += sizeof( *dest );
   return true;
}

DLL_PUBLIC bool net_buff_decode_int64( net_buff nb, int64_t * dest ) {
   return net_buff_decode_uint64( nb, (uint64_t *)dest );
}

DLL_PUBLIC bool net_buff_decode_float( net_buff nb, float * dest ) {
   return net_buff_decode_uint32( nb, (uint32_t *)dest );
}

DLL_PUBLIC bool net_buff_decode_double( net_buff nb, double * dest ) {
   return net_buff_decode_uint64( nb, (uint64_t *)dest );
}

DLL_PUBLIC bool net_buff_decode_string( net_buff nb, char * target, size_t target_size ) {
   if( nb == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "nb: null argument" );
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "internal buffer is null" );
   }
   if( This->position + sizeof( uint32_t ) > This->limit ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "underflow" );
   }
   uint32_t length = ntohl( *(uint32_t *)( This->buffer + This->position ));
   if( This->position + sizeof( length ) >= This->limit ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "underflow" );
   }
   if( This->position + length > This->limit ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "underflow (string length = %d)", length );
   }
   This->position += sizeof( length );
   if( length >= target_size ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "too small target buffer, %d bytes needed\n", length+1 );
   }
   strncpy( target, (char *)( This->buffer + This->position ), length );
   target[length] = '\0';
   This->position += length;
   return true;
}

DLL_PUBLIC bool net_buff_delete( net_buff * nb ) {
   if( nb == NULL ) {
      return utils_error_stack_push( __FILE__, __LINE__, __func__, "nb: null argument" );
   }
   if( *nb ) {
      net_buff_private * This = (net_buff_private *)*nb;
      free( This->buffer );
      free( This );
      *nb = NULL;
   }
   return true;
}
