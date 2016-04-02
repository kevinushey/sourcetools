

[![Travis-CI Build Status](https://travis-ci.org/kevinushey/sourcetools.svg?branch=master)](https://travis-ci.org/kevinushey/sourcetools) [![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/github/kevinushey/sourcetools?branch=master&svg=true)](https://ci.appveyor.com/project/kevinushey/sourcetools)


# sourcetools

Tools for reading, tokenizing, and (eventually) parsing `R` code.

## Getting Started

`sourcetools` is not yet on CRAN -- install with


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
##        expr   min     lq  mean median     uq    max neval cld
##    readChar   5.2   6.54  10.5   7.02   8.73  36.56    10 ab 
##   readLines 155.9 159.69 162.4 161.95 163.15 171.76    10   c
##        read   5.3   5.48   6.5   5.97   7.52   9.35    10 a  
##  read_lines  13.5  13.95  14.4  14.09  14.50  16.97    10  b
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
