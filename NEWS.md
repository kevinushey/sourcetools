
## sourcetools 0.2.0 (UNRELEASED)

- Remove calls to `std::sprintf()`.

- Support `=>` pipe-bind operator, to be introduced in R 4.1.0.

- Support `|>` pipe operator, to be introduced in R 4.1.0.

- Support raw string syntax, introduced in R 4.0.0.

## sourcetools 0.1.7

- Ensure tests pass on platforms where `char` is unsigned. (#21)

## sourcetools 0.1.6

- Register native routines.

## sourcetools 0.1.5

- More work to ensure `sourcetools` can build on Solaris.

## sourcetools 0.1.4

- More work to ensure `sourcetools` can build on Solaris.

## sourcetools 0.1.3

- Relax C++11 requirement, to ensure that `sourcetools` can
  build on machines with older compilers (e.g. gcc 4.4).
  
## sourcetools 0.1.2

- Disable failing tests on Solaris.

## sourcetools 0.1.1

- Rename token type `ERR` to `INVALID` to fix build errors
  on Solaris.

## sourcetools 0.1.0

### Features

The first release of `sourcetools` comes with a small set
of features exposed to R:

- `read(file)`: Read a file (as a string). Similar to
  `readChar()`, but faster (and maybe be optimized to
  use a memory mapped file reader in the future).

- `tokenize_file(file)`: Tokenize an R script.

- `tokenize_string(string)`: Tokenize a string of R code.
