#include "all_tests.h"

#include <stdlib.h>

int main( int argc, char * argv[] ) {
   return tests_run( argc, argv,
      "utils_set"  , utils_set_test,
      "utils_prefs", utils_prefs_test,
      "utils_cli"  , utils_cli_test,
      NULL );
}
