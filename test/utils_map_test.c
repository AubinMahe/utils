#include <utils/utils_map.h>

#include "all_tests.h"

#include <string.h>

static bool for_each( size_t index, map_pair pair, void * user_context ) {
   struct tests_report * report = (struct tests_report *)user_context;
   const bool unexpected_item = false;
   switch( index ) {
   case 0 :
      ASSERT( report, 0 == strcmp( pair.key  , "Aubin" ));
      ASSERT( report, 0 == strcmp( pair.value, "24/01/1966" ));
      return true;
   case 1 :
      ASSERT( report, 0 == strcmp( pair.key  , "Eve" ));
      ASSERT( report, 0 == strcmp( pair.value, "28/02/2008" ));
      return true;
   case 2 :
      ASSERT( report, 0 == strcmp( pair.key  , "Muriel" ));
      ASSERT( report, 0 == strcmp( pair.value, "26/01/1973" ));
      return true;
   default:
      ASSERT( report, unexpected_item );
      break;
   }
   return false;
}

static int name_compare( const void * l, const void * r ) {
   const char * left  = *(const char * const *)l;
   const char * right = *(const char * const *)r;
   return strcmp( left, right );
}

void utils_map_test( struct tests_report * report ) {
   tests_chapter( report, "Sorted map" );
   const char * eve_birthday    = "28/02/2008";
   const char * muriel_birthday = "26/01/1973";
   const char * aubin_birthday  = "24/01/1966";
   utils_map src;
   ASSERT( report, utils_map_new( &src, name_compare, false, false ));
   ASSERT( report, utils_map_put( src, "Muriel", muriel_birthday ));
   ASSERT( report, utils_map_put( src, "Eve"   , aubin_birthday  ));
   ASSERT( report, utils_map_put( src, "Eve"   , eve_birthday    ));
   ASSERT( report, utils_map_put( src, "Aubin" , aubin_birthday  ));
   size_t card = 0;
   ASSERT( report, utils_map_get_size( src, &card ));
   ASSERT( report, card == 3 );
   utils_map map;
   ASSERT( report, utils_map_new( &map, name_compare, false, false ));
   ASSERT( report, utils_map_get_size( map, &card ));
   ASSERT( report, card == 0 );
   bool own = false;
   ASSERT( report, utils_map_contains( map, "Aubin", &own ));
   ASSERT( report, own == false );
   char * data = NULL;
   ASSERT( report, utils_map_get( map, "Muriel", (void *)&data ) == false );
   ASSERT( report, data == NULL );
   ASSERT( report, utils_map_put( map, "Muriel", muriel_birthday ));
   ASSERT( report, utils_map_merge( map, src ));
   ASSERT( report, utils_map_get_size( map, &card ));
   ASSERT( report, card == 3 );
   ASSERT( report, utils_map_contains( map, "Aubin" , &own ));
   ASSERT( report, own == true );
   ASSERT( report, utils_map_contains( map, "Muriel", &own ));
   ASSERT( report, own == true );
   ASSERT( report, utils_map_contains( map, "Eve"   , &own ));
   ASSERT( report, own == true );
   ASSERT( report, utils_map_contains( map, "Toto"  , &own ));
   ASSERT( report, own == false );
   ASSERT( report, utils_map_get     ( map, "Muriel", (void *)&data ));
   ASSERT( report, data &&( 0 == strcmp( data, muriel_birthday )));
   ASSERT( report, utils_map_remove  ( map, "Eve" ));
   ASSERT( report, utils_map_contains( map, "Eve"   , &own ));
   ASSERT( report, own == false );
   ASSERT( report, utils_map_get_size( map, &card ));
   ASSERT( report, card == 2 );
   ASSERT( report, utils_map_put     ( map, "Eve"   , eve_birthday ));
   ASSERT( report, utils_map_contains( map, "Eve"   , &own ));
   ASSERT( report, own == true );
   ASSERT( report, utils_map_get_size( map, &card ));
   ASSERT( report, card == 3 );
   ASSERT( report, utils_map_foreach ( map, for_each, report ));
   map_key keys[3];
   size_t count = sizeof( keys ) / sizeof( keys[0] );
   bool ok = false;
   ASSERT( report, ok = utils_map_get_keys( map, keys, &count ));
   ASSERT( report, count == ( sizeof( keys ) / sizeof( keys[0] )));
   ASSERT( report, ( count > 0 )&&( 0 == strcmp( keys[0], "Aubin"  )));
   ASSERT( report, ( count > 1 )&&( 0 == strcmp( keys[1], "Eve"    )));
   ASSERT( report, ( count > 2 )&&( 0 == strcmp( keys[2], "Muriel" )));
   map_key values[3];
   count = sizeof( values ) / sizeof( values[0] );
   ASSERT( report, ok = utils_map_get_values( map, values, &count ));
   ASSERT( report, count == ( sizeof( values ) / sizeof( values[0] )));
   ASSERT( report, ( count > 0 )&&( 0 == strcmp( values[0], aubin_birthday  )));
   ASSERT( report, ( count > 1 )&&( 0 == strcmp( values[1], eve_birthday    )));
   ASSERT( report, ( count > 2 )&&( 0 == strcmp( values[2], muriel_birthday )));
   map_pair pairs[3];
   count = sizeof( pairs ) / sizeof( pairs[0] );
   ASSERT( report, ok = utils_map_get_entries( map, pairs, &count ));
   ASSERT( report, count == ( sizeof( pairs ) / sizeof( pairs[0] )));
   ASSERT( report, ( count > 0 )&&( 0 == strcmp( pairs[0].value, aubin_birthday  )));
   ASSERT( report, ( count > 1 )&&( 0 == strcmp( pairs[1].value, eve_birthday    )));
   ASSERT( report, ( count > 2 )&&( 0 == strcmp( pairs[2].value, muriel_birthday )));
   ASSERT( report, utils_map_remove  ( map, "Toto" ) == false );
   ASSERT( report, utils_map_get_size( map, &card ));
   ASSERT( report, card == 3 );
   ASSERT( report, utils_map_remove  ( map, "Albert" ) == false );
   ASSERT( report, utils_map_remove  ( map, "Muriel" ));
   ASSERT( report, utils_map_clear   ( map ));
   ASSERT( report, utils_map_contains( map, "Aubin" , &own ));
   ASSERT( report, own == false );
   ASSERT( report, utils_map_contains( map, "Muriel", &own ));
   ASSERT( report, own == false );
   ASSERT( report, utils_map_contains( map, "Eve"   , &own ));
   ASSERT( report, own == false );
   ASSERT( report, utils_map_contains( map, "Toto"  , &own ));
   ASSERT( report, own == false );
   ASSERT( report, utils_map_get_size( map, &card ));
   ASSERT( report, card == 0 );
   ASSERT( report, utils_map_delete  ( &map ));
   ASSERT( report, utils_map_delete  ( &src ));
}
