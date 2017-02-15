library(sourcetools)
library(microbenchmark)

file <- tempfile()

n <- 1024
junk <- replicate(1E4, {
  paste(sample(letters, n, TRUE), collapse = "")
})
writeLines(junk, con = file)

stopifnot(identical(
  read(file),
  readChar(file, file.info(file)$size, TRUE)
))

stopifnot(identical(
  readLines(file),
  read_lines(file)
))

# read a file into a string
mb <- microbenchmark(
  sourcetools::read(file),
  base::readChar(file, file.info(file)$size, TRUE),
  readr::read_file(file)
)
print(mb)

# read a file, splitting on newline characters
mb <- microbenchmark(
  sourcetools::read_lines(file),
  base::readLines(file),
  readr::read_lines(file, progress = FALSE)
)
print(mb)

unlink(file)
