#include <utils/utils_time.h>

#include "all_tests.h"

void utils_time_test( struct tests_report * report ) {
   tests_chapter( report, "Time management features" );
   ASSERT( report, utils_sleep( 20U ));
}
