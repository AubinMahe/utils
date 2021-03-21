#include <utils/utils_prefs.h>

#include "all_tests.h"

#include <string.h>

void utils_prefs_test( struct tests_report * report ) {
   tests_chapter( report, "Preferences" );
   utils_prefs prefs = NULL;
   const char *   address = NULL;
   const char *   pseudo  = NULL;
   long           l_port  = 0;
   int            i_port  = 0;
   unsigned short us_port = 0;
   ASSERT( report, utils_prefs_load( &prefs, "n_existe_pas" ) == false );
   ASSERT( report, utils_prefs_get_string( prefs, "pseudo" , &pseudo  ) == false );
   ASSERT( report, pseudo == NULL );
   ASSERT( report, utils_prefs_get_string( prefs, "address", &address ) == false );
   ASSERT( report, address == NULL );
   ASSERT( report, utils_prefs_get_long  ( prefs, "port", &l_port  ) == false );
   ASSERT( report, l_port  == 0L );
   ASSERT( report, utils_prefs_get_int   ( prefs, "port", &i_port  ) == false );
   ASSERT( report, i_port  == 0 );
   ASSERT( report, utils_prefs_get_ushort( prefs, "port", &us_port ) == false );
   ASSERT( report, us_port == 0 );
   ASSERT( report, utils_prefs_delete( &prefs ));
   prefs   = NULL;
   address = NULL;
   pseudo  = NULL;
   l_port  = 0;
   i_port  = 0;
   us_port = 0;
   ASSERT( report, utils_prefs_load( &prefs, "chat" ));
   ASSERT( report, utils_prefs_get_string( prefs, "pseudo" , &pseudo  ));
   ASSERT( report, pseudo && strcmp( pseudo, "Le Titi-papatte" ) == 0);
   ASSERT( report, utils_prefs_get_string( prefs, "address", &address ));
   ASSERT( report, address && strcmp( address, "239.0.0.66" ) == 0);
   ASSERT( report, utils_prefs_get_long  ( prefs, "port", &l_port  ));
   ASSERT( report, l_port  == 2416L);
   ASSERT( report, utils_prefs_get_int   ( prefs, "port", &i_port  ));
   ASSERT( report, i_port  == 2416);
   ASSERT( report, utils_prefs_get_ushort( prefs, "port", &us_port ));
   ASSERT( report, us_port == 2416);
   pseudo  = NULL;
   l_port  = 0;
   i_port  = 0;
   us_port = 0;
   ASSERT( report, utils_prefs_get_string( prefs, "n'existe-pas", &pseudo  ) == false );
   ASSERT( report, pseudo == NULL);
   ASSERT( report, utils_prefs_get_long  ( prefs, "n'existe-pas", &l_port  ) == false );
   ASSERT( report, l_port  == 0);
   ASSERT( report, utils_prefs_get_int   ( prefs, "n'existe-pas", &i_port  ) == false );
   ASSERT( report, i_port  == 0);
   ASSERT( report, utils_prefs_get_ushort( prefs, "n'existe-pas", &us_port ) == false );
   ASSERT( report, us_port == 0);
   ASSERT( report, utils_prefs_delete( &prefs ));
}
