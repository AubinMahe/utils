#include <utils/utils_time.h>
#include <time.h>

#define MILLIS_PER_SECS  1000U
#define NANOS_PER_MILLIS 1000000U

DLL_PUBLIC bool utils_sleep( unsigned milliseconds ) {
   const struct timespec t = {
      .tv_sec  = milliseconds / MILLIS_PER_SECS,
#ifdef __linux
      .tv_nsec = ( milliseconds % MILLIS_PER_SECS ) * NANOS_PER_MILLIS
#else
      .tv_nsec = (long)(( milliseconds % MILLIS_PER_SECS ) * NANOS_PER_MILLIS )
#endif
   };
   return nanosleep( &t, NULL ) == 0;
}
