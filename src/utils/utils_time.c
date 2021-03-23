#include <utils/utils_time.h>
#include <time.h>

#define MILLIS_PER_SECS  1000U
#define NANOS_PER_MILLIS 1000000U

bool utils_sleep( unsigned milliseconds ) {
   const struct timespec t = {
      .tv_sec  = milliseconds / MILLIS_PER_SECS,
      .tv_nsec = ( milliseconds % MILLIS_PER_SECS ) * NANOS_PER_MILLIS
   };
   return nanosleep( &t, NULL ) == 0;
}
