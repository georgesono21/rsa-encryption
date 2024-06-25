
// clang-format off
#include <stdio.h>
#include <assert.h>
#include <gmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "numtheory.h"
#include "randstate.h"
// clang-format on

void lambda(mpz_t n, mpz_t p,
            mpz_t q) // helper function for calculating lambda(n)
{
  mpz_t totp; // tot(p) = p -1
  mpz_init(totp);
  mpz_sub_ui(totp, p, 1);

  mpz_t totq; // tot(q) = q -1
  mpz_init(totq);
  mpz_sub_ui(totq, q, 1);

  mpz_t totqxtotp;
  ; // (p-1) (q-1)
  mpz_init(totqxtotp);
  mpz_mul(totqxtotp, totq, totp);

  mpz_t gcd_totp_totq; // gcd(tot(p), tot(q))
  mpz_init(gcd_totp_totq);
  gcd(gcd_totp_totq, totp, totq);

  mpz_t quotient; // equal to lcm(tot(p), tot(q))
  mpz_init(quotient);
  mpz_fdiv_q(quotient, totqxtotp, gcd_totp_totq);

  mpz_set(n, quotient); // set destination variable equal to quotient

  // clear created mpz_t vars
  // printf("clearing mpz extras\n");
  mpz_clear(totp);
  mpz_clear(totq);
  mpz_clear(totqxtotp);
  mpz_clear(gcd_totp_totq);
  mpz_clear(quotient);
}

void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits,
                  uint64_t iters) {
  uint64_t p_upper =
      (3 * nbits / 4); // credit to TA Sanjana Patil that helped me understand
                       // how pbits and qbits are derived from nbits
  uint64_t p_lower = (nbits / 4);

  uint64_t pbits = (random() % (p_upper - p_lower)) +
                   p_lower; // RNG code based on code from Tutor Ben Grant
  uint64_t qbits = nbits - pbits;

  // making p,q, n (p x q)
  make_prime(p, pbits, iters);
  make_prime(q, qbits, iters);
  mpz_mul(n, p, q);

  mpz_t lambda_n; // carmichael function
  mpz_init(lambda_n);
  lambda(lambda_n, p, q);

  uint64_t counter = 0;
  while (counter < nbits) // iterating for "around nbits"
  {
    mpz_t rand_num;
    mpz_init(rand_num);
    mpz_urandomb(rand_num, state, nbits);

    mpz_t gcd_rand_lambda_n;
    mpz_init(gcd_rand_lambda_n);
    gcd(gcd_rand_lambda_n, rand_num, lambda_n);

    if (mpz_cmp_ui(gcd_rand_lambda_n, 1) == 0) // rand_num and n are coprime
    {

      mpz_set(e, rand_num); // rand_num is the public exponent
      mpz_clear(gcd_rand_lambda_n);
      mpz_clear(rand_num);
      mpz_clear(lambda_n);
      return;
    }
    mpz_clear(gcd_rand_lambda_n);
    mpz_clear(rand_num);
    counter += 1;
  }
  mpz_clear(lambda_n);
  return;
}

void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
  // writing to pbfile, setting file stream to pbfile file pointer
  gmp_fprintf(pbfile, "%Zx\n", n);
  gmp_fprintf(pbfile, "%Zx\n", e);
  gmp_fprintf(pbfile, "%Zx\n", s);
  gmp_fprintf(pbfile, "%s\n", username);
}

void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
  // reading and storing variables from pbfile, setting file stream to pbfile
  // file pointer
  gmp_fscanf(pbfile, "%Zx\n", n);
  gmp_fscanf(pbfile, "%Zx\n", e);
  gmp_fscanf(pbfile, "%Zx\n", s);
  gmp_fscanf(pbfile, "%s\n", username);
}

void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {
  // private key is the inverse of (e mod lambda(n))
  mpz_t lambda_n;
  mpz_init(lambda_n);
  lambda(lambda_n, p, q);
  mod_inverse(d, e, lambda_n);
  mpz_clear(lambda_n);
}

void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {
  // writing to pvfile, setting file stream to pvfile file pointer
  gmp_fprintf(pvfile, "%Zx\n", n);
  gmp_fprintf(pvfile, "%Zx\n", d);
}

void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {
  // reading and storing variables from pvfile, setting file stream to pvfile
  // file pointer
  gmp_fscanf(pvfile, "%Zx\n", n);
  gmp_fscanf(pvfile, "%Zx\n", d);
}

void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) { pow_mod(c, m, e, n); }

void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {
  uint64_t k = (mpz_sizeinbase(n, 2) - 1) /
               8; // finding the size of each block (must be less than n)
  uint8_t *kblock = (uint8_t *)calloc(
      k, sizeof(uint8_t)); // creating an empty array of size k

  kblock[0] = 0xFF; // setting the first index of kblock to 0xFF to avoid
                    // encrypting issues

  while (1) // while true (we will break out manually)
  {

    // j = numbers of bytes read (fread returns bytes read)
    size_t j = fread(kblock + 1, sizeof(uint8_t), k - 1, infile);
    mpz_t message;
    mpz_init(message);
    mpz_import(message, j + 1, 1, sizeof(uint8_t), 1, 0,
               kblock); // we do j+1 because we want to

    mpz_t cipher;
    mpz_init(cipher);

    rsa_encrypt(cipher, message, e, n);

    gmp_fprintf(outfile, "%Zx\n", cipher);

    mpz_clear(message);
    mpz_clear(cipher);
    if (j < (k - 1)) // if j is less than k-1, than it means that we read the
                     // finaly block of the file
    {
      break;
    }
  }

  free(kblock);
}

void rsa_decrypt(
    mpz_t m, mpz_t c, mpz_t d,
    mpz_t n) // for rsa, encrypt and decrypt use the same function (pow_mod)
{
  pow_mod(m, c, d, n);
}

void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {

  uint64_t k = (mpz_sizeinbase(n, 2) - 1) / 8; // same thing as encrypt_file
  uint8_t *kblock = (uint8_t *)calloc(
      k, sizeof(uint8_t)); // creating an empty array of size k

  while (1) // while true (we will break out manually)
  {

    mpz_t cipher;
    mpz_init(cipher);

    // scan in a block of text and store into cipher (mpz)
    gmp_fscanf(infile, "%Zx\n", cipher);

    size_t bytes_read; // used to read how many bytes were read in mpz_export

    mpz_t deciphered_m;
    mpz_init(deciphered_m);
    rsa_decrypt(deciphered_m, cipher, d,
                n); // decrypt cipher and store into deciphered_m

    mpz_export(kblock, &bytes_read, 1, sizeof(uint8_t), 1, 0,
               deciphered_m); // bytes_read should be k-1 unless last block

    if (bytes_read < (k - 1)) { // if the amount of bytes read are less than k,
                                // then we reached the last byte of the file

      fwrite(kblock + 1, sizeof(uint8_t), bytes_read - 1, outfile);

      mpz_clear(cipher);
      mpz_clear(deciphered_m);
      break;
    }

    fwrite(kblock + 1, sizeof(uint8_t), bytes_read - 1,
           outfile); // help from TA Zack Jorquera

    mpz_clear(cipher);
    mpz_clear(deciphered_m);
  }

  free(kblock);
}

void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
  mpz_powm(s, m, d, n);
  pow_mod(s, m, d, n);
}

bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
  mpz_t t;
  mpz_init(t);
  pow_mod(t, s, e, n); // decrypt signature with public exponent and n (p x q)

  if (mpz_cmp(t, m) == 0) { // if the decrypted signature and username line up,
                            // then we can confirm the verification
    mpz_clear(t);
    return true;
  } else {
    mpz_clear(t);
    return false;
  }
}
