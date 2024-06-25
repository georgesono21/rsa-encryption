// clang-format off
#include <stdio.h>
#include <assert.h>
#include <gmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "randstate.h"
// clang-format on


void gcd(mpz_t d, mpz_t a, mpz_t b) {
  mpz_t temp; // declaring variables
  mpz_init(temp);

  // saving original variable values (as we will change them)
  mpz_t og_a;
  mpz_init_set(og_a, a);

  mpz_t og_b;
  mpz_init_set(og_b, b);

  if (mpz_cmp(a, b) == 0) // if a and b are the same
  {
    mpz_set(
        d,
        a); // greatest common factor of the same numbers is the number itself
    mpz_clear(og_a);
    mpz_clear(og_b);
    mpz_clear(temp);
    return;
  }

  while (mpz_cmp_ui(b, 0)) // while b is not zero
  {
    mpz_set(temp, b);
    mpz_mod(b, a, b);
    mpz_set(a, temp);
  }

  // set d to the gcd and clear mpz variables
  mpz_clear(temp);
  mpz_set(d, a);

  // set a amd b back to their original values
  mpz_set(a, og_a);
  mpz_set(b, og_b);

  mpz_clear(og_a);
  mpz_clear(og_b);
  return;
}

void pow_mod(mpz_t o, mpz_t a, mpz_t d, mpz_t n) {

  // saving original variable values (as we may change them)
  mpz_t og_a;
  mpz_init_set(og_a, a);
  mpz_t og_d;
  mpz_init_set(og_d, d);
  mpz_t og_n;
  mpz_init_set(og_n, n);

  if (mpz_cmp_ui(n, 0) == 0) // stop the program if the n is 0
  {
    printf("you cannot divide by zero (n is zero)");
    assert(0);
    return;
  }

  // declare v and p (as in the given pseudocode)

  // v=1
  mpz_t v;
  mpz_init_set_ui(v, 1);

  // p=a
  mpz_t p;
  mpz_init_set(p, a);

  while (mpz_cmp_ui(d, 0) > 0) {

    if (mpz_odd_p(d) > 0) { // if d is odd then the cmp will return 1
      mpz_t vxpmodn;
      mpz_init_set(vxpmodn, v); // start with v

      mpz_mul(vxpmodn, vxpmodn, p); // v x p
      mpz_mod(vxpmodn, vxpmodn, n); // (v x p) mod n

      mpz_set(v, vxpmodn); // set v to the result
      mpz_clear(vxpmodn);  // free vxpmodn
    }

    mpz_t pxpmodn;                      // pxpmodn = (p * p) mod n
    mpz_init_set(pxpmodn, p);           // start with p
    mpz_mul(pxpmodn, pxpmodn, pxpmodn); // p x p
    mpz_mod(pxpmodn, pxpmodn, n);       // (p x p) mod n

    mpz_set(p, pxpmodn); // set p to the result
    mpz_clear(pxpmodn);  // free pxpmodn

    mpz_t dfdiv2;
    mpz_init_set(dfdiv2, d);          // start with d
    mpz_fdiv_q_ui(dfdiv2, dfdiv2, 2); // floor div d by 2

    mpz_set(d, dfdiv2);
    mpz_clear(dfdiv2); // free dfdiv2
  }

  mpz_set(o, v); // set dest pointer as v (as we return v in psuedo code)

  // setting variables back to their original values in case they have been
  // changed
  mpz_set(a, og_a);
  mpz_set(d, og_d);
  mpz_set(n, og_n);

  mpz_clear(p);
  mpz_clear(v);
  mpz_clear(og_a);
  mpz_clear(og_d);
  mpz_clear(og_n);
}

void mod_inverse(mpz_t o, mpz_t a, mpz_t n) {
  // declare variables r, r', t, t'
  mpz_t r;
  mpz_init_set(r, n);
  mpz_t rprime;
  mpz_init_set(rprime, a);

  mpz_t t;
  mpz_init_set_ui(t, 0);
  mpz_t tprime;
  mpz_init_set_ui(tprime, 1);

  while (mpz_cmp_ui(rprime, 0) != 0) {

    // q = r/r'
    mpz_t rfdivrp;
    mpz_init_set(rfdivrp, r);             // set rfdiv as r which is the divided
    mpz_fdiv_q(rfdivrp, rfdivrp, rprime); // floor div r by r' (prime)
    mpz_t q;                              // creating q
    mpz_init_set(q, rfdivrp);             // setting q to quotient
    mpz_clear(rfdivrp);

    //(r,r') = (r', r - (q x r'))
    // creating a temp for r
    mpz_t prev_r;
    mpz_init_set(prev_r, r);

    // r = r'
    mpz_set(r, rprime);

    // r' = r - (q x r')
    mpz_t qxrprime;
    mpz_init_set(qxrprime, q);
    mpz_mul(qxrprime, qxrprime, rprime);

    mpz_t rminus_qxrprime;
    mpz_init_set(rminus_qxrprime, prev_r);
    mpz_sub(rminus_qxrprime, rminus_qxrprime, qxrprime);

    mpz_set(rprime, rminus_qxrprime); // r' = r - (q x r')

    mpz_clear(prev_r);
    mpz_clear(rminus_qxrprime);
    mpz_clear(qxrprime);

    //(t,t') = (t', t - (q x t'))
    // creating a temp for t
    mpz_t prev_t;
    mpz_init_set(prev_t, t);

    // t = t'
    mpz_set(t, tprime);

    // t' = t - (q x t')
    mpz_t qxtprime;
    mpz_init_set(qxtprime, q);
    mpz_mul(qxtprime, qxtprime, tprime);

    mpz_t tminus_qxtprime;
    mpz_init_set(tminus_qxtprime, prev_t);
    mpz_sub(tminus_qxtprime, tminus_qxtprime, qxtprime);

    mpz_set(tprime, tminus_qxtprime); // t' = t - (q x t')

    mpz_clear(prev_t);
    mpz_clear(tminus_qxtprime);
    mpz_clear(qxtprime);

    mpz_clear(q);
  }

  if (mpz_cmp_ui(r, 1) > 0) {
    mpz_set_ui(o, 0); // if no modular inverse is found, set o to 0

    mpz_clear(r);
    mpz_clear(rprime);

    mpz_clear(t);
    mpz_clear(tprime);

    return;
  }

  if (mpz_cmp_ui(t, 0) < 0) {
    // t = t + n
    mpz_t tplusn;

    mpz_init(tplusn);
    mpz_set(tplusn, t);
    mpz_add(tplusn, tplusn, n);

    mpz_set(t, tplusn);
    mpz_clear(tplusn);
  }

  mpz_set(o, t);

  mpz_clear(r);
  mpz_clear(rprime);

  mpz_clear(t);
  mpz_clear(tprime);

  return;
}

bool is_prime(mpz_t n, uint64_t iters) {

  // is_prime does not work for numbers [0~3] so i will hardcode them
  // does this matter though? not really, with a min bit size of 50 for p and q
  // (in keygen), these numbers will likely not be tested

  if ((mpz_cmp_ui(n, 0) == 0) || (mpz_cmp_ui(n, 1) == 0)) {
    // printf("hardcode 0-1\n");
    return false;
  } else if (mpz_cmp_ui(n, 3) <= 0) {
    // printf("hardcode 2-3\n");
    return true;
  }

  if (mpz_even_p(n) >
      0) { // if n is even we return false (optimization power move)
    return false;
  }

  mpz_t n_1; // defining n - 1
  mpz_init(n_1);

  mpz_sub_ui(n_1, n, 1);

  // defining s and r for miller rabin algorithim

  mpz_t s;
  mpz_init_set_ui(s, 0);

  mpz_t r;
  mpz_init_set(r, n_1);

  mpz_t m; // temp variable for r which will be used for calculating s and r
           // values
  mpz_init_set(m, r);

  while (mpz_even_p(r) > 0) { // while r is even
    // credit to TA Zack Jorequera for psuedocode (refer to README for more
    // info)
    if (mpz_odd_p(m) > 0) { // if m is odd
      mpz_set(r, m);
      break;
    } else {
      mpz_t s_1; // s+1
      mpz_init_set(s_1, s);
      mpz_add_ui(s_1, s_1, 1);
      mpz_set(s, s_1);
      mpz_clear(s_1);

      mpz_t m_div_2; // m/2
      mpz_init_set(m_div_2, m);
      mpz_fdiv_q_ui(m_div_2, m_div_2, 2);
      mpz_set(m, m_div_2);
      mpz_clear(m_div_2);
    }
  }

  mpz_clear(m); // clear the temp variable

  for (uint64_t i = 1; i < iters; i++) {
    mpz_t rand_num;
    mpz_init(rand_num);
    mpz_urandomm(rand_num, state, n);

    mpz_t n_2; // n-2
    mpz_init(n_2);
    mpz_sub_ui(n_2, n, 2);

    if ((mpz_cmp_ui(rand_num, 2) < 0) ||
        (mpz_cmp(rand_num, n_2) >
         0)) // making sure we print a number in range [2,n-2]
    {

      bool not_in_range = true;
      while (not_in_range) {
        mpz_urandomm(rand_num, state, n);

        if ((mpz_cmp_ui(rand_num, 2) >= 0) && (mpz_cmp(rand_num, n_2) <= 0)) {
          not_in_range = false;
          break;
        }
      }
    }
    mpz_clear(n_2);

    // setup miller rabin algorithim variables
    mpz_t a;
    mpz_init_set(a, rand_num);
    mpz_clear(rand_num);

    mpz_t y;
    mpz_init(y);
    pow_mod(y, a, r, n); // y = pow_mod(a,r,n)

    if ((mpz_cmp_ui(y, 1) != 0) && (mpz_cmp(y, n_1) != 0)) {

      mpz_t j;
      mpz_init_set_ui(j, 1); // j =1

      mpz_t s_1; // s - 1
      mpz_init(s_1);
      mpz_sub_ui(s_1, s, 1);

      while ((mpz_cmp(j, s_1) <= 0) && (mpz_cmp(y, n_1) != 0)) {
        mpz_t num_2; // literally the number 2 (used as pow_mod does not take in
                     // unsigned longs)
        mpz_init_set_ui(num_2, 2);

        mpz_t ypowm;
        mpz_init(ypowm);
        pow_mod(ypowm, y, num_2, n);
        mpz_set(y, ypowm);

        mpz_clear(ypowm);
        mpz_clear(num_2);

        if (mpz_cmp_ui(y, 1) == 0) // if y = 1, return false
        {

          mpz_clear(s);
          mpz_clear(r);

          mpz_clear(a);
          mpz_clear(y);
          mpz_clear(j);
          mpz_clear(s_1);
          mpz_clear(n_1);

          return false;
        }
        mpz_add_ui(j, j, 1); // j = j +1
      }

      if (mpz_cmp(y, n_1) != 0) // if y != n-1, return false
      {
        mpz_clear(s);
        mpz_clear(r);

        mpz_clear(a);
        mpz_clear(y);
        mpz_clear(j);
        mpz_clear(s_1);
        mpz_clear(n_1);

        return false;
      }

      mpz_clear(j);
      mpz_clear(s_1);
    }
    mpz_clear(a);
    mpz_clear(y);
  }
  mpz_clear(s);
  mpz_clear(r);
  mpz_clear(n_1);
  return true;
}

void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
  bool not_prime = true;
  while (not_prime) // keep on generating numbers of nbis until they are prime
  {
    mpz_t rand_number;
    mpz_init(rand_number);

    mpz_urandomb(rand_number, state, bits);
    if (is_prime(rand_number, iters) &&
        (bits ==
         mpz_sizeinbase(rand_number,
                        2))) { // sizeinbase checks if theyre atleast bits long
      mpz_set(p, rand_number);
      mpz_clear(rand_number);
      return;
    }
    mpz_clear(rand_number);
  }
}
