# sourcetools 0.1.1

- Rename token type `ERR` to `INVALID` to fix build errors
  on Solaris.

# sourcetools 0.1.0

## Features

The first release of `sourcetools` comes with a small set
of features exposed to R:

- `read(file)`: Read a file (as a string). Similar to
  `readChar()`, but faster (and maybe be optimized to
  use a memory mapped file reader in the future).

- `tokenize_file(file)`: Tokenize an R script.

- `tokenize_string(string)`: Tokenize a string of R code.
