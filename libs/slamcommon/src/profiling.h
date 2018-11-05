#include <time.h>
#include <errno.h>

static double timespec_diff_to_sec(const timespec &from, const timespec &to) {
  timespec result;
  if ((to.tv_nsec - from.tv_nsec) < 0) {
    result.tv_sec = to.tv_sec - from.tv_sec - 1;
    result.tv_nsec = to.tv_nsec - from.tv_nsec + 1000000000;
  } else {
    result.tv_sec = to.tv_sec - from.tv_sec;
    result.tv_nsec = to.tv_nsec - from.tv_nsec;
  }    return result.tv_sec + (double)result.tv_nsec / 1000000000.0;
}

// static struct timespec read_time() {
//   struct timespec time;
//   int error;

//   error = clock_gettime(CLOCK_MONOTONIC, &time);
//   assert(!error);

//   return time;
// }
