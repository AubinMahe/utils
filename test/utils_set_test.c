#include <utils/utils_set.h>

#include "all_tests.h"

#include <string.h>

static bool for_each( size_t index, const void * item, void * user_context ) {
   struct tests_report * report = (struct tests_report *)user_context;
   const bool unexpected_item = true;
   switch( index ) {
   case 0 :
      ASSERT( report, 0 == strcmp( item, "Aubin"  ));
      return true;
   case 1 :
      ASSERT( report, 0 == strcmp( item, "Eve"    ));
      return true;
   case 2 :
      ASSERT( report, 0 == strcmp( item, "Muriel" ));
      return true;
   default:
      ASSERT( report, unexpected_item == false );
      break;
   }
   return false;
}

static int person_compare( const void * l, const void * r ) {
   const char * left  = *(const char * const *)l;
   const char * right = *(const char * const *)r;
   return strcmp( left, right );
}

void utils_set_test( struct tests_report * report ) {
   tests_chapter( report, "Sorted set" );
   size_t card = 0;
   bool own = false;
   utils_set set;
   ASSERT( report, utils_set_new     ( &set, person_compare ));
   ASSERT( report, utils_get_size    ( set, &card ));
   ASSERT( report, card == 0);
   ASSERT( report, utils_set_contains( set, "Aubin", &own ));
   ASSERT( report, own == false );
   ASSERT( report, utils_set_add     ( set, "Muriel", false ));
   ASSERT( report, utils_set_add     ( set, "Eve"   , false ));
   ASSERT( report, utils_set_add     ( set, "Aubin" , false ));
   ASSERT( report, utils_get_size    ( set, &card ));
   ASSERT( report, card == 3);
   ASSERT( report, utils_set_contains( set, "Aubin" , &own ));
   ASSERT( report, own);
   ASSERT( report, utils_set_contains( set, "Muriel", &own ));
   ASSERT( report, own);
   ASSERT( report, utils_set_contains( set, "Eve"   , &own ));
   ASSERT( report, own);
   ASSERT( report, utils_set_contains( set, "Toto"  , &own ));
   ASSERT( report, own == false );
   ASSERT( report, utils_set_remove  ( set, "Eve", false ));
   ASSERT( report, utils_set_contains( set, "Eve", &own ));
   ASSERT( report, own == false );
   ASSERT( report, utils_get_size    ( set, &card ));
   ASSERT( report, card == 2);
   ASSERT( report, utils_set_add     ( set, "Eve", false ));
   ASSERT( report, utils_set_contains( set, "Eve", &own ));
   ASSERT( report, own);
   ASSERT( report, utils_get_size    ( set, &card ));
   ASSERT( report, card == 3);
   ASSERT( report, utils_set_foreach ( set, for_each, report ));
   ASSERT( report, utils_set_replace ( set, "Aubin", "Albert" , false ));
   ASSERT( report, utils_get_size    ( set, &card ));
   ASSERT( report, card == 3);
   ASSERT( report, utils_set_contains( set, "Aubin" , &own ));
   ASSERT( report, own == false );
   ASSERT( report, utils_set_contains( set, "Albert", &own ));
   ASSERT( report, own);
   ASSERT( report, utils_set_remove  ( set, "Toto", false ) == false );
   ASSERT( report, utils_get_size    ( set, &card ));
   ASSERT( report, card == 3);
   ASSERT( report, utils_set_remove  ( set, "Albert", false ));
   ASSERT( report, utils_set_remove  ( set, "Muriel", false ));
   ASSERT( report, utils_set_clear   ( set, false ));
   ASSERT( report, utils_set_contains( set, "Aubin" , &own ));
   ASSERT( report, own == false );
   ASSERT( report, utils_set_contains( set, "Muriel", &own ));
   ASSERT( report, own == false );
   ASSERT( report, utils_set_contains( set, "Eve"   , &own ));
   ASSERT( report, own == false );
   ASSERT( report, utils_set_contains( set, "Toto"  , &own ));
   ASSERT( report, own == false );
   ASSERT( report, utils_get_size    ( set, &card ));
   ASSERT( report, card == 0);
   ASSERT( report, utils_set_delete  ( &set, false ));
}
