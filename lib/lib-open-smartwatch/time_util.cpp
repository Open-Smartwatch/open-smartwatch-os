#include "time_util.h"

long seconds(long seconds) { return (seconds % 60); }

long minutes(long seconds) {
  long fh = (seconds / 3600);  // full hours
  return (((seconds - fh * 3600) / 60.0));
}
long hours24(long seconds) {
  long fd = (seconds / 3600) / 24;  // full days
  return ((seconds - fd * 24 * 3600) / 3600.0) * 30;
}