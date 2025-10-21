#include "../parlay/internal/get_time.h"
#include <time.h>
#include <papi.h>
#include <string.h>
#include <iomanip>

#define MAX_EVENTS 6

// Function to find the next power of 2
size_t next_power_of_2(size_t n)
{
  n--;
  n |= n >> 1;
  n |= n >> 2;
  n |= n >> 4;
  n |= n >> 8;
  n |= n >> 16;
  n |= n >> 32; // For 64-bit systems
  n++;
  return n;
}

void clear_cache()
{
  const size_t size = 40 * 1024 * 1024;

  // 1. Allocate the main array on the heap.
  char *arr = (char *)malloc(size * sizeof(char));
  if (!arr)
  {
    perror("Failed to allocate arr");
    return;
  }

  // 2. Set up the LCG.
  // We need a modulus 'm' that is a power of 2 and >= size.
  const size_t m = next_power_of_2(size);
  const size_t a = 6364136223846793005ULL; // A good multiplier for 64-bit
  const size_t c = 1442695040888963407ULL; // A good increment (must be odd)

  // Start with a random seed.
  srand(time(NULL));
  size_t x = rand();

  // 3. Iterate and access the array in a pseudo-random order.
  volatile char temp_val;
  for (size_t i = 0; i < m; i++)
  {
    // Calculate the next index in the sequence
    x = a * x + c;

    // Use the higher bits for better randomness and scale down.
    // This is equivalent to (x % m) when m is a power of 2.
    size_t index = x & (m - 1);

    // Ensure the generated index is within our array bounds.
    if (index < size)
    {
      temp_val = arr[index];
      // arr[index] = 0; // Optional write
    }
  }

  // 4. Clean up.
  free(arr);
  // printf("Cache thrashed using LCG method.\n");
}

void handle_error(int retval, int return_code, const char *str)
{
  if (retval != return_code)
  {
    printf("PAPI error %d: %s, %s\n", retval, PAPI_strerror(retval), str);
    exit(1);
  }
}

template <class F, class G, class H>
void time_loop(int rounds, double delay, F initf, G runf, H endf)
{

  int retval, event_set = PAPI_NULL;
  int event_code;
  long long hw_values[MAX_EVENTS] = {0};
  long double totals[MAX_EVENTS] = {0};
  int events[MAX_EVENTS] = {0};

  double l1_i_mpki = 0;
  double l1_d_mpki = 0;
  double l2_mpki = 0;
  double llc_mpki = 0;

  // PAPI initialize
  retval = PAPI_library_init(PAPI_VER_CURRENT);
  handle_error(retval, PAPI_VER_CURRENT, "PAPI_library_init");

  // Initialize event set
  retval = PAPI_create_eventset(&event_set);
  handle_error(retval, PAPI_OK, "PAPI_create_eventset");

  // Add events
  // retval = PAPI_event_name_to_code("perf::L1-DCACHE-LOAD-MISSES", &events[0]);
  // handle_error(retval, PAPI_OK, "perf::L1-DCACHE-LOAD-MISSES");

  // retval = PAPI_event_name_to_code("perf::L1-DCACHE-STORE-MISSES", &events[1]);
  // handle_error(retval, PAPI_OK, "perf::L1-DCACHE-STORE-MISSES");

  // perf::PERF_COUNT_HW_CACHE_L1I
  retval = PAPI_event_name_to_code("perf::PERF_COUNT_HW_CACHE_L1I:MISS", &events[MAX_EVENTS - 6]);
  handle_error(retval, PAPI_OK, "perf::PERF_COUNT_HW_CACHE_L1I:MISS");

  retval = PAPI_event_name_to_code("perf::PERF_COUNT_HW_CACHE_L1D:MISS", &events[MAX_EVENTS - 5]);
  handle_error(retval, PAPI_OK, "perf::PERF_COUNT_HW_CACHE_L1D:MISS");

  retval = PAPI_event_name_to_code("L2_RQSTS:MISS", &events[MAX_EVENTS - 4]);
  handle_error(retval, PAPI_OK, "L2_RQSTS:MISS");

  retval = PAPI_event_name_to_code("ix86arch::LLC_MISSES", &events[MAX_EVENTS - 3]);
  handle_error(retval, PAPI_OK, "ix86arch::LLC_MISSES");

  retval = PAPI_event_name_to_code("perf::PERF_COUNT_HW_INSTRUCTIONS", &events[MAX_EVENTS - 2]);
  handle_error(retval, PAPI_OK, "perf::PERF_COUNT_HW_INSTRUCTIONS");

  retval = PAPI_event_name_to_code("perf::CYCLES", &events[MAX_EVENTS - 1]);
  handle_error(retval, PAPI_OK, "perf::CYCLES");

  for (int i = 0; i < MAX_EVENTS; i++)
  {
    std::string error_message = "add event " + std::to_string(i);
    retval = PAPI_add_event(event_set, events[i]);
    handle_error(retval, PAPI_OK, error_message.c_str());
  }

  parlay::internal::timer t;
  // run for delay seconds to "warm things up"
  // will skip if delay is zero
  while (t.total_time() < delay)
  {
    initf();
    clear_cache();
    runf();
    endf();
  }
  for (int i = 0; i < rounds; i++)
  {
    initf();

    clear_cache();

    t.start();

    retval = PAPI_start(event_set);
    handle_error(retval, PAPI_OK, "PAPI_start in loop");

    runf();

    retval = PAPI_stop(event_set, hw_values);
    handle_error(retval, PAPI_OK, "PAPI_stop in loop");

    // for (int k = 0; k < MAX_EVENTS; ++k)
    // {
    //   assert(hw_values[k] != 0);
    // }

    t.next("");

    endf();

    for (int j = 0; j < MAX_EVENTS; j++)
    {
      totals[j] += hw_values[j];
    }
  }
  // if (totals[MAX_EVENTS - 2] > 0)
  //   printf("-> %Lf\n", totals[MAX_EVENTS - 2]);
  // long long l1_d_misses = totals[0] + totals[1];
  l1_i_mpki = (totals[MAX_EVENTS - 6] * 1000) / totals[MAX_EVENTS - 2];
  l1_d_mpki = (totals[MAX_EVENTS - 5] * 1000) / totals[MAX_EVENTS - 2];
  l2_mpki = (totals[MAX_EVENTS - 4] * 1000) / totals[MAX_EVENTS - 2];
  llc_mpki = (totals[MAX_EVENTS - 3] * 1000) / totals[MAX_EVENTS - 2];

  std::cout << std::fixed << std::setprecision(5);
  std::cout << "Event L1_I_MPKI  = " << l1_i_mpki << std::endl;
  std::cout << "Event L1_D_MPKI  = " << l1_d_mpki << std::endl;
  std::cout << "Event L2_MPKI  = " << l2_mpki << std::endl;
  std::cout << "Event LLC_MPKI  = " << llc_mpki << std::endl;
  std::cout << "Event # Instructions  = " << totals[MAX_EVENTS - 2] / rounds << std::endl;
  std::cout << "Event # Cycles  = " << totals[MAX_EVENTS - 1] / rounds << std::endl;
}
