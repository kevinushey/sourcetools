library(sourcetools)
library(microbenchmark)

file <- tempfile()
unlink(file)

n <- 1024
for (i in seq_len(1E3)) {
  junk <- paste(sample(letters, n, TRUE), collapse = "\n")
  cat(junk, file = file, sep = "\n", append = TRUE)
}

stopifnot(identical(
  read(file),
  readChar(file, file.info(file)$size, TRUE)
))

stopifnot(identical(
  readLines(file),
  read_lines(file)
))

mb <- microbenchmark(
  S   = read(file),
  R   = readChar(file, file.info(file)$size, TRUE),
  SL  = read_lines(file),
  RL  = readLines(file),
  RRL = readr::read_lines(file)
)
print(mb)
