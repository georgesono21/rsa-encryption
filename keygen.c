
// clang-format off
#include <stdio.h>
#include <assert.h>
#include <gmp.h>
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"

// clang-format on

gmp_randstate_t state;

int main(int argc, char **argv) { // allows compiled file to get command line
                                  // args (usually its void)
  int opt = 0;

  // setting default values

  uint64_t nbits = 1024;
  uint64_t iters = 50;

  // dynamically allocate pb_file_names and pv_file_names to heap memory (so we
  // have more space in stack)

  char *pb_file_name = (char *)(calloc(sizeof(char), 4096));
  char *pv_file_name = (char *)(calloc(sizeof(char), 4096));

  if (pv_file_name == NULL) { // check if pointers return null
    fprintf(stderr, "No more memory!\n");
    return 1;
  }

  if (pb_file_name == NULL) { // check if pointers return null
    fprintf(stderr, "No more memory!\n");
    return 1;
  }

  // credit to this stack overflow article for redefining strings
  // //https://stackoverflow.com/questions/10063222/freeing-strings-in-c more
  // info in README
  strcpy(pb_file_name, "rsa.pub");
  strcpy(pv_file_name, "rsa.priv");

  uint64_t seed = time(NULL);
  int verbose = 0;

  // while loop to read getopt command line args
  while ((opt = getopt(argc, argv, "b:i:n:d:s:vh")) != -1) {
    switch (opt) {
    case 'b': // specifies bits
      nbits = strtoul(optarg, NULL, 10);
      if ((nbits > 4096) ||
          (nbits < 50)) { // if nbits are not in range, print help message and
                          // return non-zero exit code
        fprintf(stderr, "Number of bits must be 50-4096, not %lu.\n", nbits);
        fprintf(stderr, "Usage: ./keygen [options]\n");
        fprintf(stderr, "  ./keygen generates a public / private key pair, "
                        "placing the keys into the public and private\n");
        fprintf(stderr, "  key files as specified below. The keys have a "
                        "modulus (n) whose length is specified in\n");
        fprintf(stderr, "  the program options.\n");
        fprintf(stderr, "    -s <seed>   : Use <seed> as the random number "
                        "seed. Default: time()\n");
        fprintf(stderr, "    -b <bits>   : Public modulus n must have at least "
                        "<bits> bits. Default: 1024\n");
        fprintf(stderr, "    -i <iters>  : Run <iters> Miller-Rabin iterations "
                        "for primality testing. Default: 50\n");
        fprintf(stderr, "    -n <pbfile> : Public key file is <pbfile>. "
                        "Default: rsa.pub\n");
        fprintf(stderr, "    -d <pvfile> : Private key file is <pvfile>. "
                        "Default: rsa.priv\n");
        fprintf(stderr, "    -v          : Enable verbose output.\n");
        fprintf(stderr,
                "    -h          : Display program synopsis and usage.\n");

        free(pb_file_name);
        free(pv_file_name);
        return 1;
      }

      break;

    case 'i': // setting iters for is_prime
      iters = strtoul(optarg, NULL, 10);
      if ((iters > 500) ||
          (iters < 1)) { // if iters are not in range, print help message and
                         // return non-zero exit code
        fprintf(stderr, "Number of bits must be 50-4096, not %lu.\n", nbits);
        fprintf(stderr, "Usage: ./keygen [options]\n");
        fprintf(stderr, "  ./keygen generates a public / private key pair, "
                        "placing the keys into the public and private\n");
        fprintf(stderr, "  key files as specified below. The keys have a "
                        "modulus (n) whose length is specified in\n");
        fprintf(stderr, "  the program options.\n");
        fprintf(stderr, "    -s <seed>   : Use <seed> as the random number "
                        "seed. Default: time()\n");
        fprintf(stderr, "    -b <bits>   : Public modulus n must have at least "
                        "<bits> bits. Default: 1024\n");
        fprintf(stderr, "    -i <iters>  : Run <iters> Miller-Rabin iterations "
                        "for primality testing. Default: 50\n");
        fprintf(stderr, "    -n <pbfile> : Public key file is <pbfile>. "
                        "Default: rsa.pub\n");
        fprintf(stderr, "    -d <pvfile> : Private key file is <pvfile>. "
                        "Default: rsa.priv\n");
        fprintf(stderr, "    -v          : Enable verbose output.\n");
        fprintf(stderr,
                "    -h          : Display program synopsis and usage.\n");

        free(pb_file_name);
        free(pv_file_name);
        return 1;
      }
      break;

    case 'n': // public key file name

      strcpy(pb_file_name, optarg);
      break;

    case 'd': // private key file name

      strcpy(pv_file_name, optarg);
      break;

    case 's': // seed
      seed = strtoul(optarg, NULL, 10);
      break;

    case 'v': // verbose
      verbose = 1;
      break;

    case 'h': // help message

      fprintf(stderr, "Usage: ./keygen [options]\n");
      fprintf(stderr, "  ./keygen generates a public / private key pair, "
                      "placing the keys into the public and private\n");
      fprintf(stderr, "  key files as specified below. The keys have a modulus "
                      "(n) whose length is specified in\n");
      fprintf(stderr, "  the program options.\n");
      fprintf(stderr, "    -s <seed>   : Use <seed> as the random number seed. "
                      "Default: time()\n");
      fprintf(stderr, "    -b <bits>   : Public modulus n must have at least "
                      "<bits> bits. Default: 1024\n");
      fprintf(stderr, "    -i <iters>  : Run <iters> Miller-Rabin iterations "
                      "for primality testing. Default: 50\n");
      fprintf(
          stderr,
          "    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n");
      fprintf(stderr, "    -d <pvfile> : Private key file is <pvfile>. "
                      "Default: rsa.priv\n");
      fprintf(stderr, "    -v          : Enable verbose output.\n");
      fprintf(stderr,
              "    -h          : Display program synopsis and usage.\n");

      free(pb_file_name);
      free(pv_file_name);
      return 0;
    default: // if the user has an invalid option, print help message and return
             // a non zero exit code
      fprintf(stderr, "Usage: ./keygen [options]\n");
      fprintf(stderr, "  ./keygen generates a public / private key pair, "
                      "placing the keys into the public and private\n");
      fprintf(stderr, "  key files as specified below. The keys have a modulus "
                      "(n) whose length is specified in\n");
      fprintf(stderr, "  the program options.\n");
      fprintf(stderr, "    -s <seed>   : Use <seed> as the random number seed. "
                      "Default: time()\n");
      fprintf(stderr, "    -b <bits>   : Public modulus n must have at least "
                      "<bits> bits. Default: 1024\n");
      fprintf(stderr, "    -i <iters>  : Run <iters> Miller-Rabin iterations "
                      "for primality testing. Default: 50\n");
      fprintf(
          stderr,
          "    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n");
      fprintf(stderr, "    -d <pvfile> : Private key file is <pvfile>. "
                      "Default: rsa.priv\n");
      fprintf(stderr, "    -v          : Enable verbose output.\n");
      fprintf(stderr,
              "    -h          : Display program synopsis and usage.\n");

      free(pb_file_name);
      free(pv_file_name);
      return 1;
    }
  }

  // initilize randstate
  randstate_init(seed);

  FILE *pb_file;
  FILE *pv_file;

  mpz_t p;
  mpz_init(p);
  mpz_t q;
  mpz_init(q);
  mpz_t e;
  mpz_init(e);
  mpz_t n;
  mpz_init(n);
  mpz_t d;
  mpz_init(d);

  // opening files

  pb_file = fopen(pb_file_name, "w");

  if (pb_file == NULL) { // if the file pointer returns null, we have an error,
                         // return non-zero code
    fprintf(stderr, "No more memory!\n");
    return 1;
  }

  pv_file = fopen(pv_file_name, "w");

  if (pv_file == NULL) { // if the file pointer returns null, we have an error,
                         // return non-zero code
    fprintf(stderr, "No more memory!\n");
    return 1;
  }

  // setting file permissions for pv_file
  int file_num = fileno(pv_file);
  fchmod(file_num, 0600);

  char *username = (char *)(calloc(
      sizeof(char),
      4096)); // credit to Lev Teytelman for telling me about the buffersize

  if (username == NULL) { // check if pointers return null
    fprintf(stderr, "No more memory!\n");
    return 1;
  }

  strcpy(username, getenv("USER"));
  mpz_t mpz_username;
  mpz_init_set_str(mpz_username, username,
                   62); // converting username into mpz for signature

  // making public and private keys
  rsa_make_pub(p, q, n, e, nbits, iters);
  rsa_make_priv(d, e, p, q);

  // s stores the signature from rsa_sign
  mpz_t s;
  mpz_init(s);
  rsa_sign(s, mpz_username, d, n);

  rsa_write_pub(n, e, s, username, pb_file);
  rsa_write_priv(n, d, pv_file);

  if (verbose == 1) { // if verbose is on
    fprintf(stderr, "username: %s\n", username);
    gmp_fprintf(stderr, "user signature (%zu bits): %Zd\n",
                mpz_sizeinbase(s, 2), s);
    gmp_fprintf(stderr, "p (%zu bits): %Zd\n", mpz_sizeinbase(p, 2), p);
    gmp_fprintf(stderr, "q (%zu bits): %Zd\n", mpz_sizeinbase(q, 2), q);
    gmp_fprintf(stderr, "n - modulus (%zu bits): %Zd\n", mpz_sizeinbase(n, 2),
                n);
    gmp_fprintf(stderr, "e - public exponent (%zu bits): %Zd\n",
                mpz_sizeinbase(e, 2), e);
    gmp_fprintf(stderr, "d - private exponent(%zu bits): %Zd\n",
                mpz_sizeinbase(d, 2), d);
  }

  // free mpz_variables and other heap memory allocations

  mpz_clear(p);
  mpz_clear(q);
  mpz_clear(e);
  mpz_clear(n);
  mpz_clear(d);
  mpz_clear(s);
  mpz_clear(mpz_username);

  fclose(pb_file);
  fclose(pv_file);

  free(pb_file_name);
  free(pv_file_name);
  free(username);

  randstate_clear();

  return 0;
}
