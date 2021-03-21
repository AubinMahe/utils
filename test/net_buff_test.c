#include <net/net_buff.h>

#include "all_tests.h"

#include <math.h>
#include <string.h>

void net_buff_test( struct tests_report * report ) {
   tests_chapter( report, "Encoding to and decoding from the network" );
   net_buff nb       = NULL;
   size_t   capacity = 0U;
   size_t   limit    = 0U;
   size_t   position = 0U;
   bool     boolean_value;
   byte     byte_value;
   int8_t   int8_value;
   uint16_t uint16_value;
   int16_t  int16_value;
   uint32_t uint32_value;
   int32_t  int32_value;
   uint64_t uint64_value;
   int64_t  int64_value;
   float    float_value;
   double   double_value;
   char     string_value[80];
   ASSERT( report, net_buff_new( &nb, 1000 ));
   ASSERT( report, net_buff_get_capacity( nb, &capacity ));
   ASSERT( report, 1000 == capacity );
   ASSERT( report, net_buff_get_limit( nb, &limit ));
   ASSERT( report, 1000 == limit );
   ASSERT( report, net_buff_get_position( nb, &position ));
   ASSERT( report, 0 == position );
   ASSERT( report, net_buff_encode_boolean( nb,             true ));
   ASSERT( report, net_buff_encode_boolean( nb,            false ));
   ASSERT( report, net_buff_encode_byte   ( nb,        UINT8_MAX ));
   ASSERT( report, net_buff_encode_int8   ( nb,         INT8_MIN ));
   ASSERT( report, net_buff_encode_int8   ( nb,         INT8_MAX ));
   ASSERT( report, net_buff_encode_uint16 ( nb,       UINT16_MAX ));
   ASSERT( report, net_buff_encode_int16  ( nb,        INT16_MIN ));
   ASSERT( report, net_buff_encode_int16  ( nb,        INT16_MAX ));
   ASSERT( report, net_buff_encode_uint32 ( nb,       UINT32_MAX ));
   ASSERT( report, net_buff_encode_int32  ( nb,        INT32_MIN ));
   ASSERT( report, net_buff_encode_int32  ( nb,        INT32_MAX ));
   ASSERT( report, net_buff_encode_uint64 ( nb,       UINT64_MAX ));
   ASSERT( report, net_buff_encode_int64  ( nb,        INT64_MIN ));
   ASSERT( report, net_buff_encode_int64  ( nb,        INT64_MAX ));
   ASSERT( report, net_buff_encode_float  ( nb,      12345.6789f ));
   ASSERT( report, net_buff_encode_double ( nb,      12345.6789 ));
   ASSERT( report, net_buff_encode_string ( nb, "Hello, world!" ));
   ASSERT( report, net_buff_flip( nb ));
   ASSERT( report, net_buff_decode_boolean( nb, &boolean_value ));
   ASSERT( report,             true  == boolean_value );
   ASSERT( report, net_buff_decode_boolean( nb, &boolean_value ));
   ASSERT( report,             false == boolean_value );
   ASSERT( report, net_buff_decode_byte   ( nb, &byte_value ));
   ASSERT( report,         UINT8_MAX == byte_value );
   ASSERT( report, net_buff_decode_int8   ( nb, &int8_value ));
   ASSERT( report,          INT8_MIN == int8_value );
   ASSERT( report, net_buff_decode_int8   ( nb, &int8_value ));
   ASSERT( report,          INT8_MAX == int8_value );
   ASSERT( report, net_buff_decode_uint16 ( nb, &uint16_value ));
   ASSERT( report,        UINT16_MAX == uint16_value );
   ASSERT( report, net_buff_decode_int16  ( nb, &int16_value ));
   ASSERT( report,         INT16_MIN == int16_value );
   ASSERT( report, net_buff_decode_int16  ( nb, &int16_value ));
   ASSERT( report,         INT16_MAX == int16_value );
   ASSERT( report, net_buff_decode_uint32 ( nb, &uint32_value ));
   ASSERT( report,        UINT32_MAX == uint32_value );
   ASSERT( report, net_buff_decode_int32  ( nb, &int32_value ));
   ASSERT( report,         INT32_MIN == int32_value );
   ASSERT( report, net_buff_decode_int32  ( nb, &int32_value ));
   ASSERT( report,         INT32_MAX == int32_value );
   ASSERT( report, net_buff_decode_uint64 ( nb, &uint64_value ));
   ASSERT( report,        UINT64_MAX == uint64_value );
   ASSERT( report, net_buff_decode_int64  ( nb, &int64_value ));
   ASSERT( report,         INT64_MIN == int64_value );
   ASSERT( report, net_buff_decode_int64  ( nb, &int64_value ));
   ASSERT( report,         INT64_MAX == int64_value );
   ASSERT( report, net_buff_decode_float  ( nb, &float_value ));
   ASSERT( report, fabsf( 12345.6789f - float_value ) < 0.00001 );
   ASSERT( report, net_buff_decode_double ( nb, &double_value ));
   ASSERT( report, fabs( 12345.6789 - double_value ) < 0.00001 );
   ASSERT( report, net_buff_decode_string( nb, string_value, sizeof( string_value )));
   ASSERT( report, net_buff_get_position( nb, &position ));
   ASSERT( report, net_buff_get_limit( nb, &limit ));
   ASSERT( report, position == limit );
   ASSERT( report, net_buff_clear( nb ));
   ASSERT( report, net_buff_delete( &nb ));
}
