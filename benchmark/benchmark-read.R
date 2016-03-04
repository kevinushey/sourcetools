library(microbenchmark)
n <- 1E6
junk <- paste(sample(letters, n, TRUE), collapse = "\n")

file <- tempfile()
cat(junk, file = file)

stopifnot(identical(junk, read(file)))
stopifnot(identical(junk, readChar(file, file.info(file)$size, TRUE)))

library(microbenchmark)
microbenchmark(
  S = read(file),
  R = readChar(file, file.info(file)$size, TRUE)
)
