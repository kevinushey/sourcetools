

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
`R` as character vectors. `read_lines()` handles both
Windows style `\r\n` line endings, as well as Unix-style
`\n` endings. Performance is on par with the readers
provided by the
[readr](https://cran.r-project.org/package=readr) package.


```r
text <- replicate(10000, {
  paste(sample(letters, 200, TRUE), collapse = "")
})
file <- tempfile()
cat(text, file = file, sep = "\n")
mb <- microbenchmark::microbenchmark(times = 10,
  base::readLines(file),
  readr::read_lines(file),
  sourcetools::read_lines(file)
)
sm <- summary(mb)
print(sm[c("expr", "mean", "median")], digits = 3)
```

```
##                            expr  mean median
## 1         base::readLines(file) 164.5  160.8
## 2       readr::read_lines(file)  15.1   13.3
## 3 sourcetools::read_lines(file)  15.2   15.0
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
