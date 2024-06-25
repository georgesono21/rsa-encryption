#pragma once

#include <gmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

//
// Generates the components for a new public RSA key.
// p and q will be large primes with n their product.
// The product n will be of a specified minimum number of bits.
// The primality is tested using Miller-Rabin.
// The public exponent e will have around the same number of bits as n.
// All mpz_t arguments are expected to be initialized.
//
// p: will store the first large prime.
// q: will store the second large prime.
// n: will store the product of p and q.
// e: will store the public exponent.
//
void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits,
                  uint64_t iters);

//
// Writes a public RSA key to a file.
// Public key contents: n, e, signature, username.
// All mpz_t arguments are expected to be initialized.
//
// n: the public modulus.
// e: the public exponent.
// s: the signature of the username.
// username: the username that was signed as s.
//
void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile);

//
// Reads a public RSA key from a file.
// Public key contents: n, e, signature, username.
// All mpz_t arguments are expected to be initialized.
//
// n: will store the public modulus.
// e: will store the public exponent.
// s: will store the signature.
// username: an allocated array to hold the username.
// pbfile: the file containing the public key
//
void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile);

//
// Generates the components for a new private RSA key.
// Requires an accompanying RSA public key to complete the pair.
// All mpz_t arguments are expected to be initialized.
//
// d: will store the RSA private key.
// e: the precomputed public exponent.
// p: the first large prime from the public key generation.
// p: the second large prime from the public key generation.
//
void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q);

//
// Writes a private RSA key to a file.
// Private key contents: n, d.
// All mpz_t arguments are expected to be initialized.
//
// n: the public modulus.
// d: the private key.
// pvfile: the file to write the private key to.
//
void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile);

//
// Reads a private RSA key from a file.
// Private key contents: n, d.
// All mpz_t arguments are expected to be initialized.
//
// n: will store the public modulus.
// d: will store the private key.
void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile);

//
// Encrypts a message given an RSA public exponent and modulus.
// All mpz_t arguments are expected to be initialized.
//
// c: will store the encrypted message.
// m: the message to encrypt.
// e: the public exponent.
// n: the public modulus.
//
void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n);

//
// Encrypts an entire file given an RSA public modulus and exponent.
// All mpz_t arguments are expected to be initialized.
// All FILE * arguments are expected to be properly opened.
//
// infile: the input file to encrypt.
// outfile: the output file to write the encrypted input to.
// n: the public modulus.
// e: the public exponent.
//
void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e);

//
// Decrypts some ciphertext given an RSA private key and public modulus.
// All mpz_t arguments are expected to be initialized.
//
// m: will store the decrypted message.
// c: the ciphertext to decrypt.
// d: the private key.
// n: the public modulus.
//
void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n);

//
// Decrypts an entire file given an RSA public modulus and private key.
// All mpz_t arguments are expected to be initialized.
// All FILE * arguments are expected to be properly opened.
//
// infile: the input file to decrypt.
// outfile: the output file to write the decrypted input to.
// n: the public modulus.
// d: the private key.
//
void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d);

//
// Signs some message given an RSA private key and public modulus.
// All mpz_t arguments are expected to be initialized.
//
// s: will store the signed message (the signature).
// m: the message to sign.
// d: the private key.
// n: the public modulus.
//
void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n);

//
// Verifies some signature given an RSA public exponent and modulus.
// Requires the expected message for verification.
// All mpz_t arguments are expected to be initialized.
//
// m: the expected message.
// s: the signature to verify.
// e: the public exponent.
// n: the public modulus.
// returns: true if signature is verified, false otherwise.
//
bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n);
