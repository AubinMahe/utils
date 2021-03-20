#include <utils/utils_set.h>

#include "all_tests.h"

#include <string.h>

static bool for_each( unsigned index, const void * item, void * user_context ) {
   struct tests_report * report = (struct tests_report *)user_context;
   const bool unexpected_item = true;
   switch( index ) {
   case 0 : ASSERT( report, strcmp( item, "Aubin"  ), 0 ); return true;
   case 1 : ASSERT( report, strcmp( item, "Eve"    ), 0 ); return true;
   case 2 : ASSERT( report, strcmp( item, "Muriel" ), 0 ); return true;
   default: ASSERT( report, unexpected_item, false ); break;
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
   unsigned card = 0;
   utils_set set;
   ASSERT( report, utils_set_new     ( &set, person_compare )          , true  );
   ASSERT( report, utils_set_size    ( set, &card )                    , true  );
   ASSERT( report, card == 0                                           , true  );
   ASSERT( report, utils_set_contains( set, "Aubin" )                  , false );
   ASSERT( report, utils_set_add     ( set, "Muriel" )                 , true  );
   ASSERT( report, utils_set_add     ( set, "Eve" )                    , true  );
   ASSERT( report, utils_set_add     ( set, "Aubin" )                  , true  );
   ASSERT( report, utils_set_size    ( set, &card )                    , true  );
   ASSERT( report, card == 3                                           , true  );
   ASSERT( report, utils_set_contains( set, "Aubin" )                  , true  );
   ASSERT( report, utils_set_contains( set, "Muriel" )                 , true  );
   ASSERT( report, utils_set_contains( set, "Eve" )                    , true  );
   ASSERT( report, utils_set_contains( set, "Toto" )                   , false );
   ASSERT( report, utils_set_remove  ( set, "Eve", false )             , true  );
   ASSERT( report, utils_set_contains( set, "Eve" )                    , false );
   ASSERT( report, utils_set_size    ( set, &card )                    , true  );
   ASSERT( report, card == 2                                           , true  );
   ASSERT( report, utils_set_add     ( set, "Eve" )                    , true  );
   ASSERT( report, utils_set_contains( set, "Eve" )                    , true  );
   ASSERT( report, utils_set_size    ( set, &card )                    , true  );
   ASSERT( report, card == 3                                           , true  );
   ASSERT( report, utils_set_foreach ( set, for_each, report )         , true  );
   ASSERT( report, utils_set_replace ( set, "Aubin", "Albert" , false ), true  );
   ASSERT( report, utils_set_size    ( set, &card )                    , true  );
   ASSERT( report, card == 3                                           , true  );
   ASSERT( report, utils_set_contains( set, "Aubin" )                  , false );
   ASSERT( report, utils_set_contains( set, "Albert" )                 , true  );
   ASSERT( report, utils_set_remove  ( set, "Toto", false )            , false );
   ASSERT( report, utils_set_size    ( set, &card )                    , true  );
   ASSERT( report, card == 3                                           , true  );
   ASSERT( report, utils_set_remove  ( set, "Albert", false )          , true  );
   ASSERT( report, utils_set_remove  ( set, "Muriel", false )          , true  );
   ASSERT( report, utils_set_clear   ( set, false )                    , true  );
   ASSERT( report, utils_set_contains( set, "Aubin" )                  , false );
   ASSERT( report, utils_set_contains( set, "Muriel" )                 , false );
   ASSERT( report, utils_set_contains( set, "Eve" )                    , false );
   ASSERT( report, utils_set_contains( set, "Toto" )                   , false );
   ASSERT( report, utils_set_size    ( set, &card )                    , true  );
   ASSERT( report, card == 0                                           , true  );
   ASSERT( report, utils_set_delete  ( &set, false )                   , true  );
}
