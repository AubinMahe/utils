#include "all_tests.h"

#include <stdlib.h>

int main( int argc, char * argv[] ) {
   return tests_run( argc, argv,
      "net_buff"   , net_buff_test,
      "utils_cli"  , utils_cli_test,
      "utils_map"  , utils_map_test,
      "utils_prefs", utils_prefs_test,
      "utils_set"  , utils_set_test,
      "utils_time" , utils_time_test,
      NULL );
}
