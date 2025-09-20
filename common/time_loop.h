#include "../parlay/internal/get_time.h"
#include <vector>
#include <cstddef>

void clear_cache() {
  std::vector<char> arr(40*1024*1024);
  for (auto& x : arr) x=x;
}

template<class F, class G, class H>
void time_loop(int rounds, double delay, F initf, G runf, H endf) {
  parlay::internal::timer t;
  // run for delay seconds to "warm things up"
  // will skip if delay is zero
  while (t.total_time() < delay) {
    initf(); clear_cache(); runf(); endf();
  } 
  for (int i=0; i < rounds; i++) {
    initf();
    clear_cache();
    t.start();
    runf();
    t.next("");
    endf();
  }
}
