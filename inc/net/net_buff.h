#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <netinet/in.h>

#include <utils/utils_visibility.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char byte;

typedef struct { unsigned unused; } * net_buff;

DLL_PUBLIC bool net_buff_new           ( net_buff * nb, size_t   capacity );
DLL_PUBLIC bool net_buff_get_capacity  ( net_buff   nb, size_t * capacity );
DLL_PUBLIC bool net_buff_get_limit     ( net_buff   nb, size_t * limit );
DLL_PUBLIC bool net_buff_get_position  ( net_buff   nb, size_t * position );
DLL_PUBLIC bool net_buff_dump          ( net_buff   nb, char * dest, size_t dest_size );
DLL_PUBLIC bool net_buff_clear         ( net_buff   nb );
DLL_PUBLIC bool net_buff_encode_boolean( net_buff   nb, bool         src );
DLL_PUBLIC bool net_buff_encode_byte   ( net_buff   nb, byte         src );
DLL_PUBLIC bool net_buff_encode_int8   ( net_buff   nb, int8_t       src );
DLL_PUBLIC bool net_buff_encode_uint16 ( net_buff   nb, uint16_t     src );
DLL_PUBLIC bool net_buff_encode_int16  ( net_buff   nb, int16_t      src );
DLL_PUBLIC bool net_buff_encode_uint32 ( net_buff   nb, uint32_t     src );
DLL_PUBLIC bool net_buff_encode_int32  ( net_buff   nb, int32_t      src );
DLL_PUBLIC bool net_buff_encode_uint64 ( net_buff   nb, uint64_t     src );
DLL_PUBLIC bool net_buff_encode_int64  ( net_buff   nb, int64_t      src );
DLL_PUBLIC bool net_buff_encode_float  ( net_buff   nb, float        src );
DLL_PUBLIC bool net_buff_encode_double ( net_buff   nb, double       src );
DLL_PUBLIC bool net_buff_encode_string ( net_buff   nb, const char * src );
DLL_PUBLIC bool net_buff_flip          ( net_buff   nb );
DLL_PUBLIC bool net_buff_send          ( net_buff   nb, int sckt, struct sockaddr_in * to );
DLL_PUBLIC bool net_buff_receive       ( net_buff   nb, int sckt, struct sockaddr_in * from );
DLL_PUBLIC bool net_buff_decode_boolean( net_buff   nb, bool *     dest );
DLL_PUBLIC bool net_buff_decode_byte   ( net_buff   nb, byte *     dest );
DLL_PUBLIC bool net_buff_decode_int8   ( net_buff   nb, int8_t *   dest );
DLL_PUBLIC bool net_buff_decode_uint16 ( net_buff   nb, uint16_t * dest );
DLL_PUBLIC bool net_buff_decode_int16  ( net_buff   nb, int16_t *  dest );
DLL_PUBLIC bool net_buff_decode_uint32 ( net_buff   nb, uint32_t * dest );
DLL_PUBLIC bool net_buff_decode_int32  ( net_buff   nb, int32_t *  dest );
DLL_PUBLIC bool net_buff_decode_uint64 ( net_buff   nb, uint64_t * dest );
DLL_PUBLIC bool net_buff_decode_int64  ( net_buff   nb, int64_t *  dest );
DLL_PUBLIC bool net_buff_decode_float  ( net_buff   nb, float *    dest );
DLL_PUBLIC bool net_buff_decode_double ( net_buff   nb, double *   dest );
DLL_PUBLIC bool net_buff_decode_string ( net_buff   nb, char *     dest, size_t dest_size );
DLL_PUBLIC bool net_buff_delete        ( net_buff * nb );

#ifdef __cplusplus
}
#endif
