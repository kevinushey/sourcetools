library(sourcetools)
library(microbenchmark)

n <- 1E6
junk <- paste(sample(letters, n, TRUE), collapse = "\n")

file <- tempfile()
cat(junk, file = file, sep = "\n")

stopifnot(identical(
  read(file),
  readChar(file, file.info(file)$size, TRUE)
))

stopifnot(identical(
  readLines(file),
  read_lines(file)
))

mb <- microbenchmark(
  S = read(file),
  R = readChar(file, file.info(file)$size, TRUE),
  SL = read_lines(file),
  SR = readLines(file)
)
print(mb)
