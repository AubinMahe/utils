#include <utils/utils_cli.h>

#include "all_tests.h"

#include <string.h>

void utils_cli_test( struct tests_report * report ) {
   tests_chapter( report, "Command Line Interface (CLI)" );
   utils_cli_arg  options   = NULL;
   const char *   group     = NULL;
   unsigned short port      = 0;
   int            argc      = 2;
   char           program[] = "test";
   char           address[] = "--group=239.0.0.66";
   char *         argv[]    = { program, address };
   ASSERT( report, utils_cli_new( &options, argc, argv,
      "group", utils_cli_STRING, true ,   NULL, &group,
      "port" , utils_cli_USHORT, false, "2416", &port, NULL ) , true );
   ASSERT( report, group && strcmp( group, "239.0.0.66" ) == 0, true );
   ASSERT( report, port == 2416                               , true );
   ASSERT( report, utils_cli_delete( &options )               , true );
}
