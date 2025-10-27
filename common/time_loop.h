#include "../parlay/internal/get_time.h"
#include <time.h>

// Function to find the next power of 2
size_t next_power_of_2(size_t n) {
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

void clear_cache() {
    const size_t size = 40 * 1024 * 1024;

    // 1. Allocate the main array on the heap.
    char *arr = (char *)malloc(size * sizeof(char));
    if (!arr) {
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
    for (size_t i = 0; i < m; i++) {
        // Calculate the next index in the sequence
        x = a * x + c; 
        
        // Use the higher bits for better randomness and scale down.
        // This is equivalent to (x % m) when m is a power of 2.
        size_t index = x & (m - 1); 

        // Ensure the generated index is within our array bounds.
        if (index < size) {
            temp_val = arr[index];
            // arr[index] = 0; // Optional write
        }
    }
    
    // 4. Clean up.
    free(arr);
    // printf("Cache thrashed using LCG method.\n");
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
