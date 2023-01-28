#include "rnd.h"

namespace RND
{
  __uint128_t g_lehmer64_state = 1337;

  uint64_t lehmer64() {
    g_lehmer64_state *= 0xda942042e4dd58b5;
    return g_lehmer64_state >> 64;
  }
  // https://www.codingame.com/playgrounds/58096/how-to-randomize-bits-and-integers-efficiently-in-c

  double convert_uint64_to_double(uint64_t value)
  {
    uint64_t u64 = 0x3FF0000000000000ULL | ((value >> 12) | 1) ;
    return *(double*)&u64 - 1.0;
  }
  // https://stackoverflow.com/questions/52147419/how-to-convert-random-uint64-t-to-random-double-in-range-0-1-using-bit-wise-o

  double RandomDouble()
  {
    return convert_uint64_to_double(lehmer64());
  }

  uint64_t RandomInt()
  {
    return lehmer64();
  }
};


