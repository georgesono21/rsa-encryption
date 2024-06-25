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

int main(int argc, char **argv) {
  randstate_init(69420); // initializing randstate

  int opt = 0;

  // setting default values

  // bool trackers to check if we are going to be using stdin and stdout for
  // input/output
  int input_stdin = 1;
  int output_stdout = 1;

  char *input_file_name = (char *)(calloc(sizeof(char), 4096));
  char *output_file_name = (char *)(calloc(sizeof(char), 4096));
  char *pb_file_name = (char *)(calloc(sizeof(char), 4096));

  // if file pointers return null
  if (input_file_name == NULL) {
    fprintf(stderr, "No more memory!\n");
    return 1;
  }

  if (output_file_name == NULL) {
    fprintf(stderr, "No more memory!\n");
    return 1;
  }

  if (pb_file_name == NULL) {
    fprintf(stderr, "No more memory!\n");
    return 1;
  }

  strcpy(pb_file_name, "rsa.pub");

  int verbose = 0;

  // while loop to read getopt command line args
  while ((opt = getopt(argc, argv, "i:o:n:vh")) != -1) {
    switch (opt) {
    case 'i': // input file name
      strcpy(input_file_name, optarg);
      input_stdin = 0;
      break;
    case 'o': // setting output file name
      strcpy(output_file_name, optarg);
      output_stdout = 0;
      break;

    case 'n': // public key file name
      strcpy(pb_file_name, optarg);
      break;

    case 'v': // verbose
      verbose = 1;
      break;

    case 'h': // help message
      fprintf(stderr, "Usage: ./encrypt [options]\n");
      fprintf(stderr, "  ./encrypt encrypts an input file using the specified "
                      "public key file,\n");
      fprintf(stderr, "  writing the result to the specified output file.\n");
      fprintf(stderr, "    -i <infile> : Read input from <infile>. Default: "
                      "standard input.\n");
      fprintf(stderr, "    -o <outfile>: Write output to <outfile>. Default: "
                      "standard output.\n");
      fprintf(
          stderr,
          "    -n <keyfile>: Public key is in <keyfile>. Default: rsa.pub.\n");
      fprintf(stderr, "    -v          : Enable verbose output.\n");
      fprintf(stderr,
              "    -h          : Display program synopsis and usage.\n");

      free(pb_file_name);
      free(output_file_name);
      free(input_file_name);

      return 0;
    default: // if the user has an invalid option, print help message and return
             // a non zero exit code
      fprintf(stderr, "Usage: ./encrypt [options]\n");
      fprintf(stderr, "  ./encrypt encrypts an input file using the specified "
                      "public key file,\n");
      fprintf(stderr, "  writing the result to the specified output file.\n");
      fprintf(stderr, "    -i <infile> : Read input from <infile>. Default: "
                      "standard input.\n");
      fprintf(stderr, "    -o <outfile>: Write output to <outfile>. Default: "
                      "standard output.\n");
      fprintf(
          stderr,
          "    -n <keyfile>: Public key is in <keyfile>. Default: rsa.pub.\n");
      fprintf(stderr, "    -v          : Enable verbose output.\n");
      fprintf(stderr,
              "    -h          : Display program synopsis and usage.\n");
      free(pb_file_name);
      free(output_file_name);
      free(input_file_name);
      return 1;
    }
  }

  mpz_t n;
  mpz_init(n);
  mpz_t e;
  mpz_init(e);
  mpz_t s;
  mpz_init(s);

  FILE *input_file;
  FILE *output_file;
  FILE *pb_file;

  // how i learned to set file pointers to stdin and stdout:
  // link:
  // https://stackoverflow.com/questions/18505530/how-to-set-a-file-variable-to-stdout
  // more info in the README.md file

  if (input_stdin == 0) { // if the user specified an input file
    input_file = fopen(input_file_name, "r");
    if (input_file == NULL) {
      fprintf(stderr,
              "encrypt: Couldn't open %s to read plaintext: No such file or "
              "directory\n",
              input_file_name);
      return 1;
    }
  } else {
    input_file = stdin;
  }

  if (output_stdout == 0) { // if the user specified an output file
    output_file = fopen(output_file_name, "w");
  } else {
    output_file = stdout;
  }

  // reading the public key
  pb_file = fopen(pb_file_name, "r");

  if (pb_file ==
      NULL) { // if the pb_file pointer returns null, the file doesn't exist in
              // the directory or theres another error
    fprintf(stderr, "./encrypt: couldn't open %s to read public key.\n",
            pb_file_name);
    return 1;
  }

  char *username = (char *)(calloc(sizeof(char), 4096));

  if (username == NULL) { // if pointer returns null, there is an error (return
                          // non-zero code)
    fprintf(stderr, "No more memory!\n");
    return 1;
  }

  rsa_read_pub(n, e, s, username,
               pb_file); // values for n,e,s,username should be filled

  if (verbose == 1) { // if verbose is on
    fprintf(stderr, "username: %s\n", username);
    gmp_fprintf(stderr, "user signature (%zu bits): %Zd\n",
                mpz_sizeinbase(s, 2), s);
    gmp_fprintf(stderr, "n - modulus (%zu bits): %Zd\n", mpz_sizeinbase(n, 2),
                n);
    gmp_fprintf(stderr, "e - public exponent (%zu bits): %Zd\n",
                mpz_sizeinbase(e, 2), e);
  }

  mpz_t mpz_username;
  mpz_init_set_str(mpz_username, username, 62);
  // this could throw error because rsa_read literally just assigns the str.
  // literal and not strcpy (possible invalid pointer bug) update: this did not
  // throw an error. valgrind did not complain.

  int verified =
      rsa_verify(mpz_username, s, e,
                 n);   // storing the result of the verification into verified
  if (verified == 1) { // if they are verified
    // printf("verified\n");
    rsa_encrypt_file(input_file, output_file, n, e);
    // rsa_encrypt_file(input_file, output_file, n, e);
  } else {
    fprintf(stderr, "Decrypted signature and username do not lineup.\n");
    mpz_clear(e);
    mpz_clear(n);
    mpz_clear(s);
    mpz_clear(mpz_username);

    if (input_stdin == 0) {
      fclose(input_file);
    }
    if (output_stdout == 0) {
      fclose(output_file);
    }

    free(input_file_name);
    free(output_file_name);
    free(pb_file_name);
    free(username);

    randstate_clear();
    return 1;
  }

  mpz_clear(e);
  mpz_clear(n);
  mpz_clear(s);
  mpz_clear(mpz_username);

  // closing files

  fclose(pb_file);

  // we cannot close stdin and stdout, so we have to check if the input and
  // output were stdin or stdout
  if (input_stdin == 0) {
    fclose(input_file);
  }
  if (output_stdout == 0) {
    fclose(output_file);
  }

  // freeing dynamically allocated arrays
  free(input_file_name);
  free(output_file_name);
  free(pb_file_name);
  free(username);

  randstate_clear();
  return 0;
}
