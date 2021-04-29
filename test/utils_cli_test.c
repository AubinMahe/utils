#include <utils/utils_cli.h>

#include "all_tests.h"

#include <stdint.h>
#include <string.h>

void utils_cli_test( struct tests_report * report ) {
   tests_chapter( report, "Command Line Interface (CLI)" );
   //------------------------------------------------------------
   bool          b         = false;
   int16_t       i16       = 0;
   uint16_t      ui16      = 0;
   int32_t       i32       = 0;
   uint32_t      ui32      = 0;
   int64_t       i64       = 0;
   uint64_t      ui64      = 0;
   float         f32       = 0.0f;
   double        d64       = 0.0;
   const char *  string    = NULL;
   //------------------------------------------------------------
   char          program[] = "test";
   char          address[] = "--group=239.0.0.66";
   char *        argv[]    = { program, address };
   int           argc      = sizeof(argv)/sizeof(argv[0]);
   utils_cli_arg options   = NULL;
   //------------------------------------------------------------
   ASSERT( report, utils_cli_new( &options, argc, argv,
      "group", utils_cli_string, NULL  , &string,
      "port" , utils_cli_uint16, "2416", &ui16,
      NULL ));
   ASSERT( report, string && strcmp( string, "239.0.0.66" ) == 0 );
   ASSERT( report, ui16 == 2416 );
   ASSERT( report, utils_cli_delete( &options ));
   //------------------------------------------------------------
   char   bool_option[] = "--bool=true";
   char * argv2[]       = { program, bool_option };
   argc = sizeof(argv2)/sizeof(argv2[0]);
   ASSERT( report, utils_cli_new( &options, argc, argv2,
      "bool"   , utils_cli_boolean, "false", &b,
      "int16"  , utils_cli_int16  , "1"    , &i16,
      "uint16" , utils_cli_uint16 , "2"    , &ui16,
      "int32"  , utils_cli_int32  , "3"    , &i32,
      "uint32" , utils_cli_uint32 , "4"    , &ui32,
      "int64"  , utils_cli_int64  , "5"    , &i64,
      "uint64" , utils_cli_uint64 , "6"    , &ui64,
      "float"  , utils_cli_float  , "7.890", &f32,
      "double" , utils_cli_double , "8.901", &d64,
      "string" , utils_cli_string , "Aubin", &string,
      NULL ));
   ASSERT( report, b );
   ASSERT( report, i16  == 1 );
   ASSERT( report, ui16 == 2 );
   ASSERT( report, i32  == 3 );
   ASSERT( report, ui32 == 4 );
   ASSERT( report, i64  == 5 );
   ASSERT( report, ui64 == 6 );
   ASSERT( report, f32 - 7.890 < 0.001 );
   ASSERT( report, d64 - 8.901 < 0.001 );
   ASSERT( report, strcmp( string, "Aubin" ) == 0 );
   ASSERT( report, utils_cli_delete( &options ));
   //------------------------------------------------------------
   char i16_opt   [] = "--int16=1";
   char ui16_opt  [] = "--uint16=2";
   char i32_opt   [] = "--int32=3";
   char ui32_opt  [] = "--uint32=4";
   char i64_opt   [] = "--int64=5";
   char ui64_opt  [] = "--uint64=6";
   char float_opt [] = "--float=7.890";
   char double_opt[] = "--double=8.901";
   char string_opt[] = "--string=Aubin";
   char * argv3[] = {
      program, bool_option, i16_opt, ui16_opt, i32_opt, ui32_opt, i64_opt, ui64_opt, float_opt, double_opt, string_opt
   };
   argc = sizeof(argv3)/sizeof(argv3[0]);
   ASSERT( report, utils_cli_new( &options, argc, argv3,
      "bool"   , utils_cli_boolean, "false", &b,
      "int16"  , utils_cli_int16  , "0"    , &i16,
      "uint16" , utils_cli_uint16 , "0"    , &ui16,
      "int32"  , utils_cli_int32  , "0"    , &i32,
      "uint32" , utils_cli_uint32 , "0"    , &ui32,
      "int64"  , utils_cli_int64  , "0"    , &i64,
      "uint64" , utils_cli_uint64 , "0"    , &ui64,
      "float"  , utils_cli_float  , "0"    , &f32,
      "double" , utils_cli_double , "0"    , &d64,
      "string" , utils_cli_string , "Toto" , &string,
      NULL ));
   ASSERT( report, b );
   ASSERT( report, i16  == 1 );
   ASSERT( report, ui16 == 2 );
   ASSERT( report, i32  == 3 );
   ASSERT( report, ui32 == 4 );
   ASSERT( report, i64  == 5 );
   ASSERT( report, ui64 == 6 );
   ASSERT( report, f32 - 7.890 < 0.001 );
   ASSERT( report, d64 - 8.901 < 0.001 );
   ASSERT( report, strcmp( string, "Aubin" ) == 0 );
   ASSERT( report, utils_cli_delete( &options ));
}
