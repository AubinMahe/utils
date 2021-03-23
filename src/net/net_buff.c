#include <net/net_buff.h>

#include <arpa/inet.h>
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

bool net_buff_new( net_buff * nb, size_t capacity ) {
   byte_order_is_little = ( htonl(1) != 1 );
   if( nb == NULL ) {
      return false;
   }
   if( capacity == 0 ) {
      return false;
   }
   net_buff_private * This = malloc( sizeof( net_buff_private ));
   memset( This, 0, sizeof( net_buff_private ));
   This->buffer   = malloc( capacity );
   This->capacity = capacity;
   This->limit    = capacity;
   *nb = (net_buff)This;
   return true;
}

bool net_buff_get_capacity( net_buff nb, size_t * capacity ) {
   if( nb == NULL ) {
      return false;
   }
   if( capacity == NULL ) {
      return false;
   }
   net_buff_private * This = (net_buff_private *)nb;
   *capacity = This->capacity;
   return true;
}

bool net_buff_get_limit( net_buff nb, size_t * limit ) {
   if( nb == NULL ) {
      return false;
   }
   if( limit == NULL ) {
      return false;
   }
   net_buff_private * This = (net_buff_private *)nb;
   *limit = This->limit;
   return true;
}

bool net_buff_get_position( net_buff nb, size_t * position ) {
   if( nb == NULL ) {
      return false;
   }
   if( position == NULL ) {
      return false;
   }
   net_buff_private * This = (net_buff_private *)nb;
   *position = This->position;
   return true;
}

bool net_buff_dump( net_buff nb, char * dest, size_t dest_size ) {
   if(( nb == NULL )||( dest == NULL )) {
      return false;
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      *dest= '\0';
      return true;
   }
   memset( dest, 0, dest_size );
   // 00005b20  00 00 00 00 00 00 00 00  01 00 00 00 00 00 00 00  |................|
   // 012345678901234567890123456789012345678901234567890123456789012345678901234567
   //          10        20        30        40        50        60        70
   size_t hi = 0, ti = 0;
   for( size_t i = 0; i < This->limit; ++i, hi += 3, ++ti ) {
      if( ti >= dest_size ) {
         return false;
      }
      if(( i % 16 ) == 0 ) {
         if( i > 0 ) {
            size_t pai = 79 * (i-16)/16;
            dest[pai+59] = ' ';
            dest[pai+77] = '|';
         }
         size_t ai = 79 * i/16;
         sprintf( dest+ai, "%08lX ", i );
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

bool net_buff_clear( net_buff nb ) {
   if( nb == NULL ) {
      return false;
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return false;
   }
   This->position = 0;
   This->limit    = This->capacity;
   return true;
}

bool net_buff_encode_boolean( net_buff nb, bool value ) {
   if( nb == NULL ) {
      return false;
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return false;
   }
   if( This->position + 1 > This->limit ) {
      return false;
   }
   This->buffer[This->position] = value ? 1 : 0;
   This->position += 1;
   return true;
}

bool net_buff_encode_byte( net_buff nb, byte value ) {
   if( nb == NULL ) {
      return false;
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return false;
   }
   if( This->position + sizeof( value ) > This->limit ) {
      return false;
   }
   This->buffer[This->position] = value;
   This->position += sizeof( value );
   return true;
}

bool net_buff_encode_int8( net_buff nb, int8_t value ) {
   return net_buff_encode_byte( nb, (byte)value );
}

bool net_buff_encode_uint16( net_buff nb, uint16_t value ) {
   if( nb == NULL ) {
      return false;
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return false;
   }
   if( This->position + sizeof( value ) > This->limit ) {
      return false;
   }
   uint16_t encoded_value = htons( value );
   memcpy( This->buffer + This->position, &encoded_value, sizeof( encoded_value ));
   This->position += sizeof( encoded_value );
   return true;
}

bool net_buff_encode_int16( net_buff nb, int16_t value ) {
   return net_buff_encode_uint16( nb, (uint16_t)value );
}

bool net_buff_encode_uint32( net_buff nb, uint32_t value ) {
   if( nb == NULL ) {
      return false;
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return false;
   }
   if( This->position + sizeof( value ) > This->limit ) {
      return false;
   }
   uint32_t encoded_value = htonl( value );
   memcpy( This->buffer + This->position, &encoded_value, sizeof( encoded_value ));
   This->position += sizeof( encoded_value );
   return true;
}

bool net_buff_encode_int32( net_buff nb, int32_t value ) {
   return net_buff_encode_uint32( nb, (uint32_t)value );
}

bool net_buff_encode_uint64( net_buff nb, uint64_t value ) {
   if( nb == NULL ) {
      return false;
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return false;
   }
   if( This->position + sizeof( value ) > This->limit ) {
      return false;
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

bool net_buff_encode_int64( net_buff nb, int64_t value ) {
   return net_buff_encode_uint64( nb, (uint64_t)value );
}

bool net_buff_encode_float( net_buff nb, float src ) {
   void * ptr = &src;
   return net_buff_encode_uint32( nb, *(uint32_t*)ptr );
}

bool net_buff_encode_double( net_buff nb, double src ) {
   void * ptr = &src;
   return net_buff_encode_uint64( nb, *(uint64_t *)ptr );
}

bool net_buff_encode_string( net_buff nb, const char * string ) {
   if( nb == NULL ) {
      return false;
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return false;
   }
   size_t length = strlen( string );
   uint32_t encoded_length = htonl((uint32_t)length );
   if( This->position + sizeof( encoded_length ) > This->limit ) {
      return false;
   }
   memcpy( This->buffer + This->position, &encoded_length, sizeof( encoded_length ));
   This->position += sizeof( encoded_length );
   if( This->position + length > This->limit ) {
      return false;
   }
   memcpy( This->buffer + This->position, string, length );
   This->position += length;
   return true;
}

bool net_buff_flip( net_buff nb ) {
   if( nb == NULL ) {
      return false;
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return false;
   }
   This->limit = This->position;
   This->position = 0;
   return true;
}

bool net_buff_send( net_buff nb, int sckt, struct sockaddr_in * to ) {
   if( nb == NULL ) {
      return false;
   }
   if( to == NULL ) {
      return false;
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return false;
   }
   size_t  length = This->limit - This->position;
   ssize_t nbytes = sendto( sckt, This->buffer + This->position, length, 0, (struct sockaddr*)to, sizeof( struct sockaddr_in ));
   if( nbytes >= 0 ) {
      This->position += (size_t)nbytes;
      return ((size_t)nbytes) == length;
   }
   perror( "sendto" );
   return false;
}

bool net_buff_receive( net_buff nb, int sckt, struct sockaddr_in * from ) {
   if(( nb == NULL )||( from == NULL )) {
      fprintf( stderr, "%s: null argument\n", __func__ );
      return false;
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      fprintf( stderr, "%s: You must call net_buff_wrap first\n", __func__ );
      return false;
   }
   socklen_t addrlen = sizeof( struct sockaddr_in );
   size_t    length  = This->limit - This->position;
   ssize_t   nbytes  = recvfrom( sckt, This->buffer + This->position, length, 0, (struct sockaddr *)from, &addrlen );
   if( nbytes >= 0 ) {
      This->position += (size_t)nbytes;
      return true;
   }
   perror( "recvfrom" );
   return false;
}

bool net_buff_decode_boolean( net_buff nb, bool * dest ) {
   if( nb == NULL ) {
      return false;
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return false;
   }
   if( This->position >= This->limit ) {
      return false;
   }
   *dest = This->buffer[This->position] != 0;
   This->position += 1;
   return true;
}

bool net_buff_decode_byte( net_buff nb, byte * dest ) {
   if( nb == NULL ) {
      return false;
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return false;
   }
   if( This->position >= This->limit ) {
      return false;
   }
   *dest = This->buffer[This->position];
   This->position += 1;
   return true;
}

bool net_buff_decode_int8( net_buff nb, int8_t * dest ) {
   return net_buff_decode_byte( nb, (byte *)dest );
}

bool net_buff_decode_uint16( net_buff nb, uint16_t * dest ) {
   if( nb == NULL ) {
      return false;
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return false;
   }
   if( This->position >= This->limit ) {
      return false;
   }
   *dest = ntohs( *(uint16_t *)( This->buffer + This->position ));
   This->position += sizeof( *dest );
   return true;
}

bool net_buff_decode_int16( net_buff nb, int16_t * dest ) {
   return net_buff_decode_uint16( nb, (uint16_t *)dest );
}

bool net_buff_decode_uint32( net_buff nb, uint32_t * dest ) {
   if( nb == NULL ) {
      return false;
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return false;
   }
   if( This->position >= This->limit ) {
      return false;
   }
   *dest = ntohl( *(uint32_t *)( This->buffer + This->position ));
   This->position += sizeof( *dest );
   return true;
}

bool net_buff_decode_int32( net_buff nb, int32_t * dest ) {
   return net_buff_decode_uint32( nb, (uint32_t *)dest );
}

bool net_buff_decode_uint64( net_buff nb, uint64_t * dest ) {
   if( nb == NULL ) {
      return false;
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return false;
   }
   if( This->position >= This->limit ) {
      return false;
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

bool net_buff_decode_int64( net_buff nb, int64_t * dest ) {
   return net_buff_decode_uint64( nb, (uint64_t *)dest );
}

bool net_buff_decode_float( net_buff nb, float * dest ) {
   return net_buff_decode_uint32( nb, (uint32_t *)dest );
}

bool net_buff_decode_double( net_buff nb, double * dest ) {
   return net_buff_decode_uint64( nb, (uint64_t *)dest );
}

bool net_buff_decode_string( net_buff nb, char * target, size_t target_size ) {
   if( nb == NULL ) {
      return false;
   }
   net_buff_private * This = (net_buff_private *)nb;
   if( This->buffer == NULL ) {
      return false;
   }
   if( This->position >= This->limit ) {
      return false;
   }
   uint32_t length = ntohl( *(uint32_t *)( This->buffer + This->position ));
   if( This->position + sizeof( length ) > This->limit ) {
      return false;
   }
   This->position += sizeof( length );
   if( length >= target_size ) {
      return false;
   }
   strncpy( target, (char *)( This->buffer + This->position ), length );
   target[length] = '\0';
   This->position += length;
   return true;
}

bool net_buff_delete( net_buff * nb ) {
   if( nb == NULL ) {
      return false;
   }
   if( *nb ) {
      net_buff_private * This = (net_buff_private *)*nb;
      free( This->buffer );
      free( This );
      *nb = NULL;
   }
   return true;
}
