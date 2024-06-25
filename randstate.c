// clang-format off
#include <stdio.h>
#include <gmp.h>
#include <stdint.h>
#include <stdlib.h>

#include "randstate.h"

// clang-format on

// gmp_randstate_t state;

void randstate_init(uint64_t seed) {

  // convert the seed into mpz
  srandom(seed);

  mpz_t seed_mpz;
  mpz_init_set_ui(seed_mpz, seed);

  gmp_randinit_mt(state);

  // initialize state with seed_mpz
  gmp_randseed(state, seed_mpz);
  mpz_clear(seed_mpz);
}

void randstate_clear(void) { gmp_randclear(state); }
