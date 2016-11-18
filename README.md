

[![Travis-CI Build Status](https://travis-ci.org/kevinushey/sourcetools.svg?branch=master)](https://travis-ci.org/kevinushey/sourcetools) [![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/github/kevinushey/sourcetools?branch=master&svg=true)](https://ci.appveyor.com/project/kevinushey/sourcetools)


# sourcetools

Tools for reading, tokenizing, and (eventually) parsing `R` code.

## Getting Started

You can install `sourcetools` from CRAN with:

```r
install.packages("sourcetools")
```

Or, you can install the development version from GitHub with:


```r
devtools::install_github("kevinushey/sourcetools")
```

## Reading

`sourcetools` comes with a couple fast functions for reading
files into `R`.

Use `read()` and `read_lines()` to quickly read a file into
`R` as character vectors. `read_lines()` handles both Windows
style `\r\n` line endings, as well as Unix-style `\n` endings.


```r
text <- replicate(10000, paste(sample(letters, 200, TRUE), collapse = ""))
file <- tempfile()
cat(text, file = file, sep = "\n")
mb <- microbenchmark::microbenchmark(times = 10,
  readChar   = readChar(file, file.info(file)$size, TRUE),
  readLines  = readLines(file),
  read       = read(file),
  read_lines = read_lines(file)
)
print(mb, digits = 3)
```

```
## Unit: milliseconds
##        expr    min     lq   mean median     uq    max neval
##    readChar   4.53   4.91   5.24   5.37   5.47   5.89    10
##   readLines 156.69 157.84 160.16 159.79 161.29 165.53    10
##        read   4.62   4.77   5.17   4.96   5.48   6.31    10
##  read_lines  12.95  14.31  14.61  14.42  15.05  16.90    10
```

```r
unlink(file)
```

## Tokenization

`sourcetools` provides the `tokenize_string()` and
`tokenize_file()` functions for generating a tokenized
representation of R code. These produce 'raw' tokenized
representations of the code, with each token's value as a
string, and a recorded row, column, and type:


```r
tokenize_string("if (x < 10) 20")
```

```
##    value row column       type
## 1     if   1      1    keyword
## 2          1      3 whitespace
## 3      (   1      4    bracket
## 4      x   1      5     symbol
## 5          1      6 whitespace
## 6      <   1      7   operator
## 7          1      8 whitespace
## 8     10   1      9     number
## 9      )   1     11    bracket
## 10         1     12 whitespace
## 11    20   1     13     number
```
